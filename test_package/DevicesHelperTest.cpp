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

    if (uvc.connectDevice("C920")) { //Bison
        auto caps = uvc.getCapabilities();
        auto allCaps = uvc.getAllProp();
        UVCProp focus = uvc.getFocus();
        UVCProp exposure = uvc.getExposure();

        uvc.setLowLightCompensation(false);
        allCaps = uvc.getAllProp();

        uvc.setAutoFocus(false);
        uvc.setAutoExposure(false);

        uvc.setExposure(-7);

        UVCProp brightness = uvc.getBrightness();
        UVCProp gain = uvc.getGain();
        uvc.setGain(255);
        allCaps = uvc.getAllProp();
        uvc.setLowLightCompensation(true);
        allCaps = uvc.getAllProp();
    }
}