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

namespace dxlib {

ThreadPool* ThreadPool::m_pInstance = new ThreadPool();

//内部数据
class ThreadPool::Impl
{
  public:
    Impl()
    {
    }
    ~Impl()
    {
    }

    /// <summary> asio的线程池. </summary>
    boost::asio::thread_pool pool{4}; // 4 threads

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
    boost::asio::post(_impl->pool, [msg, this] {
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

void ThreadPool::waitDone()
{
    int count = 1;
    while (true) {
        if (_impl->addMsgCount.load() == _impl->doneCount.load()) {
            break;
        }
        count = count * 2;
        std::this_thread::sleep_for(std::chrono::milliseconds(10 * count < 1000 ? 10 * count : 1000));
    }
}

void ThreadPool::dispose()
{
    _impl->pool.join();
    _impl->pool.stop();
}

int ThreadPool::waitExecuteCount()
{
    return _impl->addMsgCount.load() - _impl->doneCount.load();
}

} // namespace dxlib