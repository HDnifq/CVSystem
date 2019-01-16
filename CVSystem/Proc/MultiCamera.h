#pragma once

#include "../Common/FPSCalc.hpp"
#include "CameraThread.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "../Common/BaseThread.h"

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画面帧处理接口对象. </summary>
    ///
    /// <remarks> Surface, 2018/11/16. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class FrameProc
    {
    public:
        virtual ~FrameProc() {};

        ///-------------------------------------------------------------------------------------------------
        /// <summary>
        /// 处理函数接口,返回值作为一个key值,如果没有按下按键,那么这里返回-1(和waitkey()的无按键返回值相同).
        /// </summary>
        ///-------------------------------------------------------------------------------------------------
        virtual int process(pCameraImage camImage) = 0;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 被使能的时候的响应. </summary>
        ///
        /// <remarks> Xian Dai, 2018/10/18. </remarks>
        ///-------------------------------------------------------------------------------------------------
        virtual void onEnable() = 0;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 不使能的时候的响应. </summary>
        ///
        /// <remarks> Xian Dai, 2018/9/28. </remarks>
        ///-------------------------------------------------------------------------------------------------
        virtual void onDisable() = 0;
    };
    typedef std::shared_ptr<FrameProc> pFrameProc;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 多摄像机，它使用一个线程进行所有图像的处理工作. </summary>
    ///
    /// <remarks> Surface, 2018/11/16. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class MultiCamera
    {
    public:

        MultiCamera(); // 1000 / 1000

        ~MultiCamera();

        static MultiCamera* GetInst()
        {
            if (m_pInstance == NULL)
                m_pInstance = new MultiCamera();
            return m_pInstance;
        }

        ///// <summary> 是否进入睡眠模式，在睡眠模式下不取图，不处理. </summary> //由于处理线程受采图线程供图处理，所以这里睡眠不在这里可以控制
        //bool isSleep = false;

        ///// <summary> 是否进入深度睡眠模式，在深度睡眠模式下关闭摄像机. </summary>
        //bool isDeepSleep = false;

        /// <summary> 帧处理计数. </summary>
        uint frameCount = 0;

        /// <summary> 处理的fps. </summary>
        float fps = 0;

        /// <summary> 多个相机处理器的处理对象,每一个相机开一个VideoProcessor. </summary>
        CameraThread* cameraThread = nullptr;

        /// <summary> (直接暴露出来使用)一次只使能一个处理. </summary>
        std::vector<pFrameProc> vProc;

        /// <summary> (直接暴露出来，只读取)当前激活的处理Index. </summary>
        uint activeProcIndex = 0;

        ///// <summary> 如果当前的FrameProc中有调用waitkey，那么可以传出一个按键值供外界的响应处理. </summary>
        //MultiCameraKeyEvent procKeyEvent = nullptr;

        ///// <summary> 当前待处理的key值，如果无按键那么为-1. (换成Event单例)</summary>
        //std::atomic_int key = -1;

        /// <summary> 是否显示窗口. </summary>
        bool isShowWin = false;

        /// <summary> 锁小线程们的锁. </summary>
        std::mutex mtx_ct;

        /// <summary> 给小线程们的通知. </summary>
        std::condition_variable cv_ct;

        /// <summary> 全局互斥锁. </summary>
        std::mutex mtx_mt;

        /// <summary> 其他人向自己的通知. </summary>
        std::condition_variable cv_mt;

        /// <summary> openCamera函数的设置. </summary>
        enum OpenCameraType
        {
            /// <summary> 如果打开相机成功就启动一个计算线程. </summary>
            StartCalcThread,

            /// <summary> 不启动计算线程，时候自己运行run()函数. </summary>
            NotStartCalcThread,

            /// <summary> 不管打开相机是否成功都强制启动一个计算线程. </summary>
            ForceStartCalcThread,
        };

        ///-------------------------------------------------------------------------------------------------
        /// <summary>
        /// 打开相机会启动线程函数,目前一般尝试创建一个线程来计算,
        /// 这里需要做一个预先的激活proc的设置（activeIndex赋值）,
        /// 以便第一帧运行就能正确的执行到想要的proc.
        /// 如果相机打开失败的话那么就不会创建分析线程。采图线程也不会创建了。.
        /// </summary>
        ///
        /// <remarks> Dx, 2018/2/1. </remarks>
        ///
        /// <param name="activeIndex"> (Optional)设置当前激活的Proc. </param>
        /// <param name="openType">    (Optional)为true则创建计算线程，为false则需要手动的运行run函数. </param>
        ///
        /// <returns> 打开成功返回ture. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool openCamera(uint activeIndex = 0, OpenCameraType openType = OpenCameraType::StartCalcThread);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 关闭所有相机. </summary>
        ///
        /// <remarks> Dx, 2018/3/1. </remarks>
        ///
        /// <param name="isDeleteProc"> (Optional)是否要释放所有proc. </param>
        ///-------------------------------------------------------------------------------------------------
        void close(bool isDeleteProc = false);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 添加一个Proc. </summary>
        ///
        /// <remarks> Dx, 2019/1/10. </remarks>
        ///
        /// <param name="proc"> The proc. </param>
        ///-------------------------------------------------------------------------------------------------
        void addProc(const pFrameProc& proc);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 添加一个Proc. </summary>
        ///
        /// <remarks> Dx, 2019/1/10. </remarks>
        ///
        /// <param name="proc"> [in,out] If non-null, the proc. </param>
        ///-------------------------------------------------------------------------------------------------
        void addProc(FrameProc* proc);

        ///-------------------------------------------------------------------------------------------------
        /// <summary>设置当前激活的Proc,这个函数会有效的标记activeProcIndex. </summary>
        ///
        /// <remarks> Dx, 2018/1/30. </remarks>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///-------------------------------------------------------------------------------------------------
        void setActiveProc(uint index);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 设置当前是否采图，主要是控制它的成员CameraThread. </summary>
        ///
        /// <remarks> Dx, 2018/11/20. </remarks>
        ///
        /// <param name="isGrab"> 如果是真就是采图. </param>
        ///-------------------------------------------------------------------------------------------------
        void setIsGrab(bool isGrab);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 是否当前正在正常工作. </summary>
        ///
        /// <remarks> Dx, 2019/1/16. </remarks>
        ///
        /// <returns> True if run, false if not. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool isRun()
        {
            return _isRun.load();
        }
    private:

        static MultiCamera* m_pInstance;

        /// <summary> 是否停止. </summary>
        std::atomic_bool _isRun = false;

        /// <summary> 是否正在打开. </summary>
        std::atomic_bool _isOpening = false;

        /// <summary> 是否正在停止. </summary>
        std::atomic_bool _isStopping = false;

        /// <summary> 综合分析线程. </summary>
        pBaseThread _thread = nullptr;

        /// <summary> 设置的下一个激活index. </summary>
        uint _nextActiveProcIndex = 0;

        /// <summary> 是否对当前proc重新执行一次disable和enable. </summary>
        bool _isResetActiveProc = false;

        void init(std::shared_ptr<BaseThread>& tb);

        void workonce(std::shared_ptr<BaseThread>& tb);

        void release(std::shared_ptr<BaseThread>& tb);


        //计算fps的辅助
        FPSCalc fpsCalc;
    };
}