#include "pch.h"

#include "../CVSystem/Proc/CameraThread.h"
#include "../CVSystem/Proc/DevicesHelper.h"

using namespace dxlib;
using namespace std;
//测试都只跑两个循环即可，主要是测试一下是否逻辑正确。稳定性暂时不考虑

//测试CameraThread的delete释放是否正常
TEST(CameraThread, new_delte)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList->begin()->second;

    for (size_t i = 0; i < 2; i++) {
        pCamera cp(new Camera(0, camName));
        CameraThread* ct = new CameraThread(cp);

        EXPECT_TRUE(ct->open() == true);
        EXPECT_TRUE(ct->isHasThread() == true);
        EXPECT_TRUE(cp->isOpened() == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); //工作100秒
        EXPECT_TRUE(ct->isThreadRunning.load() == true);
        EXPECT_TRUE(ct->isThreadWaitingStart.load() == false); //它不用等待通知

        delete ct;
    }
}

//测试一下多次打开/关闭有没有问题
TEST(CameraThread, OpenClose)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList->begin()->second;

    for (size_t i = 0; i < 2; i++) {
        pCamera cp(new Camera(0, camName));
        CameraThread* ct = new CameraThread(cp);

        EXPECT_TRUE(ct->open() == true);
        EXPECT_TRUE(cp->isOpened() == true);
        EXPECT_TRUE(ct->open() == false);    //重复打开，这里函数应该返回失败
        EXPECT_TRUE(cp->isOpened() == true); //但是cp里对相机状态的标记应该是打开的
        EXPECT_TRUE(ct->open() == false);
        EXPECT_TRUE(cp->isOpened() == true);
        EXPECT_TRUE(ct->open() == false);
        EXPECT_TRUE(cp->isOpened() == true);
        ct->close();
        EXPECT_TRUE(cp->isOpened() == false);
        ct->close();
        ct->close();
        ct->close();
        ct->close();
        delete ct;
    }
}

//测试一下多次打开/关闭有没有问题
TEST(CameraThread, OpenClose2)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() < 2) {
        return;
    }
    wstring camName0 = DevicesHelper::GetInst()->devList->begin()->second;
    wstring camName1 = (++DevicesHelper::GetInst()->devList->begin())->second;

    for (size_t i = 0; i < 2; i++) {
        pCamera cp(new Camera(0, camName0));
        CameraThread* ct = new CameraThread(cp);

        pCamera cp2(new Camera(1, camName1));
        CameraThread* ct2 = new CameraThread(cp2);

        EXPECT_TRUE(ct->open() == true);
        EXPECT_TRUE(cp->isOpened() == true);

        EXPECT_TRUE(ct2->open() == true);
        EXPECT_TRUE(cp2->isOpened() == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        delete ct;
        delete ct2;
    }
}
