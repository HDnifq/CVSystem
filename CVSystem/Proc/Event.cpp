#include "Event.h"

namespace dxlib {

    Event::Event()
    {
    }

    Event::~Event()
    {
    }

    Event* Event::m_pInstance = NULL;

    Event* Event::GetInst()
    {
        if (m_pInstance == NULL)
            m_pInstance = new Event();
        return m_pInstance;
    }
}