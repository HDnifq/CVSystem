#include "Camera.h"
#include "dlog/dlog.h"
#include "../Common/StringHelper.h"
#include "DevicesHelper.h"
#include <chrono>
#include <thread>

//所有的CV_CAP_PROP的数组的长度
#define DXLIB_CAMERA_CAP_PROP_LEN 128

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

struct Camera::Impl
{
    /// <summary> 暂存的上次设置的结果属性值,可以供随便的查询一下,但是一般用不到. </summary>
    double lastCapProp[DXLIB_CAMERA_CAP_PROP_LEN];

    /// <summary> 用来做状态的记录，好触发设置动作. </summary>
    double _setCapProp[DXLIB_CAMERA_CAP_PROP_LEN];
};

Camera::Camera(int aCamIndex, std::wstring aDevName, cv::Size aSize, int aBrightness)
    : camIndex(aCamIndex), devName(aDevName), size(aSize)
{
    //构造成员
    _impl = new Impl();

    for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        _impl->lastCapProp[i] = FLT_MIN;
    }
    for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        _impl->_setCapProp[i] = FLT_MIN;
    }

    //默认设置一个属性值
    //_impl->_setCapProp[CV_CAP_PROP_FOURCC] = CV_FOURCC('M', 'J', 'P', 'G');
    _impl->_setCapProp[CV_CAP_PROP_BRIGHTNESS] = aBrightness;
    //_impl->_setCapProp[CV_CAP_PROP_FRAME_HEIGHT] = aSize.height;
    //_impl->_setCapProp[CV_CAP_PROP_FRAME_WIDTH] = aSize.width;

    devNameA = StringHelper::ws2s(aDevName);

    paramSize = aSize;
}

Camera::~Camera()
{
    delete _impl;
}

void Camera::setProp(int CV_CAP_PROP, double value)
{
    _impl->_setCapProp[CV_CAP_PROP] = value;
}

bool Camera::open()
{
    if (isVirtualCamera) {
        return false;
    }

    //如果存在VideoCapture那么就释放
    if (capture != nullptr) {
        capture->release();
    }
    FPS = 0;

    capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());

    // CV_FOURCC('M', 'J', 'P', 'G');

    //列出设备
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        LogE("Camera.openCamera():listDevices相机个数为0,直接返回!");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    devID = DevicesHelper::GetInst()->getIndexWithName(devName); //记录devID
#else
    devID = DevicesHelper::GetInst()->getIndexWithName(devName, true); //记录devID,使用正则寻找
#endif
    if (devID != -1) { //如果打开失败会返回-1

        // In case a resource was already
        // associated with the VideoCapture instance
        capture->release();

        int count = 0;
        while (count < 3) { //重试3次
            try {
                //调用opencv的打开相机
#if defined(_WIN32) || defined(_WIN64)
                if (capture->open(devID)) {
#elif __linux
                if (capture->open(StringHelper::ws2s(devName), cv::CAP_V4L)) {
#endif
                    outputProp();
                    //重设一下等会要打开相机的时候需要设置的属性
                    //double dFourcc = capture->get(CV_CAP_PROP_FOURCC);
                    //重设一下等会要打开相机的时候需要设置的属性
                    setProp(CV_CAP_PROP_FRAME_HEIGHT, size.height);
                    setProp(CV_CAP_PROP_FRAME_WIDTH, size.width);
                    setProp(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
                    //capture->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));

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

void Camera::release()
{
    if (capture != nullptr) {
        capture->release();
    }
    FPS = 0;
}

bool Camera::applyCapProp()
{
    if (isVirtualCamera) {
        return true;
    }

    if (capture == nullptr || !capture->isOpened()) {
        return false;
    }

    for (int i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        if (_impl->_setCapProp[i] != FLT_MIN) {      //不等于表示有用户设置过了
            const char* str = CAP_PROP_STR[i];       //这个属性的字符串
            double setValue = _impl->_setCapProp[i]; //用户想设置的值

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
            _impl->_setCapProp[i] = FLT_MIN; //重新标记为默认值

            //读当前值然后顺便记录一下
            _impl->lastCapProp[i] = capture->get(i);
            if (i == CV_CAP_PROP_FOURCC) { //fourcc这个值得转一下才能log
                std::string fourcc = toFOURCC(_impl->lastCapProp[i]);
                LogI("Camera.applyCapProp():当前相机%d实际值 -> %s = %s", camIndex, str, fourcc.c_str());
            }
            else {
                LogI("Camera.applyCapProp():当前相机%d实际值 -> %s = %d", camIndex, str, (int)_impl->lastCapProp[i]);
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

    LogI("Camera.outputProp()相机当前w=%d h=%d fps=%d mode=%d fourcc=%s brightness=%d", w, h, fps, mode, fourcc, brightness);
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

cv::Point3d Camera::screenToWorld(cv::Point2f screenPoint, float z)
{
    screenPoint.y = this->size.height - screenPoint.y; //是以左下角为起点的
    const double* pd = this->camMatrix.ptr<double>();
    double fx = pd[0];
    double cx = pd[2];
    double fy = pd[4];
    double cy = pd[5];

    //世界坐标
    cv::Mat mp = (cv::Mat_<double>(4, 1) << (screenPoint.x - cx) / fx * z, (screenPoint.y - cy) / fy * z, z, 1);
    cv::Mat p = this->camRT4x4 * mp;
    return cv::Point3d{p.at<double>(0), p.at<double>(1), p.at<double>(2)};
}
} // namespace dxlib