#include "BaseThread.h"
#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"

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

#endif // USE_BTGC

}