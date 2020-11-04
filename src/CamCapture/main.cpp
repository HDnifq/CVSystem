#include "CVSystem/CVSystem.h"

#include "dlog/dlog.h"
#include <thread>

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



int main(int argc, char* argv[])
{
    dlog_init("log", "CamCapture", dlog_init_relative::MODULE);
    CameraManger::GetInst()->clear();
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        LogE("没有发现相机!");
        dlog_close();
        return 1;
    }
    LogI("找到了%zu个相机", DevicesHelper::GetInst()->devList.size());

    for (auto& kvp : DevicesHelper::GetInst()->devList)
    {
        string camName = kvp.second;
        CameraManger::GetInst()->add(pCamera(new Camera(camName, cv::Size(1280, 400))));

    }

    std::map<int, pCamera>& camMap = CameraManger::GetInst()->camMap;
    for (auto& kvp : camMap) {
        kvp.second->isVirtualCamera = true; //标记0,1为逻辑相机
        kvp.second->size = cv::Size(1280, 400);
        kvp.second->paramSize = cv::Size(640, 400);

        kvp.second->setProp(cv::CAP_PROP_FPS, 60);
        //sc->setProp(CV_CAP_PROP_AUTO_EXPOSURE, 0);
        kvp.second->setProp(cv::CAP_PROP_EXPOSURE, -11);
        //输出一下
        kvp.second->outputProp();
    }

    MultiCamera::GetInst()->addProc(new TestProc());
    MultiCamera::GetInst()->openCamera(); //打开相机

    //启动计算线程
    MultiCamera::GetInst()->start();

    dlog_set_console_thr(dlog_level::info);
    dlog_set_file_thr(dlog_level::info);
    while (true)
    {
        LogI("当前fps=%s", MultiCamera::GetInst()->fps());
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    dlog_close();
    return 0;
}