#include "pch.h"

#include "../CVSystem/CVSystem.h"
#include "../CVSystem/Common/ThreadPool.h"

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

//TEST(ThreadPool, io_context_usetest)
//{
//
//    boost::asio::io_context io_c;
//    boost::asio::thread_pool pool{4}; // 2 threads
//
//    int count1 = 500;
//    int count2 = 200;
//    for (size_t i = 0; i < 100; i++) {
//        std::atomic_int count{0};
//
//        //创建一堆任务
//        io_c.stop();
//        //这里不安全操
//        io_c.restart();
//        for (size_t i = 0; i < count1; i++) {
//            boost::asio::post(io_c, [&] { count++; });
//        }
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        io_c.run(); //自己也亲自执行并且直到任务全部执行完毕
//        ASSERT_TRUE(count == count1) << count;
//
//        //重新创建一堆任务
//        io_c.stop();
//        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        io_c.restart();
//        for (size_t i = 0; i < count2; i++) {
//            boost::asio::post(io_c, [&] { count++; });
//        }
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        boost::asio::post(pool, boost::bind(&boost::asio::io_service::run, &io_c));
//        io_c.run();
//        EXPECT_TRUE(count == count1 + count2) << count;
//    }
//}

void funTest(std::atomic<int> *count)
{
    for (size_t i = 0; i < 1; i++) {
        (*count)++;
    }
}
void fun(int x, int y, int z)
{
    std::cout << x << "  " << y << "  " << z << std::endl;
}

void fun_2(int &a, int &b)
{
    a++;
    b++;
    std::cout << a << "  " << b << std::endl;
}

TEST(ThreadPool, CommonThreadPool)
{
    using namespace dxlib;
    std::atomic<int> count{0};
    ThreadPool *pool = ThreadPool::GetInst();

    for (size_t i = 0; i < 2000; i++) {
        auto msg = ThreadPool::GetInst()->creatMsg();
        msg->doWork = std::bind([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            count++;
            //printf("一次%d", count.load());
        });
        ThreadPool::GetInst()->post(msg);
    }
    ThreadPool::GetInst()->waitDone();

    EXPECT_TRUE(count.load() == 2000) << "count=" << count.load();

    for (size_t i = 0; i < 2000; i++) {
        auto msg = ThreadPool::GetInst()->creatMsg();
        msg->doWork = std::bind([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            count++;
            //printf("一次%d", count.load());
        });
        ThreadPool::GetInst()->post(msg);
    }

    ThreadPool::GetInst()->dispose();

    EXPECT_TRUE(count.load() == 4000) << "count=" << count.load();
}

TEST(ThreadPool, bindTest)
{
    using namespace dxlib;
    std::atomic<int> count{0};
    boost::asio::thread_pool pool{4}; // 2 threads

    std::thread::id mainId = std::this_thread::get_id();

    for (size_t i = 0; i < 10000; i++) {
        boost::asio::post(pool, std::bind([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            EXPECT_TRUE(mainId!= std::this_thread::get_id());
            if (mainId == std::this_thread::get_id())
            {
                return;
            }
            count++; }));
    }

    //pool.join(); //join之后这个线程池已经失效了.
    //pool.stop(); //如果直接stop那么下面的count会等于0;
    //EXPECT_TRUE(count.load() == 10000) << "count=" << count.load();

    //boost::asio::post(pool, std::bind(funTest, &count));
    pool.join();
    EXPECT_TRUE(count.load() == 10000) << "count=" << count.load();
}