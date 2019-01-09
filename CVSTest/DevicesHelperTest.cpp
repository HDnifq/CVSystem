#include "pch.h"

#include "../CVSystem/Proc/DevicesHelper.h"

using namespace dxlib;

TEST(DevicesHelper, listDevices)
{
    int devCount = DevicesHelper::GetInst()->listDevices();
    EXPECT_TRUE(devCount > 0);
}