#pragma once

#include "BaseThread.h"
//
//namespace dxlib {
//
//    ///-------------------------------------------------------------------------------------------------
//    /// <summary> 工作线程. </summary>
//    ///
//    /// <remarks> Dx, 2019/1/15. </remarks>
//    ///-------------------------------------------------------------------------------------------------
//    class WorkThread
//    {
//    public:
//
//        WorkThread()
//        {
//        }
//
//        ~WorkThread()
//        {
//            if (isRun.load())
//                Stop();
//        }
//
//        /// <summary> 是否工作/停止. </summary>
//        std::atomic_bool isRun = false;
//
//        /// <summary> 启动线程. </summary>
//        bool Start();
//
//        /// <summary> 停止线程. </summary>
//        bool Stop();
//
//    private:
//
//    };
//
//}