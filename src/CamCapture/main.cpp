#include "CVSystem/CVSystem.h"

#include "dlog/dlog.h"

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
        CameraManger::GetInst()->add(pCamera(new Camera(camName)));
    }

    CameraManger::GetInst()->camMap[0]->setProp(cv::CAP_PROP_AUTO_EXPOSURE, 0);
    MultiCamera::GetInst()->addProc(new TestProc());
    MultiCamera::GetInst()->openCamera(); //打开相机

    //输出一下
    CameraManger::GetInst()->camMap[0]->outputProp();

    //启动计算线程
    MultiCamera::GetInst()->start();

    getchar();
    dlog_close();
    return 0;
}