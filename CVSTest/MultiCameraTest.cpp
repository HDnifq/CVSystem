#include "pch.h"

#include "../CVSystem/CVSystem.h"

using namespace dxlib;
using namespace std;

class TestProc : public FrameProc
{
  public:
    TestProc() {}
    ~TestProc() {}

    int count = 0;

    void process(pCameraImage camImage, int& key) override
    {
        cv::Mat image = camImage->vImage[0].image;
        EXPECT_FALSE(image.empty()); //确实能采图
        count++;                     //采图的计数加1

        key = cv::waitKey(1); //一定要加waitKey无法显示图片
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

    void process(pCameraImage camImage, int& key) override
    {
        LogI("TestProcRelease.process():开始执行！");
        MultiCamera::GetInst()->close();
        LogI("TestProcRelease.process():执行完毕！");
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
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList.begin()->second;

    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));
    CameraManger::GetInst()->camMap[0]->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    MultiCamera::GetInst()->addProc(new TestProc());

    for (size_t i = 0; i < 2; i++) {          //测试两次
        MultiCamera::GetInst()->openCamera(); //打开相机
        EXPECT_TRUE(CameraManger::GetInst()->camMap[0]->isOpened());
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //工作500秒

        //测一下TestProc里是不是处理了图像帧
        TestProc* testfp = (TestProc*)MultiCamera::GetInst()->vProc[0].get();
        EXPECT_TRUE(testfp->count > 0);

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

    bool result = MultiCamera::GetInst()->openCamera(); //打开相机
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
    dlog_set_file_thr(dlog_level::debug);

    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList.begin()->second;

    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));
    CameraManger::GetInst()->camMap[0]->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    MultiCamera::GetInst()->addProc(new TestProcRelease());

    for (size_t i = 0; i < 2; i++) {
        EXPECT_TRUE(MultiCamera::GetInst()->openCamera()); //打开相机
        LogI("TEST(MultiCamera, release):当前MT工作状态 = %d", MultiCamera::GetInst()->isRun());
        while (MultiCamera::GetInst()->isRun()) {
            LogI("TEST(MultiCamera, release):主线程等待关闭！id=%d", std::this_thread::get_id());
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); //等待500
        }

        LogI("TEST(MultiCamera, release):主线程发现已经关闭！");
    }

    MultiCamera::GetInst()->vProc.clear();
    CameraManger::GetInst()->clear();
    BaseThread::GC();
    dlog_set_file_thr(dlog_level::info);
}