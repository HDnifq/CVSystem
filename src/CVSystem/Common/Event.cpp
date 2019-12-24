#include "Event.h"
#include "string.h"

namespace dxlib {

class Event::Impl
{
  public:
    Impl() {}
    ~Impl() {}

    /// <summary> 共享内存的消息模式. </summary>
    void* memEventData = nullptr;

    /// <summary> 事件总线. </summary>
    std::shared_ptr<Dexode::EventBus> bus = std::make_shared<Dexode::EventBus>();
};

///-------------------------------------------------------------------------------------------------
/// <summary> 一个内存消息在内存中的地址分布. </summary>
///
/// <remarks> Dx, 2019/1/13. </remarks>
///-------------------------------------------------------------------------------------------------
struct EventItem
{
    //标记有事件
    int flag;
    //事件的传递值
    int value[128];
};

Event::Event()
{
    _impl = new Impl();
}

Event::~Event()
{
    delete _impl;
}

Event* Event::m_pInstance = NULL;

Dexode::EventBus* Event::getBus()
{
    return _impl->bus.get();
}

void Event::setMemEventData(void* data)
{
    if (data != nullptr)
        _impl->memEventData = data;
}

void Event::checkMemEvent()
{
    if (_impl->memEventData == nullptr) {
        return;
    }

    EventItem* ptr = (EventItem*)_impl->memEventData;
    for (int i = 0; i < 64; i++) { //最多支持64条事件吧
        if (ptr[i].flag != 0) {
            ptr[i].flag = 0; //标记回0

            MemEvent* e = new MemEvent();
            e->id = i;

#if defined(_WIN32) || defined(_WIN64)
            memcpy_s(e->value, sizeof(e->value), ptr[i].value, sizeof(e->value));
#else
            memcpy(e->value, ptr[i].value, sizeof(e->value));
#endif
            memset(ptr, 0, sizeof(EventItem)); //清空这个消息的标记值
            _impl->bus->notify(*e);
        }
    }
}

} // namespace dxlib