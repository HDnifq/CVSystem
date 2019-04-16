#include "Camera.h"
#include "../dlog/dlog.h"
#include "../Common/StringHelper.h"
#include "DevicesHelper.h"
#include <Chrono>
#include <Thread>

namespace dxlib {

const char* CAP_PROP_STR[] = {
    "POS_MSEC",
    "POS_FRAMES",
    "POS_AVI_RATIO",
    "FRAME_WIDTH",
    "FRAME_HEIGHT",
    "FPS",
    "FOURCC",
    "FRAME_COUNT",
    "FORMAT",
    "MODE",
    "BRIGHTNESS",
    "CONTRAST",
    "SATURATION",
    "HUE",
    "GAIN",
    "EXPOSURE",
    "CONVERT_RGB",
    "WHITE_BALANCE_BLUE_U",
    "RECTIFICATION",
    "MONOCHROME",
    "SHARPNESS",
    "AUTO_EXPOSURE",
    "GAMMA",
    "TEMPERATURE",
    "TRIGGER",
    "TRIGGER_DELAY",
    "WHITE_BALANCE_RED_V",
    "ZOOM",
    "FOCUS",
    "GUID",
    "ISO_SPEED",
    "MAX_DC1394",
    "BACKLIGHT",
    "PAN",
    "TILT",
    "ROLL",
    "IRIS",
    "SETTINGS",
    "BUFFERSIZE",
    "AUTOFOCUS",
    "SAR_NUM",
    "SAR_DEN",
};

Camera::Camera(int aCamIndex, std::wstring aDevName, cv::Size aSize, int aBrightness)
    : camIndex(aCamIndex), devName(aDevName), size(aSize)
{
    for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        lastCapProp[i] = FLT_MIN;
    }
    for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        _setCapProp[i] = FLT_MIN;
    }

    //默认设置一个属性值
    _setCapProp[CV_CAP_PROP_BRIGHTNESS] = aBrightness;
    _setCapProp[CV_CAP_PROP_FRAME_HEIGHT] = aSize.height;
    _setCapProp[CV_CAP_PROP_FRAME_WIDTH] = aSize.width;

    devNameA = StringHelper::ws2s(aDevName);

    paramSize = aSize;
}

bool Camera::openCamera()
{
    //如果存在VideoCapture那么就释放
    if (capture != nullptr) {
        capture->release();
    }

    capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());

    //重设一下等会要打开相机的时候需要设置的属性
    _setCapProp[CV_CAP_PROP_FRAME_HEIGHT] = size.height;
    _setCapProp[CV_CAP_PROP_FRAME_WIDTH] = size.width;
    // CV_FOURCC('M', 'J', 'P', 'G');

    //列出设备
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        LogE("Camera.openCamera():listDevices相机个数为0,直接返回!");
        return false;
    }
    devID = DevicesHelper::GetInst()->getIndexWithName(devName); //记录devID
    if (devID != -1) {                                           //如果打开失败会返回-1

        // In case a resource was already
        // associated with the VideoCapture instance
        capture->release();

        int count = 0;
        while (count < 3) { //重试3次
            try {
                //调用opencv的打开相机
                if (capture->open(devID)) {
                    applyCapProp(); //打开成功之后设置一下相机属性
                    break;          //打开成功
                }
                else {
                    LogW("Camera.openCamera():打开摄像头失败，重试。。。");
                }
            }
            catch (const std::exception& e) {
                LogE("Camera.openCamera():打开摄像头异常:%s ", e.what());
            }
            count++;

            capture->release();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //休眠1000ms
        }

        if (count == 3) { //失败超过3次
            LogE("Camera.openCamera():打开摄像头失败!");
            return false;
        }

        outputProp(); //输出一下当前关心的相机属性状态
        return true;
    }
    else {
        LogE("Camera.openCamera():未找到该名称的相机%s!", this->devNameA.c_str());
        for (auto& kvp : DevicesHelper::GetInst()->devList) {
            LogE("Camera.openCamera():当前相机有:%s", StringHelper::ws2s(kvp.second).c_str());
        }

        return false;
    }
}

