#pragma once

#include <chrono>
#include <memory>

#include "../Model/CameraImageGroup.h"
#include "../Model/ICameraImageFactory.h"
#include "../Model/CameraImageQueue.h"

#include "dlog/dlog.h"

#include "Poco/ThreadPool.h"
#include "Poco/TaskManager.h"

#include "FrameProc.h"

#include <atomic>

#include "../Common/CoolTime.hpp"
#include "../Common/Event.h"
#include "../Common/FPSCalc.hpp"

namespace dxlib {

/**
 * 多个相机设备使用一个Task不停的抓图添加图片到队列.
 *
 * @author daixian
 * @date 2020/11/26
 */
class TaskGrabManyCamera : public Poco::Runnable
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
    TaskGrabManyCamera(const std::string& name,
                       CameraImageQueue* imageQueue,
                       const std::vector<ICameraImageFactory*>& vCameraImageFactory)
        : imageQueue(imageQueue),
          vCameraImageFactory(vCameraImageFactory) {}

    virtual ~TaskGrabManyCamera() {}

    // 是否运行
    std::atomic_bool isRun{true};

    // 图片队列
    CameraImageQueue* imageQueue = nullptr;

    // 图片工厂
    std::vector<ICameraImageFactory*> vCameraImageFactory;

    // 是否执行采图(如果不采图了那么会降低cpu开销).
    std::atomic_bool isGrab{true};

    // 是否是主任务(只有主任务才执行事件)
    bool isMainTask = false;

    // 是否执行处理
    bool isDoProc = false;

    // proc对象.
    pFrameProc proc = nullptr;

    // 处理的fps.
    float fps = 0;

    // 当前按下的按键记录（看后面要不要删掉）.
    std::atomic_int cvKey{-1};

    /**
     * 线程执行函数.
     *
     * @author daixian
     * @date 2020/11/27
     */
    virtual void run()
    {
        if (imageQueue == nullptr) {
            LogE("TaskGrabManyCamera.runTask():imageQueue == nullptr");
            return;
        }
        if (vCameraImageFactory.empty()) {
            LogE("TaskGrabManyCamera.runTask():pCameraImageFactory == nullptr");
            return;
        }

        if (isGrab.load()) {
            //如果还没有打开相机就打开相机
            for (size_t factoryIndex = 0; factoryIndex < vCameraImageFactory.size(); factoryIndex++) {
                ICameraImageFactory* pCameraImageFactory = vCameraImageFactory[factoryIndex];
                if (!pCameraImageFactory->device->isOpened()) {
                    LogI("TaskGrabManyCamera.runTask():设备相机还没有打开,打开相机...");
                    if (pCameraImageFactory->device->open()) {
                        LogI("TaskGrabManyCamera.runTask():相机打开成功!");
                    }
                    else {
                        LogE("TaskGrabManyCamera.runTask():打开相机失败无法采图...");
                        isRun = false;
                        return;
                    }
                }
            }
        }

        if (isMainTask && isDoProc && proc != nullptr) {
            imageQueue->lockGetImage.lock();
            try {
                proc->onEnable();
            }
            catch (const std::exception& e) {
                LogE("TaskGrabManyCamera.run():执行onEnable异常 e=%s", e.what());
            }
            imageQueue->lockGetImage.unlock();
        }

        while (isRun.load()) {
            try {
                //使用imageQueue中的提取计数去计算一下处理的fps
                fps = _fpsCalc.update(imageQueue->frameCount);

                if (isGrab.load()) {
                    //如果需要采图那就执行一次采图
                    setCamerasFPS(fps);

                    for (size_t factoryIndex = 0; factoryIndex < vCameraImageFactory.size(); factoryIndex++) {
                        ICameraImageFactory* pCameraImageFactory = vCameraImageFactory[factoryIndex];
                        pCameraImageFactory->device->applyCapProp();

                        std::vector<CameraImage> images = pCameraImageFactory->Create();
                        for (size_t i = 0; i < images.size(); i++) {
                            //如果采图成功才放进队列好了,当相机有切换相机属性的时候是会产生硬件的采图失败的.
                            if (images[i].isSuccess) {
                                imageQueue->PushImage(images[i]);
                            }
                        }
                    }
                }
                else {
                    //如果不采图那么就标记这些相机的fps为0
                    setCamerasFPS(0);
                }
            }
            catch (const std::exception& e) {
                LogE("TaskGrabManyCamera.run():采图异常 e=%s", e.what());
            }

            try {
                // 如果这个采图完了它就去检察是否能够执行proc处理,如果没有人正在进行帧处理,那么就会持有一个锁
                if (isDoProc && proc != nullptr &&
                    imageQueue->lockGetImage.try_lock()) {
                    if (isGrab.load()) {
                        //因为这个主相机有可能采图超前一帧,所以这里执行两次处理
                        if (doOnceProc())
                            doOnceProc();
                    }
                    else {
                        LogD("TaskGrabManyCamera.run():执行onLightSleep!");
                        int ckey = -1; //让proc去自己想检测keydown就keydown
                        proc->onLightSleep(ckey);
                        if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                            cvKey.exchange(ckey);
                        }

                        //如果不抓图那么就睡眠
                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    }

                    //干脆用这个线程来驱动检查事件
                    Event::GetInst()->checkMemEvent();

                    imageQueue->lockGetImage.unlock(); //解锁
                }
            }
            catch (const std::exception& e) {
                LogE("TaskGrabManyCamera.run():执行proc异常 e=%s", e.what());
                imageQueue->lockGetImage.unlock(); //异常了也解锁
            }
        }

