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

    // 是否运行
    std::atomic_bool isRun{true};

    // 图片队列
    CameraImageQueue* imageQueue = nullptr;

    // 图片工厂
    ICameraImageFactory* pCameraImageFactory = nullptr;

    // 是否执行采图(如果不采图了那么会降低cpu开销).
    std::atomic_bool isGrab{true};

    // 是否执行处理(只有主相机才执行处理)
    bool isDoProc = false;

    // proc对象.
    pFrameProc proc = nullptr;

    // 处理的fps.
    float fps = 0;

    // 帧处理计数.
    uint frameCount = 0;

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
            LogE("TaskGrabOneCamera.runTask():imageQueue == nullptr");
            return;
        }
        if (pCameraImageFactory == nullptr) {
            LogE("TaskGrabOneCamera.runTask():pCameraImageFactory == nullptr");
            return;
        }

        if (isDoProc && proc != nullptr) {
            proc->onEnable();
        }

        while (isRun.load()) {
            //TODO:这里要加异常包围

            //执行一次采图
            if (isGrab.load()) {
                pCameraImageFactory->device->applyCapProp();

                LogD("TaskGrabOneCamera.runTask():CameraImageFactory执行采图!");
                std::vector<CameraImage> images = pCameraImageFactory->Create();
                for (size_t i = 0; i < images.size(); i++) {
                    //如果采图成功才放进队列好了,当相机有切换相机属性的时候是会产生硬件的采图失败的.
                    if (images[i].isSuccess) {
                        imageQueue->PushImage(images[i]);
                    }
                }
            }

            // 如果这个采图完了它就去检察是否能够执行proc处理
            if (isDoProc && proc != nullptr) {
                if (isGrab.load()) {
                    //如果是采图过程中
                    pCameraImageGroup cimg = imageQueue->GetImage();
                    if (cimg != nullptr) {
                        LogD("TaskGrabOneCamera.run():相机采图并处理.frameCount=%u", this->frameCount);
                        cimg->fnum = frameCount;
                        cimg->procStartTime = clock(); //标记处理开始时间

                        fps = _fpsCalc.update(++frameCount);

                        //标注一下当前工作相机的FPS
                        for (size_t i = 0; i < cimg->vImage.size(); i++) {
                            if (cimg->vImage[i].camera != nullptr) {
                                cimg->vImage[i].camera->FPS = fps;
                            }
                        }

                        LogD("TaskGrabOneCamera.run():执行proc!,从采图结束到现在等待了%f ms", cimg->waitProcTime());
                        int ckey = -1; //让proc去自己想检测keydown就keydown
                        proc->process(cimg, ckey);
                        if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                            cvKey.exchange(ckey);
                        }

                        cimg->procEndTime = clock(); //标记处理结束时间
                        LogD("TaskGrabOneCamera.run():采图耗时%.0fms,处理耗时%.0fms", cimg->grabCostTime(), cimg->procCostTime());
                    }
                }
                else {
                    LogD("MultiCamera.run():执行onLightSleep!");
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
            }
        }
    }

  private:
    // 计算fps的辅助.
    FPSCalc _fpsCalc;
};

} // namespace dxlib