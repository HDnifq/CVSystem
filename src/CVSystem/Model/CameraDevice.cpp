#include "CameraDevice.h"

#include <chrono>
#include <thread>
#include <memory>

#include "../System/DevicesHelper.h"
#include <opencv2/videoio.hpp>

#include "dlog/dlog.h"

//所有的CV_CAP_PROP的数组的长度
#define DXLIB_CAMERA_CAP_PROP_LEN 64

namespace dxlib {

class CameraDevice::Impl
{
  public:
    Impl()
    {
        for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
            lastCapProp[i] = FLT_MIN;
        }
        for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
            _setCapProp[i] = FLT_MIN;
        }
    }
    ~Impl() {}

    // 暂存的读取的上次设置的实际结果属性值,可以供随便的查询一下,但是一般用不到.
    double lastCapProp[DXLIB_CAMERA_CAP_PROP_LEN];

    // 用来做状态的记录，好触发设置动作，执行过设置属性之后会把它置为FLT_MIN.
    double _setCapProp[DXLIB_CAMERA_CAP_PROP_LEN];

    // 相机属性和字符串对应的map.
    static std::map<cv::VideoCaptureProperties, const char*> mPropStr;
};

std::map<cv::VideoCaptureProperties, const char*> CameraDevice::Impl::mPropStr = {
    {cv::CAP_PROP_POS_MSEC, "CAP_PROP_POS_MSEC"},
    {cv::CAP_PROP_POS_FRAMES, "CAP_PROP_POS_FRAMES"},
    {cv::CAP_PROP_POS_AVI_RATIO, "CAP_PROP_POS_AVI_RATIO"},
    {cv::CAP_PROP_FRAME_WIDTH, "CAP_PROP_FRAME_WIDTH"},
    {cv::CAP_PROP_FRAME_HEIGHT, "CAP_PROP_FRAME_HEIGHT"},
    {cv::CAP_PROP_FPS, "CAP_PROP_FPS"},
    {cv::CAP_PROP_FOURCC, "CAP_PROP_FOURCC"},
    {cv::CAP_PROP_FRAME_COUNT, "CAP_PROP_FRAME_COUNT"},
    {cv::CAP_PROP_FORMAT, "CAP_PROP_FORMAT"},
    {cv::CAP_PROP_MODE, "CAP_PROP_MODE"},
    {cv::CAP_PROP_BRIGHTNESS, "CAP_PROP_BRIGHTNESS"},
    {cv::CAP_PROP_CONTRAST, "CAP_PROP_CONTRAST"},
    {cv::CAP_PROP_SATURATION, "CAP_PROP_SATURATION"},
    {cv::CAP_PROP_HUE, "CAP_PROP_HUE"},
    {cv::CAP_PROP_GAIN, "CAP_PROP_GAIN"},
    {cv::CAP_PROP_EXPOSURE, "CAP_PROP_EXPOSURE"},
    {cv::CAP_PROP_CONVERT_RGB, "CAP_PROP_CONVERT_RGB"},
    {cv::CAP_PROP_WHITE_BALANCE_BLUE_U, "CAP_PROP_WHITE_BALANCE_BLUE_U"},
    {cv::CAP_PROP_RECTIFICATION, "CAP_PROP_RECTIFICATION"},
    {cv::CAP_PROP_MONOCHROME, "CAP_PROP_MONOCHROME"},
    {cv::CAP_PROP_SHARPNESS, "CAP_PROP_SHARPNESS"},
    {cv::CAP_PROP_AUTO_EXPOSURE, "CAP_PROP_AUTO_EXPOSURE"},
    {cv::CAP_PROP_GAMMA, "CAP_PROP_GAMMA"},
    {cv::CAP_PROP_TEMPERATURE, "CAP_PROP_TEMPERATURE"},
    {cv::CAP_PROP_TRIGGER, "CAP_PROP_TRIGGER"},
    {cv::CAP_PROP_TRIGGER_DELAY, "CAP_PROP_TRIGGER_DELAY"},
    {cv::CAP_PROP_WHITE_BALANCE_RED_V, "CAP_PROP_WHITE_BALANCE_RED_V"},
    {cv::CAP_PROP_ZOOM, "CAP_PROP_ZOOM"},
    {cv::CAP_PROP_FOCUS, "CAP_PROP_FOCUS"},
    {cv::CAP_PROP_GUID, "CAP_PROP_GUID"},
    {cv::CAP_PROP_ISO_SPEED, "CAP_PROP_ISO_SPEED"},
    {cv::CAP_PROP_BACKLIGHT, "CAP_PROP_BACKLIGHT"},
    {cv::CAP_PROP_PAN, "CAP_PROP_PAN"},
    {cv::CAP_PROP_TILT, "CAP_PROP_TILT"},
    {cv::CAP_PROP_ROLL, "CAP_PROP_ROLL"},
    {cv::CAP_PROP_IRIS, "CAP_PROP_IRIS"},
    {cv::CAP_PROP_SETTINGS, "CAP_PROP_SETTINGS"},
    {cv::CAP_PROP_BUFFERSIZE, "CAP_PROP_BUFFERSIZE"},
    {cv::CAP_PROP_AUTOFOCUS, "CAP_PROP_AUTOFOCUS"},
    {cv::CAP_PROP_SAR_NUM, "CAP_PROP_SAR_NUM"},
    {cv::CAP_PROP_SAR_DEN, "CAP_PROP_SAR_DEN"},
    {cv::CAP_PROP_BACKEND, "CAP_PROP_BACKEND"},
    {cv::CAP_PROP_CHANNEL, "CAP_PROP_CHANNEL"},
    {cv::CAP_PROP_AUTO_WB, "CAP_PROP_AUTO_WB"},
    {cv::CAP_PROP_WB_TEMPERATURE, "CAP_PROP_WB_TEMPERATURE"},
    {cv::CAP_PROP_CODEC_PIXEL_FORMAT, "CAP_PROP_CODEC_PIXEL_FORMAT"}};

