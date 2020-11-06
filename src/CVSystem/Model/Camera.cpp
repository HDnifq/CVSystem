#include "dlog/dlog.h"
#include <chrono>
#include <thread>
#include "Camera.h"

#include "../Common/StringHelper.h"
#include "../System/DevicesHelper.h"
#include "../System/CameraManger.h"

#include <opencv2/videoio.hpp>

//所有的CV_CAP_PROP的数组的长度
#define DXLIB_CAMERA_CAP_PROP_LEN 64

using namespace dxlib::cvsystem;

namespace dxlib {

class Camera::Impl
{
  public:
    Impl()
    {
    }

    ~Impl() {}
};

Camera::Camera(const std::string& aName, cv::Size aSize)
    : name(aName), size(aSize)
{
    //构造成员.
    _impl = new Impl();

    //默认的参数size等于画面size.
    paramSize = aSize;
}

Camera::~Camera()
{
    delete _impl;
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
    cv::Mat p = this->camTR4x4 * mp;
    return cv::Point3d{p.at<double>(0), p.at<double>(1), p.at<double>(2)};
}

bool Camera::SwitchParam(cv::Size size)
{
    //搜索一下是否带有这个参数
    for (size_t i = 0; i < vParams.size(); i++) {
        //如果找到了这参数,就按当前参数设置数据
        if (vParams[i].first == size) {
            pCameraParam param = vParams[i].second;
            this->camMatrix = param->camMatrix;
            this->distCoeffs = param->distCoeffs;
            this->camMatrix = param->camMatrix;

            this->projection = param->projection;
            this->R = param->R;
            this->P = param->P;
            this->camTR4x4 = param->camTR4x4;
            return true;
        }
    }
    return false;
}

pCameraParam Camera::getParam(cv::Size size)
{
    for (size_t i = 0; i < vParams.size(); i++) {
        if (vParams[i].first == size) {
            return vParams[i].second;
        }
    }
    return nullptr;
}

void Camera::loadParam(const std::string& path, const std::string& nameTargetSize,
                       const std::string& nameCamMatrix,
                       const std::string& nameDistCoeffs,
                       bool isApply)
{
    using namespace cv;
    FileStorage fs(path, FileStorage::READ); //参数
    if (fs.isOpened()) {
        Size targetSize;
        pCameraParam pParam = nullptr;
        if (!nameTargetSize.empty()) {
            fs[nameTargetSize] >> targetSize;
            //搜索一下是否带有这个参数
            for (size_t i = 0; i < vParams.size(); i++) {
                if (vParams[i].first == targetSize) {
                    pParam = vParams[i].second;
                    break;
                }
            }
            //如果没有搜索到就添加这个参数的记录
            if (pParam == nullptr) {
                pParam = std::make_shared<CameraParam>();
                vParams.push_back({targetSize, pParam});
            }

            if (!nameCamMatrix.empty())
                fs[nameCamMatrix] >> pParam->camMatrix;
            if (!nameDistCoeffs.empty())
                fs[nameDistCoeffs] >> pParam->distCoeffs;
        }
        else {
            LogE("Camera.loadParam():打开参数文件失败! path=%s", path.c_str());
        }
        fs.release();
        if (isApply) {
            SwitchParam(targetSize);
        }
    }
}

void StereoCamera::setCameraPhyLR(pCamera& camPhy, pCamera& camL, pCamera& camR)
{
    this->camPhy = camPhy;
    this->camL = camL;
    this->camR = camR;

    this->camL->physicalCamera = camPhy;
    this->camR->physicalCamera = camPhy;

    this->camL->stereoOther = this->camR;
    this->camR->stereoOther = this->camL;

    this->camPhy->stereoCamIndexL = camL->camIndex;
    this->camPhy->stereoCamIndexR = camR->camIndex;

    if (this->scID >= 0) {
        this->camPhy->scID = this->scID;
        this->camL->scID = this->scID;
        this->camR->scID = this->scID;
    }
    else {
        LogE("StereoCamera.setCameraPhyLR():未分配scID, 应该先把StereoCamera添加进CameraManger中分配scID!");
    }
}

} // namespace dxlib