#pragma once

#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "time.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 一个相机一次采图的结果. </summary>
    ///
    /// <remarks> Dx, 2018/11/27. </remarks>
    ///-------------------------------------------------------------------------------------------------
    struct ImageItem
    {
        /// <summary> 采图图片. </summary>
        cv::Mat image;

        /// <summary> 对应相机. </summary>
        Camera* camera;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 若干个相机一次的采图结果. </summary>
    ///
    /// <remarks> Dx, 2018/11/14. </remarks>
    ///-------------------------------------------------------------------------------------------------
    struct CameraImage
    {
        /// <summary> 相机帧序号. </summary>
        int fnum = 0;

        /// <summary> 这一帧里这一组相机的原始图像(但是index并不是camIndex). </summary>
        std::vector<ImageItem> vImage;

        /// <summary> 这一帧的采集时间戳. </summary>
        clock_t startTime = 0;

        /// <summary> 这一帧的采集消耗时间. </summary>
        float costTime = 0;

        ///-------------------------------------------------------------------------------------------------
        /// <summary>
        /// vImage并不是一个以camIndex为序号的数据结构，所以用这个函数来查找一个ImageItem.
        /// </summary>
        ///
        /// <remarks> Dx, 2018/12/19. </remarks>
        ///
        /// <param name="camIndex"> camIndex. </param>
        ///
        /// <returns> 如果没有找到那么就返回null. </returns>
        ///-------------------------------------------------------------------------------------------------
        ImageItem* getItem(int camIndex);

    };
    /// <summary>定义这个智能指针类型. </summary>
    typedef std::shared_ptr<CameraImage> pCameraImage;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 使用一个线程用来对若干个相机不停的读图. </summary>
    ///
    /// <remarks> Dx, 2018/11/7. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class CameraThread
    {
    public:
        CameraThread(pCamera cp);
        CameraThread(std::vector<pCamera> cps);
        CameraThread(std::map<int, pCamera> cps);

        ~CameraThread();

        /// <summary> 相机参数. </summary>
        std::vector<pCamera> vCameras;

        /// <summary> 是否停止运行（由外部操作）. </summary>
        std::atomic_bool isStop = false;

        /// <summary> 是否进行采图,如果为false则只占用相机不进行采图（由外部操作）. </summary>
        std::atomic_bool isGrab = true;

        /// <summary> 已采集的帧数. </summary>
        long fnumber = 0;

        /// <summary> 锁小线程们的锁. </summary>
        std::mutex* mtx_ct = nullptr;

        /// <summary> 从外面传进来，如果有定义那么等待它的通知. </summary>
        std::condition_variable* cv_ct = nullptr;

        /// <summary> 小线程们通知外面的mt里的综合分析线程的. </summary>
        std::condition_variable* cv_mt = nullptr;

        /// <summary> 采集的帧队列. </summary>
        moodycamel::ConcurrentQueue<pCameraImage> frameQueue;

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

        /// <summary> 打开一个相机. </summary>
        bool openCamera(pCamera& cameraParam);

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
}