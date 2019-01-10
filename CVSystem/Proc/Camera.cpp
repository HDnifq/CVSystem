﻿#include "Camera.h"
#include "../dlog/dlog.h"
#include "../Common/Common.h"


namespace dxlib {

    Camera::Camera(int aCamIndex, std::wstring aDevName, cv::Size aSize, int aBrightness)
        : camIndex(aCamIndex), devName(aDevName), size(aSize), brightness(aBrightness)
    {
        devNameA = ws2s(aDevName);
        capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());//一开始也构造一个，简化判断
    }

    void Camera::setBrightness(int aBrightness, bool isForce)
    {
        if (aBrightness != this->brightness || isForce) {

            if (capture == nullptr || !capture->isOpened()) {
                LogW("Camera.setBrightness():设置相机%s亮度失败,相机未打开!", devNameA.c_str());
                return;
            }
            LogI("Camera.setBrightness():设置相机%d亮度为%d", camIndex, aBrightness);
            if (!capture->set(CV_CAP_PROP_BRIGHTNESS, aBrightness)) {
                LogW("Camera.setBrightness():设置亮度BRIGHTNESS失败.");
            }

            this->brightness = aBrightness;
        }
    }

    void Camera::initUndistortRectifyMap()
    {
        cv::initUndistortRectifyMap(this->camMatrix, this->distCoeffs, this->R, this->P, this->size, CV_16SC2, this->rmap1, this->rmap2);
    }

}