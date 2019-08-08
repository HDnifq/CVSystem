#include "ThreadPool.h"

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "dlog/dlog.h"
#ifdef DLOG_EXPORT
#    include "dlog/dlog.h"
#else
#    define LogD printf
#    define LogI printf
#    define LogW printf
#    define LogE printf
#endif // DLOG_EXPORT

#if defined(_WIN32) || defined(_WIN64)
#    include <windows.h>
#endif

namespace dxlib {

ThreadPool* ThreadPool::m_pInstance = new ThreadPool();

//内部数据
class ThreadPool::Impl
{
  public:
    Impl()
    {
        pool = new boost::asio::thread_pool{4};
    }
    ~Impl()
    {
        delete pool;
    }

    /// <summary> asio的线程池. </summary>
    boost::asio::thread_pool* pool{nullptr}; // 4 threads

    /// <summary> 输入执行的计数. </summary>
    std::_Atomic_ullong addMsgCount{0};

    /// <summary> 已完成的消息的计数. </summary>
    std::_Atomic_ullong doneCount{0};

    /// <summary> 消息的id的计数. </summary>
    std::_Atomic_ullong idCount{0};

    std::mutex mut;

  private:
};

ThreadPool::ThreadPool()
{
    _impl = new Impl();
}

ThreadPool::~ThreadPool()
{
    delete _impl;
}

pThreadMsg ThreadPool::creatMsg()
{
    std::shared_ptr<ThreadMsg> msg = std::make_shared<ThreadMsg>();
    //随便的分配一个ID,实际上也没有使用这个id
    msg->id = _impl->idCount.load();
    _impl->idCount++;
    return msg;
}

void ThreadPool::post(std::shared_ptr<ThreadMsg>& msg)
{
    //如果当前不是工作状态那么就直接返回算了
    if (!isRun.load()) {
        return;
    }

    //添加计数递增
    _impl->addMsgCount += 1;

    //这里的传递必须要使用一个拷贝,否则会被释放有问题
    boost::asio::post(*(_impl->pool), [msg, this] {
        if (isRun.load()) {
            //执行记录的三个函数指针
            if (msg->init != nullptr) {
                try {
                    msg->init();
                }
                catch (const std::exception& e) {
                    LogE("ThreadPool.post():消息%s执行init委托异常:%s", msg->name.c_str(), e.what());
                }
            }

            if (msg->doWork != nullptr) {
                try {
                    //在这里用户的代码判断是否需要跳出了然后直接结束
                    msg->doWork(this->isRun);
                }
                catch (const std::exception& e) {
                    LogE("ThreadPool.post():消息%s执行doWork委托异常:%s", msg->name.c_str(), e.what());
                }
            }

            if (msg->release != nullptr) {
                try {
                    msg->release();
                }
                catch (const std::exception& e) {
                    LogE("ThreadPool.post():消息%s执行release委托异常:%s", msg->name.c_str(), e.what());
                }
            }
        }

        _impl->doneCount += 1; //标记已经完成了一条消息
    });
}

void ThreadPool::waitDone(int timeSec)
{
    int count = 1;
    clock_t t0 = clock();
    while (true) {
        if (_impl->addMsgCount.load() == _impl->doneCount.load()) {
            break;
        }
        double costTime = (double)(clock() - t0) / CLOCKS_PER_SEC; //计算一个等待了的时间
        if (timeSec > 0 && costTime > timeSec) {
            break;
        }
        count = count * 2;
        std::this_thread::sleep_for(std::chrono::milliseconds(10 * count < 100 ? 10 * count : 100));
    }
}

void ThreadPool::dispose()
{
    _impl->pool->join();
    _impl->pool->stop();
}

void ThreadPool::reset(size_t num_threads)
{
    _impl->pool->stop();
    delete _impl->pool;
    _impl->pool = new boost::asio::thread_pool{num_threads};
}

int ThreadPool::waitExecuteCount()
{
    return _impl->addMsgCount.load() - _impl->doneCount.load();
}

#if defined(_WIN32) || defined(_WIN64)

void setPriority(unsigned long processPriority, int threadPriority)
{
    DWORD dwError;
    if (!SetPriorityClass(GetCurrentProcess(), processPriority)) {
        dwError = GetLastError();
        LogE("ThreadPool.setPriority():进程优先级设置失败%d", dwError);
    }
    //_thread->native_handle()
    if (!SetThreadPriority(GetCurrentProcess(), threadPriority)) {
        dwError = GetLastError();
        LogE("ThreadPool.setPriority():进程优先级设置失败%d", dwError);
    }
}

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 当前的线程设置高优先级.
/// </summary>
///
/// <remarks> Dx, 2019/8/7. </remarks>
///-------------------------------------------------------------------------------------------------
void ThreadPool::CurThreadSetPriorityHigh()
{
    setPriority(REALTIME_PRIORITY_CLASS, THREAD_PRIORITY_HIGHEST);
}

#endif

} // namespace dxlib