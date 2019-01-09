#include "pch.h"

#include "../CVSystem/Proc/CameraThread.h"
#include "../CVSystem/CVSystem.h"

using namespace dxlib;
using namespace std;
 

//测试CameraThread的delete释放是否正常
TEST(MultiCamera, open)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList->size() == 0) {
        return;
    }
    wstring camName = DevicesHelper::GetInst()->devList->begin()->second;

    CameraManger::GetInst()->add(pCamera(new Camera(0, camName)));

    MultiCamera::GetInst()->openCamera();//打开相机
    EXPECT_TRUE(CameraManger::GetInst()->camMap[0]->capture->isOpened());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));//工作500秒
    MultiCamera::GetInst()->release();
}