#include "CameraPair.h"
#include "dlog/dlog.h"

namespace dxlib {

void CameraPair::loadParam(const std::string& path, const std::string& nameTargetSize,
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
        LogE("CameraPair.loadParam():打开参数文件失败! path=%s", path.c_str());
    }
    fs.release();
}
} // namespace dxlib
