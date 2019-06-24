#pragma once

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <thread>
#include <mutex>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 全局的线程池. </summary>
///
/// <remarks> Dx, 2019/5/20. </remarks>
///-------------------------------------------------------------------------------------------------
class ThreadPool
{
  private:
    /// <summary> The instance. </summary>
    static ThreadPool* m_pInstance;

  public:
    ThreadPool();
    ~ThreadPool();

    /// <summary> 单例. </summary>
    static ThreadPool* GetInst()
    {
        return m_pInstance;
    }

    void test()
    {
        boost::asio::post(pool, [] {});
    }

    void post()
    {
        //传入一个匿名函数
        boost::asio::post(pool, [&] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    }

  private:
    /// <summary> asio的线程池. </summary>
    boost::asio::thread_pool pool{4}; // 4 threads
};
} // namespace dxlib
