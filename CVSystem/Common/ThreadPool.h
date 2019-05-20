#pragma once

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

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

  private:
    boost::asio::thread_pool pool{4}; // 4 threads
};
} // namespace dxlib