CameraDevice::CameraDevice(const std::string& aDevName, cv::Size aSize, int aBrightness)
    : devName(aDevName), size(aSize)
{
    //构造成员
    _impl = new Impl();

    //因为没有成员变量,所以只能设置一下
    _impl->_setCapProp[cv::VideoCaptureProperties::CAP_PROP_BRIGHTNESS] = aBrightness;
}

CameraDevice::~CameraDevice()
{
    delete _impl;
}

bool CameraDevice::open(float& costTime)
{
    //统计时间
    costTime = 0;
    clock_t startTime = clock();

    //如果存在VideoCapture那么就释放
    if (capture == nullptr) {
        capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());
    }
    else {
        capture->release();
    }
    FPS = 0;

    //默认设置一个属性值
    //_impl->_setCapProp[CV_CAP_PROP_FOURCC] = CV_FOURCC('M', 'J', 'P', 'G');

    _impl->_setCapProp[cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT] = size.height;
    _impl->_setCapProp[cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH] = size.width;

    //列出设备
    DevicesHelper::GetInst()->listDevices();
    if (DevicesHelper::GetInst()->devList.size() == 0) {
        LogE("CameraDevice.open():listDevices相机个数为0,直接返回!");
        return false;
    }

#if __linux
    devID = DevicesHelper::GetInst()->getIndexWithName(devName, true); //记录devID,使用正则寻找
#else
    devID = DevicesHelper::GetInst()->getIndexWithName(devName); //记录devID
