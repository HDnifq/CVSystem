#pragma once
#include <atomic>
#include "../eventbus/EventBus.h"

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 内存事件定义. </summary>
    ///
    /// <remarks> Dx, 2019/1/13. </remarks>
    ///-------------------------------------------------------------------------------------------------
    struct MemEvent
    {
        /// <summary> 事件类型. </summary>
        int id;
        int value[128];
    };

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
        static Event* GetInst()
        {
            if (m_pInstance == NULL)
                m_pInstance = new Event();
            return m_pInstance;
        }

        /// <summary>
        /// 当前opencv界面来的待处理的key值，如果无按键那么为-1.
        /// (由各个模块自己设置它的值，然后监听部分轮询它的当前值，响应后再将它设置回-1).
        /// </summary>
        std::atomic_int cvKey = -1;

        /// <summary> 相机错误?. </summary>
        std::atomic_int cameraError;

        /// <summary> 事件总线. </summary>
        std::shared_ptr<Dexode::EventBus> bus = std::make_shared<Dexode::EventBus>();

        #pragma region 使用共享内存的方法来判断事件发出事件

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 设置共享内存中数据位置起始地址. </summary>
        ///
        /// <remarks> Dx, 2019/1/13. </remarks>
        ///
        /// <param name="data"> [in,out] If non-null, the data. </param>
        ///-------------------------------------------------------------------------------------------------
        void setMemEventData(void* data);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 检查是否有一条事件. </summary>
        ///
        /// <remarks> Dx, 2019/1/13. </remarks>
        ///-------------------------------------------------------------------------------------------------
        void checkMemEvent();

        #pragma endregion

    private:
        /// <summary> 单例. </summary>
        static Event* m_pInstance;

        /// <summary> 共享内存的消息模式. </summary>
        void* memEventData = nullptr;
    };



}
