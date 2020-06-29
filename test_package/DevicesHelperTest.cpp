#include "pch.h"

#include "CVSystem/System/DevicesHelper.h"
#include "CVSystem/DShow/UVCCameraLibrary.h"

using namespace dxlib;

//没有相机会失败
TEST(DevicesHelper, listDevices)
{
    int devCount = DevicesHelper::GetInst()->listDevices();
    EXPECT_TRUE(devCount > 0);
}

TEST(UVCCameraLibrary, listDevices)
{
    std::vector<std::string> list;
    UVCCameraLibrary::listDevices(list);

    UVCCameraLibrary uvc;

    if (uvc.connectDevice("C920")) {
        auto caps = uvc.getCapabilities();
        UVCProp focus = uvc.getFocus();
        UVCProp exposure = uvc.getExposure();
        uvc.setAutoFocus(false);
        uvc.setAutoExposure(false);
        uvc.setLowLightCompensation(false);
        uvc.setExposure(-7);

        UVCProp brightness = uvc.getBrightness();
        UVCProp gain = uvc.getGain();
        uvc.setGain(255);
    }
}