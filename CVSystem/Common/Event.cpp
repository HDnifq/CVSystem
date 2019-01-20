#include "Event.h"

namespace dxlib {


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
    }

    Event::~Event()
    {
    }

    Event* Event::m_pInstance = NULL;

    void Event::setMemEventData(void* data)
    {
        if(data != nullptr)
            this->memEventData = data;
    }

    void Event::checkMemEvent()
    {
        if (memEventData == nullptr) {
            return;
        }

        EventItem* ptr = (EventItem*)memEventData;
        for (int i = 0; i < 64; i++) { //最多支持64条事件吧
            if (ptr[i].flag != 0) {
                ptr[i].flag = 0;//标记回0

                MemEvent e;
                e.id = i;
                memcpy_s(e.value, sizeof(e.value), ptr[i].value, sizeof(e.value));

                memset(ptr, 0, sizeof(EventItem));//清空这个消息的标记值
                bus->notify(e);
            }
        }

    }

}