#endif

    //如果获取ID失败会返回-1
    if (devID == -1) {
        LogE("CameraDevice.open():未找到该名称的相机%s!", this->devName.c_str());
        for (auto& kvp : DevicesHelper::GetInst()->devList) {
            LogE("CameraDevice.open():当前相机有:%s", kvp.second.c_str());
        }

        release(); //去把capture置为null
        return false;
    }

    // In case a resource was already
    // associated with the VideoCapture instance
    capture->release();

    int count = 0;
    while (true) { //重试5次，调用opencv的打开相机
        try {
            //打开相机是否成功
            bool isSuccess = false;
#if __linux
            isSuccess = capture->open(DevicesHelper::GetInst()->devList[devID], cv::CAP_V4L);
#else
            //这个open函数可能会弹窗,导致一直卡死在这里
            isSuccess = capture->open(devID, cv::CAP_DSHOW);
#endif

            if (isSuccess) {
                outputProp();
                //重设一下等会要打开相机的时候需要设置的属性
                //setProp(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G')); //下面的applyCapProp里生效
                //capture->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));//立即生效

                applyCapProp(); //打开成功之后立马设置一下相机属性
                break;          //打开成功
            }
            else {
                LogW("CameraDevice.open():打开摄像头失败，重试。。。");
            }
        }
        catch (const std::exception& e) {
            LogE("CameraDevice.open():打开摄像头异常:%s ", e.what());
        }
        count++;

        capture->release();

        if (count == 3) { //失败超过5次
            LogE("CameraDevice.open():打开摄像头失败!");
            release(); //去把capture置为null
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); //休眠2000ms
    }

    //打开相机成功之后会进入这里
    outputProp(); //输出一下当前关心的相机属性状态

    //统计时间
    costTime = (float)(clock() - startTime) / CLOCKS_PER_SEC * 1000;

    return true;
}

bool CameraDevice::open()
{
    float costTime;
    return open(costTime);
}

bool CameraDevice::isOpened()
{
    if (capture == nullptr) {
        return false;
    }
    else {
        return capture->isOpened();
    }
}

bool CameraDevice::read(cv::Mat& image)
{
    if (!isOpened()) {
        return false;
    }
    return capture->read(image);
}

void CameraDevice::release()
{
    if (capture != nullptr) {
        capture->release();
        capture = nullptr;
    }
    //清空属性设置的触发
    for (size_t i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        _impl->_setCapProp[i] = FLT_MIN;
    }
    FPS = 0;
}

#pragma region 相机属性

void CameraDevice::setProp(cv::VideoCaptureProperties CV_CAP_PROP, double value)
{
    _impl->_setCapProp[(int)CV_CAP_PROP] = value;
}

void CameraDevice::setPropWithString(cv::VideoCaptureProperties CV_CAP_PROP, std::string value)
{
    //如果这是fourcc
    if (CV_CAP_PROP == cv::CAP_PROP_FOURCC) {
        _impl->_setCapProp[(int)CV_CAP_PROP] = cv::VideoWriter::fourcc(value[0], value[1], value[2], value[3]);
    }
    else {
        _impl->_setCapProp[(int)CV_CAP_PROP] = std::stod(value);
    }
}

void CameraDevice::setFourcc(const std::string& fourcc)
{
    _impl->_setCapProp[(int)cv::CAP_PROP_FOURCC] = cv::VideoWriter::fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
}

bool CameraDevice::applyCapProp()
{

    if (capture == nullptr || !capture->isOpened()) {
        return false;
    }

    for (int i = 0; i < DXLIB_CAMERA_CAP_PROP_LEN; i++) {
        if (_impl->_setCapProp[i] != FLT_MIN) {                                              //不等于FLT_MIN表示有用户设置过了
            const char* sProp = CameraDevice::Impl::mPropStr[(cv::VideoCaptureProperties)i]; //这个属性的字符串
            double setValue = _impl->_setCapProp[i];                                         //用户想设置的值

            //输出用户想设置的值
            if (i == cv::VideoCaptureProperties::CAP_PROP_FOURCC) { //fourcc这个值得转一下才能log
                std::string fourcc = toFOURCC(setValue);
                LogI("CameraDevice.applyCapProp():设置相机%s -> %s = %s", devName.c_str(), sProp, fourcc.c_str());
            }
            else {
                LogI("CameraDevice.applyCapProp():设置相机%s -> %s = %d", devName.c_str(), sProp, (int)setValue);
            }

            //进行设置
            if (!capture->set(i, setValue)) {
                LogW("CameraDevice.applyCapProp():设置%s失败.", sProp);
            }
            _impl->_setCapProp[i] = FLT_MIN; //重新标记为默认值

            //读当前值然后顺便记录一下
            _impl->lastCapProp[i] = capture->get(i);
            if (i == cv::VideoCaptureProperties::CAP_PROP_FOURCC) { //fourcc这个值得转一下才能log
                std::string fourcc = toFOURCC(_impl->lastCapProp[i]);
                LogI("CameraDevice.applyCapProp():当前相机%s实际值 -> %s = %s", devName.c_str(), sProp, fourcc.c_str());
            }
            else {
                LogI("CameraDevice.applyCapProp():当前相机%s实际值 -> %s = %d", devName.c_str(), sProp, (int)_impl->lastCapProp[i]);
            }
        }
    }
    return true;
}

