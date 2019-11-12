#include "pch.h"

#include "../CVSystem/CVSystem.h"

using namespace dxlib;

int workCount1 = 0;

//工作一次就是sleep 100毫秒
void WorkOnce1()
{
    LogI("WorkOnce1():执行一次！");
    workCount1++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

//WorkThread* wt;

//void WorkOnce2()
//{
//    LogI("WorkOnce2():执行一次！");
//    std::this_thread::sleep_for(std::chrono::milliseconds(1));
//
//    //关闭线程，然后又启动了个别的
//    wt->Stop();
//    wt->workOnce = WorkOnce1;
//    wt->Start();
//}
//
//TEST(WorkThread, new_delete)
//{
//    workCount1 = 0;
//
//    for (size_t i = 0; i < 100; i++)
//    {
//        wt = new WorkThread();
//        wt->workOnce = WorkOnce1;
//        wt->Start();
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        wt->Stop();
//        delete wt;
//    }
//    EXPECT_TRUE(workCount1 > 0);
//}
//
//TEST(WorkThread, new_delete_NoStop)
//{
//    workCount1 = 0;
//    for (size_t i = 0; i < 100; i++)
//    {
//        wt = new WorkThread();
//        wt->workOnce = WorkOnce1;
//        wt->Start();
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        //不做stop,直接delete
//        delete wt;
//    }
//    EXPECT_TRUE(workCount1 > 0);
//}
//
//TEST(WorkThread, new_delete_StopSelf)
//{
//    workCount1 = 0;
//    for (size_t i = 0; i < 1; i++)
//    {
//        wt = new WorkThread();
//        wt->workOnce = WorkOnce2;
//        wt->Start();
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//        //不做stop,直接delete
//        delete wt;
//    }
//    EXPECT_TRUE(workCount1 > 0);
//}