void Camera::releaseCamera()
{
    if (capture != nullptr) {
        capture->release();
    }
    FPS = 0;
}

bool Camera::applyCapProp()
{
    if (capture == nullptr || !capture->isOpened()) {
        return false;
    }

    for (int i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        if (_setCapProp[i] != FLT_MIN) {       //不等于表示有用户设置过了
            const char* str = CAP_PROP_STR[i]; //这个属性的字符串
            double setValue = _setCapProp[i];  //用户想设置的值

            //输出用户想设置的值
            if (i == CV_CAP_PROP_FOURCC) { //fourcc这个值得转一下才能log
                std::string fourcc = toFOURCC(setValue);
                LogI("Camera.applyCapProp():设置相机%d -> %s = %s", camIndex, str, fourcc.c_str());
            }
            else {
                LogI("Camera.applyCapProp():设置相机%d -> %s = %d", camIndex, str, (int)setValue);
            }

            //进行设置
            if (!capture->set(i, setValue)) {
                LogW("Camera.applyCapProp():设置%s失败.", str);
            }
            _setCapProp[i] = FLT_MIN; //重新标记为默认值

            //读当前值然后顺便记录一下
            lastCapProp[i] = capture->get(i);
            if (i == CV_CAP_PROP_FOURCC) { //fourcc这个值得转一下才能log
                std::string fourcc = toFOURCC(lastCapProp[i]);
                LogI("Camera.applyCapProp():当前相机%d实际值 -> %s = %s", camIndex, str, fourcc.c_str());
            }
            else {
                LogI("Camera.applyCapProp():当前相机%d实际值 -> %s = %d", camIndex, str, (int)lastCapProp[i]);
            }
        }
    }
    return true;
}

void Camera::outputProp()
{
    if (capture == nullptr || !capture->isOpened()) {
        return;
    }
    int w = static_cast<int>(capture->get(CV_CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(capture->get(CV_CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(capture->get(CV_CAP_PROP_FPS));
    int mode = static_cast<int>(capture->get(CV_CAP_PROP_MODE));
    int ex = static_cast<int>(capture->get(CV_CAP_PROP_FOURCC));
    char fourcc[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0};

    int brightness = static_cast<int>(capture->get(CV_CAP_PROP_BRIGHTNESS));
    int EXPOSURE = static_cast<int>(capture->get(CV_CAP_PROP_EXPOSURE));
    int FOCUS = static_cast<int>(capture->get(CV_CAP_PROP_FOCUS));
    int AUTO_EXPOSURE = static_cast<int>(capture->get(CV_CAP_PROP_AUTO_EXPOSURE));

    LogI("Camera.outputProp()相机当前w =%d h=%d fps=%d mode=%d fourcc=%s brightness=%d", w, h, fps, mode, fourcc, brightness);
    LogI("Camera.outputProp()AUTO_EXPOSURE=%d EXPOSURE=%d FOCUS=%d", AUTO_EXPOSURE, EXPOSURE, FOCUS);
}

void Camera::outputAllProp()
{
    if (capture == nullptr || !capture->isOpened()) {
        return;
    }
    for (int i = 0; i <= 41; i++) {
        int w = static_cast<int>(capture->get(i));
        LogI("Camera.outputAllProp(): %d = %d", i, w);
    }
}

std::string Camera::toFOURCC(double FOURCC)
{
    int ex = (int)FOURCC;
    char fourcc[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0};
    return std::string(fourcc);
}

void Camera::initUndistortRectifyMap()
{
    if (this->camMatrix.empty()) {
        LogE("Camera.initUndistortRectifyMap():当前相机camMatrix为空!");
        return;
    }
    if (this->distCoeffs.empty()) {
        LogE("Camera.initUndistortRectifyMap():当前相机distCoeffs为空!");
        return;
    }
    cv::initUndistortRectifyMap(this->camMatrix, this->distCoeffs, this->R, this->P, this->size, CV_16SC2, this->rmap1, this->rmap2);
}

} // namespace dxlib