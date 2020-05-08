#include "StereoCamera.h"
#include "dlog/dlog.h"
#include "xuexue/math/math.h"

namespace dxlib {

void StereoCamera::loadParam(const std::string& path, const std::string& nameTargetSize,
                             const std::string& nameCamMatrixL,
                             const std::string& nameDistCoeffsL,
                             const std::string& nameCamMatrixR,
                             const std::string& nameDistCoeffsR,
                             const std::string& nameR,
                             const std::string& nameT,
                             const std::string& nameE,
                             const std::string& nameF,
                             const std::string& nameQ,
                             const std::string& nameR1,
                             const std::string& nameP1,
                             const std::string& nameR2,
                             const std::string& nameP2)
{

    using namespace cv;
    FileStorage fs(path, FileStorage::READ); //参数
    if (fs.isOpened()) {
        if (!nameTargetSize.empty()) {
            fs[nameTargetSize] >> paramSize;
            fs[nameTargetSize] >> this->camL->paramSize;
            fs[nameTargetSize] >> this->camR->paramSize;
        }
        if (!nameCamMatrixL.empty())
            fs[nameCamMatrixL] >> this->camL->camMatrix;
        if (!nameDistCoeffsL.empty())
            fs[nameDistCoeffsL] >> this->camL->distCoeffs;

        if (!nameCamMatrixR.empty())
            fs[nameCamMatrixR] >> this->camR->camMatrix;
        if (!nameDistCoeffsR.empty())
            fs[nameDistCoeffsR] >> this->camR->distCoeffs;

        if (!nameR.empty())
            fs[nameR] >> this->R;

        if (!nameT.empty())
            fs[nameT] >> this->T;

        if (!nameE.empty())
            fs[nameE] >> this->E;

        if (!nameF.empty())
            fs[nameF] >> this->F;

        if (!nameQ.empty())
            fs[nameQ] >> this->Q;

        if (!nameR1.empty())
            fs[nameR1] >> this->camL->R;

        if (!nameP1.empty())
            fs[nameP1] >> this->camL->P;

        if (!nameR2.empty())
            fs[nameR2] >> this->camR->R;

        if (!nameP2.empty())
            fs[nameP2] >> this->camR->P;
    }
    else {
        LogE("StereoCamera.loadParam():打开参数文件失败! path=%s", path.c_str());
    }
    fs.release();
}

bool StereoCamera::SwitchParam(cv::Size size)
{
    //搜索一下是否带有这个参数
    for (size_t i = 0; i < vParams.size(); i++) {
        //如果找到了这参数,就按当前参数设置数据
        if (vParams[i].first == size) {

            //pCameraParam param = vParams[i].second;
            //this->camMatrix = param->camMatrix;
            //this->distCoeffs = param->distCoeffs;
            //this->camMatrix = param->camMatrix;

            //this->projection = param->projection;
            //this->R = param->R;
            //this->P = param->P;
            //this->camTR4x4 = param->camTR4x4;
            return true;
        }
    }
    return false;
}

void StereoCamera::createProjectMat()
{
    using namespace xuexue;
    cv::Mat LP = camL->camMatrix * Math::Mat3x4I();
    cv::Mat RP = camR->camMatrix * Math::TRMat3x4(R, T);
    setProjection(LP, RP);
}

pStereoCameraParam StereoCamera::getParam(cv::Size size)
{
    for (size_t i = 0; i < vParams.size(); i++) {
        if (vParams[i].first == size) {
            return vParams[i].second;
        }
    }
    return nullptr;
}

} // namespace dxlib