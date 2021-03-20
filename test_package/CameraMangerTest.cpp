#include "pch.h"

#include "CVSystem/CVSystem.h"

using namespace dxlib;

TEST(CameraManger, Create)
{
    CameraManger::GetInst()->clear();

    pCameraDevice device;
    pStereoCamera stereo;
    CameraManger::GetInst()->CreateStereoCamera("weidoo01", 1280, 400, device, stereo);

    ASSERT_TRUE(device != nullptr);
    ASSERT_TRUE(stereo != nullptr);

    ASSERT_EQ(device->scID, 0);
    ASSERT_EQ(stereo->scID, 0);
    ASSERT_EQ(stereo->camL->scID, 0);
    ASSERT_EQ(stereo->camR->scID, 0);

    ASSERT_EQ(device->id, 0);

    ASSERT_EQ(stereo->camL->camIndex, 0);
    ASSERT_EQ(stereo->camR->camIndex, 1);

    ASSERT_EQ(stereo->camL->device, device.get());
    ASSERT_EQ(stereo->camR->device, device.get());

    CameraManger::GetInst()->clear();
}