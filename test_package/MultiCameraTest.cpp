﻿#include "pch.h"

#include "CVSystem/CVSystem.h"
#include <thread>

using namespace dxlib;
using namespace std;

class TestProc : public FrameProc
{
  public:
    TestProc() {}
    ~TestProc() {}

    int count = 0;

    void process(pCameraImageGroup camImage, int& key) override
    {
        cv::Mat image = camImage->vImage[0].image;
        ASSERT_FALSE(image.empty()); //确实能采图
        count++;                     //采图的计数加1

        try {
            key = cv::waitKey(1); //一定要加waitKey无法显示图片
        }
        catch (const std::exception& e) {
        }
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

    void process(pCameraImageGroup camImage, int& key) override
    {
        LogI("TestProcRelease.process():开始执行！");
        //MultiCamera::GetInst()->close();
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
TEST(MultiCamera, openMono)
{
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    string camName = DevicesHelper::GetInst()->devList.begin()->second;

    pCameraDevice device = pCameraDevice(new CameraDevice(camName, cv::Size(1280, 720), 16));
    pCamera camera = pCamera(new Camera(camName, cv::Size(1280, 720)));
    CameraManger::GetInst()->add(device);
    CameraManger::GetInst()->add(camera);
    MonoCameraImageFactory* factory = new MonoCameraImageFactory(device, camera);
    CameraManger::GetInst()->add(pCameraImageFactory(factory));

    MultiCamera::GetInst()->addProc(new TestProc());

    for (size_t i = 0; i < 2; i++) {          //测试两次
        MultiCamera::GetInst()->openCamera(); //打开相机

        CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
        CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

        //打开相机成功了
        //ASSERT_TRUE(MultiCamera::GetInst()->isCameraOpened());
        //ASSERT_TRUE(CameraManger::GetInst()->vDevice[0]->isOpened());

        //输出一下
        CameraManger::GetInst()->vDevice[0]->outputProp();

        //启动计算线程
        MultiCamera::GetInst()->start();
        ASSERT_TRUE(MultiCamera::GetInst()->isRunning());
        std::this_thread::sleep_for(std::chrono::milliseconds(4000)); //工作1000毫秒

        //测一下TestProc里是不是处理了图像帧
        TestProc* testfp = (TestProc*)MultiCamera::GetInst()->getProc(0);
        EXPECT_TRUE(testfp->count > 0);
        LogI("MultiCamera.openMono():fps=%f", MultiCamera::GetInst()->fps());

        ASSERT_TRUE(MultiCamera::GetInst()->frameCount() > 0);
        MultiCamera::GetInst()->stop();
        ASSERT_TRUE(!MultiCamera::GetInst()->isRunning());
        MultiCamera::GetInst()->closeCamera();
        ASSERT_TRUE(!MultiCamera::GetInst()->isCameraOpened());
    }

    MultiCamera::GetInst()->clearProc();
    CameraManger::GetInst()->clear();
}

TEST(MultiCamera, openStereo)
{
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    string camName = DevicesHelper::GetInst()->devList.begin()->second;

    //pCameraDevice device = pCameraDevice(new CameraDevice(camName, cv::Size(1280, 720), 0));
    //pCamera cameraL = pCamera(new Camera(camName + "-L", cv::Size(640, 720)));
    //pCamera cameraR = pCamera(new Camera(camName + "-R", cv::Size(640, 720)));
    //CameraManger::GetInst()->add(device);
    //CameraManger::GetInst()->add(cameraL);
    //CameraManger::GetInst()->add(cameraR);

    //StereoCameraImageFactory* factory = new StereoCameraImageFactory(device, {cameraL, cameraR});
    //CameraManger::GetInst()->add(pCameraImageFactory(factory));

    //使用工厂方法
    pCameraDevice device;
    pStereoCamera stereo;
    CameraManger::GetInst()->CreateStereoCamera(camName, 1280, 720, device, stereo);

    MultiCamera::GetInst()->addProc(new TestProc());

    CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
    CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

    for (size_t i = 0; i < 2; i++) {          //测试两次
        MultiCamera::GetInst()->openCamera(); //打开相机

        CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
        CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

        //打开相机成功了
        ASSERT_TRUE(MultiCamera::GetInst()->isCameraOpened());
        ASSERT_TRUE(CameraManger::GetInst()->vDevice[0]->isOpened());

        //输出一下
        CameraManger::GetInst()->vDevice[0]->outputProp();

        //启动计算线程
        MultiCamera::GetInst()->start();
        ASSERT_TRUE(MultiCamera::GetInst()->isRunning());
        std::this_thread::sleep_for(std::chrono::milliseconds(4000)); //工作1000毫秒

        //测一下TestProc里是不是处理了图像帧
        TestProc* testfp = (TestProc*)MultiCamera::GetInst()->getProc(0);
        EXPECT_TRUE(testfp->count > 0);
        LogI("MultiCamera.openStereo():fps=%f", MultiCamera::GetInst()->fps());

        ASSERT_TRUE(MultiCamera::GetInst()->frameCount() > 0);
        MultiCamera::GetInst()->stop();
        ASSERT_TRUE(!MultiCamera::GetInst()->isRunning());
        MultiCamera::GetInst()->closeCamera();
        ASSERT_TRUE(!MultiCamera::GetInst()->isCameraOpened());
    }

    MultiCamera::GetInst()->clearProc();
    CameraManger::GetInst()->clear();
}

TEST(MultiCamera, openStereoMT)
{
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    string camName = DevicesHelper::GetInst()->devList.begin()->second;

    //pCameraDevice device = pCameraDevice(new CameraDevice(camName, cv::Size(1280, 720), 0));
    //pCamera cameraL = pCamera(new Camera(camName + "-L", cv::Size(640, 720)));
    //pCamera cameraR = pCamera(new Camera(camName + "-R", cv::Size(640, 720)));
    //CameraManger::GetInst()->add(device);
    //CameraManger::GetInst()->add(cameraL);
    //CameraManger::GetInst()->add(cameraR);

    //StereoCameraImageFactory* factory = new StereoCameraImageFactory(device, {cameraL, cameraR});
    //CameraManger::GetInst()->add(pCameraImageFactory(factory));

    //使用工厂方法
    pCameraDevice device;
    pStereoCamera stereo;
    CameraManger::GetInst()->CreateStereoCamera(camName, 1280, 720, device, stereo);

    MultiCamera::GetInst()->addProc(new TestProc());

    CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
    CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

    for (size_t i = 0; i < 2; i++) {          //测试两次
        MultiCamera::GetInst()->openCamera(); //打开相机

        CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
        CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

        //打开相机成功了
        ASSERT_TRUE(MultiCamera::GetInst()->isCameraOpened());
        ASSERT_TRUE(CameraManger::GetInst()->vDevice[0]->isOpened());

        //输出一下
        CameraManger::GetInst()->vDevice[0]->outputProp();

        //启动计算线程
        MultiCamera::GetInst()->startMT();
        ASSERT_TRUE(MultiCamera::GetInst()->isRunning());

        MultiCamera::GetInst()->setFrameFlag("测试标记", 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //工作1000毫秒

        //测一下TestProc里是不是处理了图像帧
        TestProc* testfp = (TestProc*)MultiCamera::GetInst()->getProc(0);
        EXPECT_TRUE(testfp->count > 0);
        LogI("MultiCamera.openStereo():fps=%f", MultiCamera::GetInst()->fps());

        ASSERT_TRUE(MultiCamera::GetInst()->frameCount() > 0);
        MultiCamera::GetInst()->stop();
        ASSERT_TRUE(!MultiCamera::GetInst()->isRunning());
        MultiCamera::GetInst()->closeCamera();
        ASSERT_TRUE(!MultiCamera::GetInst()->isCameraOpened());
    }

    MultiCamera::GetInst()->clearProc();
    CameraManger::GetInst()->clear();
}

TEST(MultiCamera, invokeStopAndClose)
{
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    string camName = DevicesHelper::GetInst()->devList.begin()->second;

    //pCameraDevice device = pCameraDevice(new CameraDevice(camName, cv::Size(1280, 720), 0));
    //pCamera cameraL = pCamera(new Camera(camName + "-L", cv::Size(640, 720)));
    //pCamera cameraR = pCamera(new Camera(camName + "-R", cv::Size(640, 720)));
    //CameraManger::GetInst()->add(device);
    //CameraManger::GetInst()->add(cameraL);
    //CameraManger::GetInst()->add(cameraR);

    //StereoCameraImageFactory* factory = new StereoCameraImageFactory(device, {cameraL, cameraR});
    //CameraManger::GetInst()->add(pCameraImageFactory(factory));

    //使用工厂方法
    pCameraDevice device;
    pStereoCamera stereo;
    CameraManger::GetInst()->CreateStereoCamera(camName, 1280, 720, device, stereo);

    MultiCamera::GetInst()->addProc(new TestProc());

    CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
    CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

    for (size_t i = 0; i < 2; i++) {          //测试两次
        MultiCamera::GetInst()->openCamera(); //打开相机

        CameraManger::GetInst()->vDevice[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
        CameraManger::GetInst()->vDevice[0]->setFourcc("MJPG");

        //打开相机成功了
        ASSERT_TRUE(MultiCamera::GetInst()->isCameraOpened());
        ASSERT_TRUE(CameraManger::GetInst()->vDevice[0]->isOpened());

        //输出一下
        CameraManger::GetInst()->vDevice[0]->outputProp();

        //启动计算线程
        MultiCamera::GetInst()->start();
        ASSERT_TRUE(MultiCamera::GetInst()->isRunning());

        //测一下TestProc里是不是处理了图像帧
        TestProc* testfp = (TestProc*)MultiCamera::GetInst()->getProc(0);
        for (size_t i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); //工作1000毫秒
            if (testfp->count > 0) {
                break;
            }
        }

        EXPECT_TRUE(testfp->count > 0);
        LogI("MultiCamera.invokeStopAndClose():fps=%f", MultiCamera::GetInst()->fps());
        ASSERT_TRUE(MultiCamera::GetInst()->frameCount() > 0);

        MultiCamera::GetInst()->invokeStopAndClose(false); //注意这里不清空proc
        for (size_t i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //工作1000毫秒
            if (!MultiCamera::GetInst()->isCameraOpened()) {
                break;
            }
        }

        ASSERT_TRUE(!MultiCamera::GetInst()->isRunning());
        ASSERT_TRUE(!MultiCamera::GetInst()->isCameraOpened());
        MultiCamera::GetInst()->stop();
        MultiCamera::GetInst()->closeCamera();
    }

    MultiCamera::GetInst()->clearProc();
    CameraManger::GetInst()->clear();
}

//创建虚拟相机的时候不会打开相机
//TEST(MultiCamera, AddVirtualCamera)
//{
//    MultiCamera::GetInst()->closeCamera();
//    MultiCamera::GetInst()->clearProc();
//
//    CameraManger::GetInst()->clear();
//    ASSERT_TRUE(CameraManger::GetInst()->addVirtual(pCamera(new Camera("VirtualCamera0"))) != nullptr);
//    ASSERT_TRUE(CameraManger::GetInst()->addVirtual(pCamera(new Camera("VirtualCamera1"))) != nullptr);
//
//    bool result = MultiCamera::GetInst()->openCamera(); //打开相机
//    //EXPECT_FALSE(result);
//    EXPECT_FALSE(CameraManger::GetInst()->camMap[0]->isOpened()); //虚拟相机的打开应该是为false的
//    EXPECT_FALSE(CameraManger::GetInst()->camMap[1]->isOpened());
//    MultiCamera::GetInst()->closeCamera();
//
//    MultiCamera::GetInst()->clearProc();
//    CameraManger::GetInst()->clear();
//}

//测试实际上没有相机设备的时候
//TEST(MultiCamera, NoCamera)
//{
//    LogI("测试没有符合的相机的时候");
//    CameraManger::GetInst()->clear();
//    MultiCamera::GetInst()->closeCamera();
//
//    CameraManger::GetInst()->add(pCamera(new Camera("cam1")));
//    CameraManger::GetInst()->add(pCamera(new Camera("cam2")));
//
//    MultiCamera::GetInst()->clearProc();
//    MultiCamera::GetInst()->addProc(pFrameProc(new CamImageProc()));
//
//    MultiCamera::GetInst()->openCamera();
//    MultiCamera::GetInst()->start();
//    while (MultiCamera::GetInst()->frameCount() < 100) {
//        this_thread::sleep_for(chrono::milliseconds(10));
//    }
//    MultiCamera::GetInst()->closeCamera();
//    MultiCamera::GetInst()->stop();
//    CameraManger::GetInst()->clear();
//}

//测试MultiCamera的release是否正常，加入的proc是 TestProcRelease
//TEST(MultiCamera, release)
//{
//    dlog_set_file_thr(dlog_level::debug);
//
//    //得到第一个相机名
//    DevicesHelper::GetInst()->listDevices();
//    if (DevicesHelper::GetInst()->devList.size() == 0) {
//        return;
//    }
//    wstring camName = DevicesHelper::GetInst()->devList.begin()->second;
//
//    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));
//    CameraManger::GetInst()->camMap[0]->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);
//
//    MultiCamera::GetInst()->addProc(new TestProcRelease());
//
//    for (size_t i = 0; i < 2; i++) {
//        EXPECT_TRUE(MultiCamera::GetInst()->openCamera()); //打开相机
//        LogI("TEST(MultiCamera, release):当前MT工作状态 = %d", MultiCamera::GetInst()->isRun());
//        while (MultiCamera::GetInst()->isRun()) {
//            LogI("TEST(MultiCamera, release):主线程等待关闭！id=%d", std::this_thread::get_id());
//            std::this_thread::sleep_for(std::chrono::milliseconds(800)); //等待800
//        }
//
//        LogI("TEST(MultiCamera, release):主线程发现已经关闭！");
//    }
//
//    MultiCamera::GetInst()->vProc.clear();
//    CameraManger::GetInst()->clear();
//    BaseThread::GC();
//    dlog_set_file_thr(dlog_level::info);
//}
//
//TEST(MultiCamera, camIndex)
//{
//    CameraManger::GetInst()->add(pCamera(new Camera(0, L"F3D0001")));
//    CameraManger::GetInst()->add(pCamera(new Camera(3, L"F3D0004")));
//    CameraManger::GetInst()->add(pCamera(new Camera(5, L"F3D0006")));
//    MultiCamera::GetInst()->addProc(new CamImageProc());
//    MultiCamera::GetInst()->MultiCamera::GetInst()->openCamera();
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//
//    MultiCamera::GetInst()->vProc.clear();
//    MultiCamera::GetInst()->close();
//}
