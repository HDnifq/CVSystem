#include "pch.h"

#include "CVSystem/System/DevicesHelper.h"
#include "CVSystem/DShow/UVCCamera.h"

using namespace dxlib;

//没有相机会失败
TEST(DevicesHelper, listDevices)
{
    int devCount = DevicesHelper::GetInst()->listDevices();
    EXPECT_TRUE(devCount > 0);
}

TEST(UVCCamera, listDevices)
{
    std::vector<std::string> list;
    UVCCamera::listDevices(list);

    UVCCamera uvc;

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
        auto allCaps = uvc.getAllProp();
    }
}