std::map<std::string, std::string> CameraDevice::outputProp()
{
    std::map<std::string, std::string> mprops;
    if (capture == nullptr || !capture->isOpened()) {
        return mprops;
    }
    int w = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH));
    mprops["CAP_PROP_FRAME_WIDTH"] = std::to_string(w);

    int h = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT));
    mprops["CAP_PROP_FRAME_HEIGHT"] = std::to_string(h);

    int fps = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_FPS));
    mprops["CAP_PROP_FPS"] = std::to_string(fps);

    int mode = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_MODE));
    mprops["CAP_PROP_MODE"] = std::to_string(mode);

    int ex = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_FOURCC));
    char fourcc[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0};
    mprops["CAP_PROP_FOURCC"] = fourcc;

    int brightness = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_BRIGHTNESS));
    mprops["CAP_PROP_BRIGHTNESS"] = std::to_string(brightness);

    int EXPOSURE = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_EXPOSURE));
    mprops["CAP_PROP_EXPOSURE"] = std::to_string(EXPOSURE);

    int FOCUS = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_FOCUS));
    mprops["CAP_PROP_FOCUS"] = std::to_string(FOCUS);

    int AUTO_EXPOSURE = static_cast<int>(capture->get(cv::VideoCaptureProperties::CAP_PROP_AUTO_EXPOSURE));
    mprops["AUTO_EXPOSURE"] = std::to_string(AUTO_EXPOSURE);

    LogI("CameraDevice.outputProp()相机当前w=%d h=%d fps=%d mode=%d fourcc=%s brightness=%d", w, h, fps, mode, fourcc, brightness);
    LogI("CameraDevice.outputProp()AUTO_EXPOSURE=%d EXPOSURE=%d FOCUS=%d", AUTO_EXPOSURE, EXPOSURE, FOCUS);
    return mprops;
}

std::map<std::string, std::string> CameraDevice::outputAllProp()
{
    std::map<std::string, std::string> mprops;
    if (capture == nullptr || !capture->isOpened()) {
        return mprops;
    }
    for (int i = 0; i <= 41; i++) {
        //31在VideoCaptureProperties里没有定义
        if (i == 31) {
            continue;
        }
        int v = static_cast<int>(capture->get(i));
        LogI("CameraDevice.outputAllProp(): %s = %d", CameraDevice::Impl::mPropStr[(cv::VideoCaptureProperties)i], v);
        if ((cv::VideoCaptureProperties)i == cv::CAP_PROP_FOURCC) {
            char fourcc[] = {(char)(v & 0XFF), (char)((v & 0XFF00) >> 8), (char)((v & 0XFF0000) >> 16), (char)((v & 0XFF000000) >> 24), 0};
            mprops[CameraDevice::Impl::mPropStr[(cv::VideoCaptureProperties)i]] = fourcc;
        }
        else {
            mprops[CameraDevice::Impl::mPropStr[(cv::VideoCaptureProperties)i]] = std::to_string(v);
        }
    }
    return mprops;
}

cv::VideoCaptureProperties CameraDevice::propStr2Enum(const std::string& str)
{
    auto& map = CameraDevice::Impl::mPropStr;
    auto itr = std::find_if(map.begin(), map.end(),
                            [str](const std::pair<cv::VideoCaptureProperties, const char*>& t) { return t.second == str; });

    if (itr != map.end()) {
        return itr->first;
    }
    //如果上面的判断都没进那么随便返回一个最后一个不用的值算了
    return cv::CV__CAP_PROP_LATEST;
}

std::map<cv::VideoCaptureProperties, const char*> CameraDevice::propStringMap()
{
    return CameraDevice::Impl::mPropStr;
}

#pragma endregion

std::string CameraDevice::toFOURCC(double FOURCC)
{
    int ex = (int)FOURCC;
    char fourcc[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0};
    return std::string(fourcc);
}

} // namespace dxlib