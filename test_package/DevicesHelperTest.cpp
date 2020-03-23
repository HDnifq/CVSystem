#include "pch.h"

#include "CVSystem/Proc/DevicesHelper.h"

using namespace dxlib;

//没有相机会失败
TEST(DevicesHelper, listDevices)
{
    int devCount = DevicesHelper::GetInst()->listDevices();
    EXPECT_TRUE(devCount > 0);
}