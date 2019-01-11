﻿#include "pch.h"

#include "../CVSystem/CVSystem.h"

using namespace dxlib;
using namespace std;


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

    for (size_t i = 0; i < 2; i++)
    {
        MultiCamera::GetInst()->openCamera();//打开相机
        EXPECT_TRUE(CameraManger::GetInst()->camMap[0]->isOpened());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));//工作500秒
        EXPECT_TRUE(MultiCamera::GetInst()->frameCount > 0);
        MultiCamera::GetInst()->release();
    }

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
    MultiCamera::GetInst()->release();
}