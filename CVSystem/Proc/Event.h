#pragma once
#include <atomic>

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 系统的公共的状态事件. </summary>
    ///
    /// <remarks> Dx, 2019/1/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Event
    {
    public:
        Event();
        ~Event();

        /// <summary> 单例. </summary>
        static Event* GetInst();

        /// <summary>
        /// 当前opencv界面来的待处理的key值，如果无按键那么为-1.
        /// (由各个模块自己设置它的值，然后监听部分轮询它的当前值，响应后再将它设置回-1).
        /// </summary>
        std::atomic_int cvKey = -1;


        /// <summary> 相机错误?. </summary>
        std::atomic_int cameraError;

    private:
        /// <summary> 单例. </summary>
        static Event* m_pInstance;
    };



}
