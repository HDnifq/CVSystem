﻿#include "pch.h"

#pragma execution_character_set("utf-8")
#include "CVSystem/CVSystem.h"
#include "time.h"

//用来测试s2ws_ws2s
#include "xuexuejson/Serialize.hpp"

using namespace dxlib;
using namespace std;

TEST(CameraDevice, propMap)
{
    auto map = CameraDevice::propStringMap();

    EXPECT_TRUE(map.size() > 0);
    int i = 0;
    for (auto kvp : map) {
        cv::VideoCaptureProperties prop = CameraDevice::propStr2Enum(kvp.second);

        //在枚举的定义里没有31号
        if (i == 31) {
            i++;
        }
        ASSERT_EQ(prop, i);
        i++;
    }
}

TEST(CameraDevice, setProp)
{
    //得到第一个相机名
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        return;
    }
    string camName = DevicesHelper::GetInst()->devList.begin()->second;

    CameraDevice camera(camName, cv::Size(640, 400), 0);
    camera.open();
    //如果打开失败这里会为null
    ASSERT_TRUE(camera.capture != nullptr);
    ASSERT_TRUE(camera.capture->isOpened());

    camera.setPropWithString(cv::CAP_PROP_BRIGHTNESS, "-64");
    camera.applyCapProp();
    camera.outputAllProp();
    cv::Mat image;
    for (int i = 0; i < 20; i++) {
        camera.read(image);
        ASSERT_TRUE(!image.empty());
        LogI("采到的图片大小%d,%d", image.size().width, image.size().height);
        camera.setProp(cv::CAP_PROP_BRIGHTNESS, (double)(-64 + i));
        camera.applyCapProp();
    }
    camera.release();
}