#include "pch.h"

#include "../CVSystem/CVSystem.h"

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

class TP : boost::asio::thread_pool
{
  public:
    int runCount()
    {
    }

  private:
};

TEST(ThreadPool, usetest)
{
    std::atomic_int count{0};

    boost::asio::thread_pool pool{4}; // 4 threads
    int count1 = 500;

    for (size_t i = 0; i < count1; i++) {
        boost::asio::post(pool, [&] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); count++; });
    }
    //pool.join();

    ASSERT_TRUE(count == count1) << count;

    for (size_t i = 0; i < 100; i++) {
        boost::asio::post(pool, [&] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); count++; });
    }
    pool.join();

    EXPECT_TRUE(count == 200) << count;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    EXPECT_TRUE(count == 200) << count;
}

TEST(ThreadPool, io_context_usetest)
{

    boost::asio::io_context io_c;
    boost::asio::thread_pool pool{4}; // 2 threads

    int count1 = 500;
    int count2 = 200;
    for (size_t i = 0; i < 100; i++) {
        std::atomic_int count{0};

        //创建一堆任务
        io_c.stop();
        //这里不安全操
        io_c.restart();
        for (size_t i = 0; i < count1; i++) {
            boost::asio::post(io_c, [&] { count++; });
        }
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        io_c.run(); //自己也亲自执行并且直到任务全部执行完毕
        ASSERT_TRUE(count == count1) << count;

        //重新创建一堆任务
        io_c.stop();
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        io_c.restart();
        for (size_t i = 0; i < count2; i++) {
            boost::asio::post(io_c, [&] { count++; });
        }
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
        io_c.run();
        EXPECT_TRUE(count == count1 + count2) << count;
    }
}