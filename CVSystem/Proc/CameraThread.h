#pragma once
#include <opencv2/opencv.hpp>

#include "Camera.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "../dlog/dlog.h"
#include "CameraImage.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 使用一个线程用来对若干个相机不停的读图. </summary>
///
/// <remarks> Dx, 2018/11/7. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraThread
{
  public:
    CameraThread(const pCamera& cp);
    CameraThread(const std::vector<pCamera>& cps);
    CameraThread(const std::map<int, pCamera>& cps);

    ~CameraThread();

    /// <summary> 相机参数. </summary>
    std::vector<pCamera> vCameras;

    /// <summary> 是否停止运行（由外部操作）. </summary>
    std::atomic_bool isStop = true;

    /// <summary> 是否进行采图,如果为false则只占用相机不进行采图（由外部操作）. </summary>
    std::atomic_bool isGrab = true;

    /// <summary> 是否存在采图线程. </summary>
    bool isHasThread()
    {
        if (_thread != nullptr) {
            return true;
        }
        else {
            return false;
        }
    }

    /// <summary> 采图线程是否正在工作（由内部标记）. </summary>
    std::atomic_bool isThreadRunning = false;

    /// <summary> 是否线程正在等待开始工作（由内部标记）. </summary>
    std::atomic_bool isThreadWaitingStart = false;

    /// <summary> 已采集的帧数. </summary>
    long fnumber = 0;

    /// <summary> 锁小线程们的锁. </summary>
    std::mutex* mtx_ct = nullptr;

    /// <summary> 从外面传进来，如果有定义那么等待它的通知. </summary>
    std::condition_variable* cv_ct = nullptr;

    /// <summary> 小线程们通知外面的mt里的综合分析线程的. </summary>
    std::condition_variable* cv_mt = nullptr;

#pragma region 队列

    /// <summary> 队列类型. </summary>
    struct QueueData;

    /// <summary> 队列数据. </summary>
    QueueData* queueData = nullptr;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 尝试从队列中取数据. </summary>
    ///
    /// <remarks> Dx, 2019/1/11. </remarks>
    ///
    /// <param name="cimg"> [in,out] The cimg. </param>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool try_dequeue(pCameraImage& cimg);

#pragma endregion

    /// <summary> 帧队列最大缓存帧数. </summary>
    uint queueMaxLen = 8;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开相机,同时新建了一个采图线程. </summary>
    ///
    /// <remarks> Dx, 2018/11/7. </remarks>
    ///
    /// <param name="isRunInError"> 当发生打开相机错误的时候是否仍然运行采图. </param>
    ///
    /// <returns> 正常执行返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool open(bool isRunInError = false);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭相机,实际上也关闭了线程. </summary>
    ///
    /// <remarks> Dx, 2018/11/12. </remarks>
    ///
    /// <returns> 正常执行返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool close();

  private:
    /// <summary> 采图线程. </summary>
    std::thread* _thread = nullptr;

    /// <summary> 线程函数. </summary>
    void dowork();

    /// <summary> 统计FPS需要的变量. </summary>
    clock_t _lastTime = 0;

    /// <summary> 统计FPS需要的变量. </summary>
    long _lastfnumber = 0;

    /// <summary> 统计一下当前的FPS. </summary>
    void updateFPS();

    /// <summary> 重置一下FPS状态. </summary>
    void reset();

    /// <summary> 限制队列的最大长度. </summary>
    void limitQueue(uint maxlen);
};
} // namespace dxlib