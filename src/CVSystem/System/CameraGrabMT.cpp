#include "CameraGrabMT.h"
#include <chrono>
#include <memory>
#include "dlog/dlog.h"

#include "Poco/ThreadPool.h"
#include "Poco/TaskManager.h"

#include "../Model/CameraImageQueue.h"

#include <atomic>

namespace dxlib {

/**
 * 4个Task不停的抓图添加图片到队列.
 *
 * @author daixian
 * @date 2020/11/26
 */
class TaskGrabOneCamera : public Poco::Runnable
{
  public:
    /**
     * 构造,需要指定一个图片队列,一个采图方法工厂.
     *
     * @author daixian
     * @date 2020/11/26
     *
     * @param      name                The name.
     * @param [in] imageQueue          If non-null, queue of images.
     * @param [in] pCameraImageFactory If non-null, the camera image factory.
     */
    TaskGrabOneCamera(const std::string& name,
                      CameraImageQueue* imageQueue,
                      ICameraImageFactory* pCameraImageFactory)
        : imageQueue(imageQueue),
          pCameraImageFactory(pCameraImageFactory) {}

    virtual ~TaskGrabOneCamera() {}

    CameraImageQueue* imageQueue = nullptr;
    ICameraImageFactory* pCameraImageFactory = nullptr;

    std::atomic_bool isRun{true};

    virtual void run()
    {
        if (imageQueue == nullptr) {
            LogE("TaskGrabOneCamera.runTask():imageQueue == nullptr");
            return;
        }
        if (pCameraImageFactory == nullptr) {
            LogE("TaskGrabOneCamera.runTask():pCameraImageFactory == nullptr");
            return;
        }

        while (isRun.load()) {
            LogD("TaskGrabOneCamera.runTask():CameraImageFactory执行采图!");
            std::vector<CameraImage> images = pCameraImageFactory->Create();
            for (size_t i = 0; i < images.size(); i++) {
                //如果采图成功才放进队列好了,当相机有切换相机属性的时候是会产生硬件的采图失败的.
                if (images[i].isSuccess) {
                    imageQueue->PushImage(images[i]);
                }
            }
        }
    }

  private:
};

class CameraGrabMT::Impl
{
  public:
    Impl()
    {
    }
    ~Impl() {}

    Poco::ThreadPool threadPool{4, 8};

    CameraImageQueue imageQueue;

    std::vector<TaskGrabOneCamera*> curTasks;

  private:
};

CameraGrabMT::CameraGrabMT(const std::vector<pCamera>& vcameras,
                           const std::vector<pCameraDevice>& vdevices,
                           const std::vector<pCameraImageFactory>& vcameraImageFactory)
    : vCameras(vcameras),
      vDevices(vdevices),
      vCameraImageFactory(vcameraImageFactory)
{
    _impl = new Impl();

    //初始化imageQueue
    for (size_t i = 0; i < vCameras.size(); i++) {
        _impl->imageQueue.AddCamera(vCameras[i].get());
    }
}

CameraGrabMT::~CameraGrabMT()
{
    delete _impl;
}

void CameraGrabMT::startGrab()
{
    LogI("CameraGrabMT.startGrab():启动采图任务...");
    for (size_t i = 0; i < vCameraImageFactory.size(); i++) {
        auto& pCameraImageFactory = vCameraImageFactory[i];
        try {
            TaskGrabOneCamera* task = new TaskGrabOneCamera(pCameraImageFactory->device->devName,
                                                            &_impl->imageQueue,
                                                            pCameraImageFactory.get());
            _impl->curTasks.push_back(task);
            _impl->threadPool.startWithPriority(Poco::Thread::Priority::PRIO_HIGHEST, *task);
        }
        catch (const std::exception& e) {
            LogE("CameraGrabMT.startGrab():异常 %s", e.what());
        }
    }
}

bool CameraGrabMT::tryGet(pCameraImageGroup& result)
{
    //执行完毕之后就更新相机的属性状态
    for (size_t camIndex = 0; camIndex < vDevices.size(); camIndex++) {
        if (vDevices[camIndex] != nullptr) {
            vDevices[camIndex]->applyCapProp();
        }
    }
    //构造采图结果：使用记录的相机的结构体来创建新的结果文件
    result = _impl->imageQueue.GetImage();

    if (result != nullptr) {
        return true;
    }
    else {
        return false;
    }
}

bool CameraGrabMT::open()
{
    if (vDevices.size() == 0) {
        LogW("CameraGrabMT.open():事先没有录入有效的相机（vCameras.size()==0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < vDevices.size(); camIndex++) {
        auto& device = vDevices[camIndex];
        if (device == nullptr) {
            LogI("CameraGrabMT.open():相机index%d 为null", camIndex);
            continue;
        }

        LogI("CameraGrabMT.open():尝试打开相机 %s ...", device->devName.c_str());
        clock_t startTime = clock();
        //打开相机
        if (device->open()) {
            double costTime = (clock() - startTime) / CLOCKS_PER_SEC * 1000.;
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            device->capture->read(img);
            if (!img.empty())
                LogI("CameraGrabMT.open():成功打开一个相机%s，耗时%.2f毫秒", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
            else
                LogE("CameraGrabMT.open():成功打开一个相机%s，耗时%.2f毫秒,但是尝试读取一帧图片失败!", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
        }
        else {
            isSuccess = false;
        }
    }

    return isSuccess;
}

bool CameraGrabMT::close()
{
    for (size_t i = 0; i < _impl->curTasks.size(); i++) {
        _impl->curTasks[i]->isRun = false;
    }
    LogI("CameraGrabMT.close():等待所有采图任务退出...");
    _impl->threadPool.joinAll();
    //删除这些线程
    for (size_t i = 0; i < _impl->curTasks.size(); i++) {
        delete _impl->curTasks[i];
    }

    for (size_t i = 0; i < vDevices.size(); i++) {
        if (vDevices[i] != nullptr) {
            LogI("CameraGrabMT.close():释放相机设备%s ...", vDevices[i]->devName.c_str());
            vDevices[i]->release();
        }
    }

    clear(); //释放数据结构
    return true;
}

bool CameraGrabMT::isAllCameraClosed()
{
    for (size_t i = 0; i < vDevices.size(); i++) {
        if (vDevices[i] != nullptr) {
            //只要有一个相机还打开着就返回false
            if (vDevices[i]->isOpened()) {
                return false;
            }
        }
    }
    return true;
}

void CameraGrabMT::clear()
{
    vCameras.clear();
    vDevices.clear();
    vCameraImageFactory.clear();
    _impl->curTasks.clear();
    _impl->imageQueue.Clear();
}
} // namespace dxlib