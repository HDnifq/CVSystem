#include "pch.h"

#include "../CVSystem/CVSystem.h"

using namespace dxlib;
using namespace std;


class TestProc : public FrameProc
{
public:
    TestProc() {}
    ~TestProc() {}

    int process(pCameraImage camImage) override
    {
        cv::Mat image = camImage->vImage[0].image;
        EXPECT_FALSE(image.empty());//确实能采图

        int key = -1;
        key = cv::waitKey(1);//一定要加waitKey无法显示图片
        return key;
    }
    void onEnable() override
    {
    }

    void onDisable() override
    {
    }
};


class TestProcRelease : public FrameProc
{
public:
    TestProcRelease() {}
    ~TestProcRelease() {}

    int process(pCameraImage camImage) override
    {
        LogI("TestProcRelease.process():开始执行！");
        MultiCamera::GetInst()->close();
        LogI("TestProcRelease.process():执行完毕！");
        return -1;
    }
    void onEnable() override
    {
    }

    void onDisable() override
    {
    }
};


//测试MultiCamera的open是否正常
TEST(MultiCamera, open)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList->begin()->second;

    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));
    CameraManger::GetInst()->camMap[0]->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    MultiCamera::GetInst()->addProc(new TestProc());

    for (size_t i = 0; i < 2; i++) {
        MultiCamera::GetInst()->openCamera();//打开相机
        EXPECT_TRUE(CameraManger::GetInst()->camMap[0]->isOpened());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));//工作500秒
        EXPECT_TRUE(MultiCamera::GetInst()->frameCount > 0);
        MultiCamera::GetInst()->close();
    }


    MultiCamera::GetInst()->vProc.clear();
    CameraManger::GetInst()->clear();
}

//创建虚拟相机的时候不会打开相机
TEST(MultiCamera, AddVirtualCamera)
{
    CameraManger::GetInst()->add(pCamera(new Camera(0, L"VirtualCamera0")), true);
    CameraManger::GetInst()->add(pCamera(new Camera(1, L"VirtualCamera0")), true);

    bool result = MultiCamera::GetInst()->openCamera();//打开相机
    EXPECT_FALSE(result);
    EXPECT_FALSE(CameraManger::GetInst()->camMap[0]->isOpened());
    EXPECT_FALSE(CameraManger::GetInst()->camMap[1]->isOpened());
    MultiCamera::GetInst()->close();



    MultiCamera::GetInst()->vProc.clear();
    CameraManger::GetInst()->clear();
}


//测试MultiCamera的release是否正常，加入的proc是 TestProcRelease
TEST(MultiCamera, release)
{
    dlog_set_usual_thr(DLOG_Debug);

    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList->begin()->second;

    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));
    CameraManger::GetInst()->camMap[0]->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    MultiCamera::GetInst()->addProc(new TestProcRelease());

    for (size_t i = 0; i < 2; i++) {
        EXPECT_TRUE(MultiCamera::GetInst()->openCamera());//打开相机
        LogI("TEST(MultiCamera, release):当前MT工作状态 = %d", MultiCamera::GetInst()->isRun());
        while (MultiCamera::GetInst()->isRun()) {
            LogI("TEST(MultiCamera, release):主线程等待关闭！id=%d", std::this_thread::get_id());
            std::this_thread::sleep_for(std::chrono::milliseconds(500));//等待500
        }

        LogI("TEST(MultiCamera, release):主线程发现已经关闭！");
    }

    MultiCamera::GetInst()->vProc.clear();
    CameraManger::GetInst()->clear();
    BaseThread::GC();
    dlog_set_usual_thr(DLOG_INFO);
}