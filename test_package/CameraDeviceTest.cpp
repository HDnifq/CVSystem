#include "pch.h"

#pragma execution_character_set("utf-8")
#include "CVSystem/CVSystem.h"
#include "time.h"

//用来测试s2ws_ws2s
#include "xuexuejson/JsonMapper.hpp"

using namespace dxlib;
using namespace std;

TEST(Video, MKVTest)
{
    cv::VideoCapture capture(""); //C:\\Users\\dx\\Desktop\\12-30-55_2a.mkv
    // check if video successfully opened 检测视频是否成功打开
    if (!capture.isOpened())
        return;

    // Get the frame rate 获取帧率
    double rate = capture.get(cv::CAP_PROP_FPS);

    cv::Mat frame; // current video frame 当前视频帧的图像
    cv::namedWindow("Extracted Frame");

    bool stop = false;
    // Delay between each frame 每一帧之间的延迟
    // corresponds to video frame rate 与视频的帧率相对应
    int delay = 1000 / rate;

    // for all frames in video 遍历每一帧
    while (!stop) {

        // read next frame if any 尝试读取下一帧
        if (!capture.read(frame)) break;
        cv::imshow("Extracted Frame", frame);

        // introduce a delay 引入延迟（使用waitKey()函数参数）
        // or press key to stop 也可通过按键停止
        if (cv::waitKey(delay) >= 0)
            stop = true;
    }

    // Close the video file
    capture.release();
}

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
    ASSERT_FALSE(camera.isError());

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

TEST(CameraDevice, fail)
{
    CameraDevice camera("31321fasdfas", cv::Size(640, 400), 0);
    camera.open();
    ASSERT_TRUE(camera.isError());
    ASSERT_TRUE(!camera.isOpened());
    ASSERT_TRUE(!camera.isOpening());
}