        if (isMainTask && isDoProc && proc != nullptr) {
            imageQueue->lockGetImage.lock();
            try {
                proc->onDisable();
            }
            catch (const std::exception& e) {
                LogE("TaskGrabManyCamera.run():执行onDisable异常 e=%s", e.what());
            }
            imageQueue->lockGetImage.unlock();
        }
    }

    /**
     * 执行一次帧处理,每个Task在采图完成之后就会去尝试进行帧处理.
     *
     * @author daixian
     * @date 2020/11/27
     *
     * @returns True if it succeeds, false if it fails.
     */
    bool doOnceProc()
    {
        //如果是采图过程中
        pCameraImageGroup cimg = imageQueue->GetImage();
        if (cimg != nullptr) {
            cimg->procStartTime = clock(); //标记处理开始时间
            if (cimg->waitProcTime() > 4)
                LogW("TaskGrabManyCamera.run():执行proc!,从采图结束到现在等待了%f ms", cimg->waitProcTime());

            cimg->fnum = imageQueue->frameCount;
            LogD("TaskGrabManyCamera.run():相机采图并处理.frameCount=%u", cimg->fnum);

            //标注一下当前工作相机的FPS
            for (size_t i = 0; i < cimg->vImage.size(); i++) {
                if (cimg->vImage[i].camera != nullptr) {
                    cimg->vImage[i].camera->FPS = fps;
                }
            }

            int ckey = -1; //让proc去自己想检测keydown就keydown
            proc->process(cimg, ckey);
            if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                cvKey.exchange(ckey);
            }

            cimg->procEndTime = clock(); //标记处理结束时间
            LogD("TaskGrabManyCamera.run():采图耗时%.0fms,处理耗时%.0fms", cimg->grabCostTime(), cimg->procCostTime());
            return true;
        }
        else {
            return false;
        }
    }

  private:
    // 计算fps的辅助.
    FPSCalc _fpsCalc;

    /**
     * 设置自己的cameras的fps
     *
     * @author daixian
     * @date 2020/11/28
     *
     * @param  fps The FPS.
     */
    void setCamerasFPS(float fps)
    {
        for (size_t factoryIndex = 0; factoryIndex < vCameraImageFactory.size(); factoryIndex++) {
            ICameraImageFactory* pCameraImageFactory = vCameraImageFactory[factoryIndex];
            pCameraImageFactory->device->FPS = fps;
            for (size_t i = 0; i < pCameraImageFactory->cameras.size(); i++) {
                pCameraImageFactory->cameras[i]->FPS = fps;
            }
        }
    }
};

} // namespace dxlib