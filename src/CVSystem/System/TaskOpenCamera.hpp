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

namespace dxlib {

/**
 * 使用一个线程去开相机，曾经考虑linux下是不是需要多线程去开相机才能提高采图性能，
 * 后来发现貌似只能多进程才能提高采图性能，所以目前不需要使用这个。
 *
 * @author daixian
 * @date 2020/11/27
 */
class TaskOpenCamera : public Poco::Runnable
{
  public:
    TaskOpenCamera(const pCameraDevice& device) : device(device)
    {}

    virtual ~TaskOpenCamera()
    {}

    pCameraDevice device;

    bool isSuccess = false;

    /**
     * 线程执行函数.
     *
     * @author daixian
     * @date 2020/11/27
     */
    virtual void run()
    {
        LogI("MultiCamera.open():尝试打开相机 %s ...", device->devName.c_str());
        clock_t startTime = clock();
        //打开相机
        if (device->open()) {
            double costTime = (clock() - startTime) / CLOCKS_PER_SEC * 1000.;
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            device->capture->read(img);
            if (!img.empty())
                LogI("MultiCamera.open():成功打开一个相机%s，耗时%.2f毫秒", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
            else
                LogE("MultiCamera.open():成功打开一个相机%s，耗时%.2f毫秒,但是尝试读取一帧图片失败!", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
            isSuccess = true;
        }
        else {
            isSuccess = false;
        }
    }

  private:
};
} // namespace dxlib