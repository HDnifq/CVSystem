#pragma once
#include <atomic>
//#include "../eventbus/EventBus.h"

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
    /// <summary> 这个数据值对应共享内存的地址分布. </summary>
    int value[128];
};

///-------------------------------------------------------------------------------------------------
/// <summary> 系统的公共的状态事件,使用了EventBus库. </summary>
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

    /// <summary> 相机错误?. </summary>
    //std::atomic_int cameraError;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 获取事件总线. </summary>
    ///
    /// <remarks> Dx, 2019/10/25. </remarks>
    ///
    /// <returns> Null if it fails, else the bus. </returns>
    ///-------------------------------------------------------------------------------------------------
    //Dexode::EventBus* getBus();

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
    /// <summary> 检查是否有一条事件，如果有事件来了会走EventBus发出一条事件. </summary>
    ///
    /// <remarks> Dx, 2019/1/13. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void checkMemEvent();

#pragma endregion

  private:
    /// <summary> 单例. </summary>
    static Event* m_pInstance;

    /// <summary> 隐藏成员. </summary>
    class Impl;
    Impl* _impl = nullptr;
};

} // namespace dxlib
