#include "pch.h"

#include "CVSystem/CVSystem.h"
#include <thread>
#include <chrono>

#include "dlog/dlog.h"

using namespace dxlib;
using namespace std;

class StereoTestProc : public FrameProc
{
  public:
    StereoTestProc() {}
    ~StereoTestProc() {}

    static int count;

    void process(pCameraImage camImage, int& key) override
    {
        //设置了真实相机不发送到前面来
        ASSERT_TRUE(camImage->vImage.size() == 2);

        ASSERT_TRUE(!camImage->getItem(0).image.empty()); //确实能采图
        ASSERT_TRUE(!camImage->getItem(1).image.empty()); //确实能采图

        ASSERT_TRUE(camImage->getItem(2).camera == nullptr);
        count++; //采图的计数加1

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

int StereoTestProc::count = 0;

//测试MultiCamera的open是否正常
TEST(Stereo, grab)
{
    StereoTestProc::count = 0;

    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList.begin()->second;

    cv::Size scSize = cv::Size(1280, 400);
    //cv::Size scSize = cv::Size(2560, 800);

    LogI("初始化设置...");

    CameraManger::GetInst()->clear();
    CameraManger::GetInst()->add(std::make_shared<Camera>(0, L"camL"));
    CameraManger::GetInst()->add(std::make_shared<Camera>(1, L"camR"));

    std::map<int, pCamera>& camMap = CameraManger::GetInst()->camMap;
    for (auto& kvp : camMap) {
        kvp.second->isVirtualCamera = true; //标记0,1为逻辑相机
        kvp.second->size = cv::Size(scSize.width / 2, scSize.height);
        kvp.second->paramSize = cv::Size(scSize.width / 2, scSize.height);
    }

    //记录一个立体相机
    auto sc = CameraManger::GetInst()->addAssist(std::make_shared<Camera>(2, camName, scSize, 0));
    sc->isStereoCamera = true;
    sc->isNoSendToProc = true; //设置不发送到前面处理
    sc->stereoCamIndexL = CameraManger::GetInst()->getCamera("camL")->camIndex;
    sc->stereoCamIndexR = CameraManger::GetInst()->getCamera("camR")->camIndex;
    sc->scID = 0;
    sc->setProp(cv::CAP_PROP_FPS, 60);
    //sc->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);
    sc->setProp(cv::CAP_PROP_EXPOSURE, -11);
    sc->setProp(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    //设置立体相机
    pStereoCamera sc1 = pStereoCamera(new StereoCamera());
    sc1->camL = camMap[0]; //暂时倒过来设置
    sc1->camR = camMap[1];
    sc1->camL->stereoOther = sc1->camR;
    sc1->camR->stereoOther = sc1->camL;
    CameraManger::GetInst()->add(sc1);

    MultiCamera::GetInst()->clearProc();
    MultiCamera::GetInst()->addProc(pFrameProc(new StereoTestProc()));

    MultiCamera::GetInst()->openCamera();

    //这里关掉控制台日志,看看处理速度
    dlog_console_log_enable(false);

    MultiCamera::GetInst()->start();

    while (true) {

        this_thread::sleep_for(chrono::milliseconds(100));
        if (StereoTestProc::count > 50) {
            break;
        }
    }

    dlog_console_log_enable(true);
    MultiCamera::GetInst()->stop();
    MultiCamera::GetInst()->closeCamera();
}
