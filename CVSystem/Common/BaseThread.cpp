#include "BaseThread.h"
#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"
#include <windows.h>

namespace dxlib {

#ifdef USE_BTGC

struct BaseThread::BTGC::Data
{
    /// <summary> 垃圾队列，持有最后一次引用. </summary>
    moodycamel::ConcurrentQueue<std::shared_ptr<BaseThread>> gcQueue;
};

BaseThread::BTGC* BaseThread::btgc = new BaseThread::BTGC();

BaseThread::BTGC::BTGC()
{
    data = new BaseThread::BTGC::Data();
}

BaseThread::BTGC::~BTGC()
{
    delete data;
}

void BaseThread::BTGC::add(std::shared_ptr<BaseThread>& bt)
{
    bool success = data->gcQueue.enqueue(bt);
    //LogD("BTGC.add():添加线程到垃圾队列id! success=%d", success);
}

void BaseThread::BTGC::clear()
{
    std::shared_ptr<BaseThread> bt;
    while (data->gcQueue.try_dequeue(bt)) {
        //LogD("BTGC.clear():垃圾队列里释放一个!");
    }
}

void BaseThread::setPriority(unsigned long processPriority, int threadPriority)
{
    DWORD dwError;
    if (!SetPriorityClass(GetCurrentProcess(), processPriority)) {
        dwError = GetLastError();
        LogE("BaseThread.setPriority():进程优先级设置失败%d", dwError);
    }

    if (_thread == nullptr) {
        LogE("BaseThread.setPriority():_thread=NULL,未设置线程优先级!");
    }
    if (!SetThreadPriority(_thread->native_handle(), threadPriority)) {
        dwError = GetLastError();
        LogE("BaseThread.setPriority():进程优先级设置失败%d", dwError);
    }
}

///-------------------------------------------------------------------------------------------------
/// <summary> 设置线程成高优先级. </summary>
///
/// <remarks> Surface, 2019/3/6. </remarks>
///-------------------------------------------------------------------------------------------------
void BaseThread::setPriorityHigh()
{
    setPriority(REALTIME_PRIORITY_CLASS, THREAD_PRIORITY_HIGHEST);
}

#endif // USE_BTGC

} // namespace dxlib