#include "StereoCamera.h"
#include "dlog/dlog.h"
#include "xuexue/math/math.h"

namespace dxlib {

void StereoCamera::setCamera(int scID, const pCameraDevice& device, const pCamera& camL, const pCamera& camR)
{
    this->scID = scID;
    this->device = device;
    this->camL = camL;
    this->camR = camR;

    this->device->scID = scID;
    this->camL->scID = scID;
    this->camR->scID = scID;

    this->camL->device = device.get();
    this->camR->device = device.get();

    this->camL->stereoOther = this->camR;
    this->camR->stereoOther = this->camL;

    this->camL->stereoCamIndexL = this->camL->camIndex;
    this->camL->stereoCamIndexR = this->camR->camIndex;
    this->camR->stereoCamIndexL = this->camL->camIndex;
    this->camR->stereoCamIndexR = this->camR->camIndex;
}

void StereoCamera::setCamera(int scID, const pCamera& camL, const pCamera& camR)
{
    this->scID = scID;
    this->camL = camL;
    this->camR = camR;

    this->camL->scID = scID;
    this->camR->scID = scID;

    this->camL->stereoOther = this->camR;
    this->camR->stereoOther = this->camL;

    this->camL->stereoCamIndexL = this->camL->camIndex;
    this->camL->stereoCamIndexR = this->camR->camIndex;
    this->camR->stereoCamIndexL = this->camL->camIndex;
    this->camR->stereoCamIndexR = this->camR->camIndex;
}

void StereoCamera::createProjectMat()
{
    using namespace xuexue;
    cv::Mat LP = camL->camMatrix * Math::Mat3x4I();
    cv::Mat RP = camR->camMatrix * Math::TRMat3x4(R, T);
    setProjection(LP, RP);
}

void StereoCamera::setTRMat4x4(const cv::Mat& TRMat4x4)
{
    CV_Assert(TRMat4x4.size() == cv::Size(4, 4));
    using namespace xuexue;
    this->camTR4x4 = TRMat4x4;
    Math::toPosRot(TRMat4x4, this->camPos, this->camRotate);
    this->camL->camTR4x4 = TRMat4x4;
    this->camL->camPos = this->camPos;
    this->camL->camRotate = this->camRotate;
}

} // namespace dxlib