#include "Camera.h"
#include "../dlog/dlog.h"
#include "../Common/Common.h"


namespace dxlib {

    Camera::Camera(int aCamIndex, std::wstring aDevName, cv::Size aSize, int aBrightness)
        : camIndex(aCamIndex), devName(aDevName), size(aSize)
    {
        //默认设置一个属性值
        prepareProp.BRIGHTNESS = aBrightness;
        prepareProp.FRAME_HEIGHT = aSize.height;
        prepareProp.FRAME_WIDTH = aSize.width;
        prepareProp.FOURCC = FLT_MIN;// CV_FOURCC('M', 'J', 'P', 'G');
        prepareProp.FPS = FLT_MIN;
        prepareProp.AUTO_EXPOSURE = FLT_MIN; //1
        prepareProp.FOCUS = FLT_MIN;
        prepareProp.EXPOSURE = FLT_MIN;

        devNameA = ws2s(aDevName);
        //capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());//一开始也构造一个，简化判断
    }

    void Camera::resetProp()
    {
        curProp.BRIGHTNESS = FLT_MIN;
        curProp.FRAME_HEIGHT = FLT_MIN;
        curProp.FRAME_WIDTH = FLT_MIN;
        curProp.FOURCC = FLT_MIN;
        curProp.FPS = FLT_MIN;
        curProp.AUTO_EXPOSURE = FLT_MIN;
        curProp.FOCUS = FLT_MIN;
        curProp.EXPOSURE = FLT_MIN;
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
        char fourcc[] = { (char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0 };

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
        for (size_t i = 0; i <= 41; i++) {
            int w = static_cast<int>(capture->get(i));
            LogI("Camera.outputAllProp(): %d = %d", i, w);
        }
    }

    std::string Camera::toFOURCC(double FOURCC)
    {
        int ex = (int)FOURCC;
        char fourcc[] = { (char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0 };
        return std::string(fourcc);
    }

    void Camera::initUndistortRectifyMap()
    {
        cv::initUndistortRectifyMap(this->camMatrix, this->distCoeffs, this->R, this->P, this->size, CV_16SC2, this->rmap1, this->rmap2);
    }

}