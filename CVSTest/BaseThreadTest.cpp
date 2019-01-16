#include "pch.h"

#include "../CVSystem/CVSystem.h"

using namespace dxlib;

TEST(BaseThread, new_delete)
{
    std::shared_ptr<BaseThread> spbt;
    //这里无脑创建线程并且瞬间执行完毕
    for (size_t i = 0; i < 5; i++) {
        spbt = BaseThread::creat(nullptr, nullptr);
        BaseThread::GC();//直接无脑GC
        //还是等他执行完毕，因为没有加入工作函数，所以一下子就会执行完毕
        while (!spbt->isThreadFunReturn()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        spbt = nullptr;
    }

    for (size_t i = 0; i < 5; i++) {
        spbt = BaseThread::creat(nullptr, nullptr);
        spbt = nullptr;
        BaseThread::GC();//直接无脑GC
    }

    for (size_t i = 0; i < 5; i++) {
        spbt = BaseThread::creat(nullptr, nullptr);

        //还是等他执行完毕，因为没有加入工作函数，所以一下子就会执行完毕
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        while (!spbt->isThreadFunReturn()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    BaseThread::GC();
}

TEST(BaseThread, stop)
{
    for (size_t i = 0; i < 3; i++) {
        std::atomic_int workCount1 = 0;
        std::shared_ptr<BaseThread> spbt = BaseThread::creat(nullptr, [](std::shared_ptr<BaseThread>& tb) {
            std::atomic_int& wc = TO_USER_OBJ(std::atomic_int, tb);
            LogI("WorkOnce1():执行一次！");
            wc++;
            if (wc == 100) { //执行到100之后自己停止自己
                tb->stop();
            }
        }, nullptr, &workCount1);

        //一直等线程执行完毕
        while (!spbt->isThreadFunReturn()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        EXPECT_TRUE(workCount1.load() == 100);
    }
}