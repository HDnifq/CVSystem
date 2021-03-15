#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "CameraDevice.h"
#include "StereoCameraParam.h"
#include "dlog/dlog.h"

namespace dxlib {

/**
 * 立体相机(实际上它叫StereoCameraInfo更贴切).
 *
 * @author daixian
 * @date 2018/11/23
 */
class IStereo
{
  public:
    virtual ~IStereo() {}

    // 这个立体相机组的名字.
    std::string name;

    // L相机.
    pCamera camL;

    // R相机.
    pCamera camR;

    // 相机的参数对应的分辨率size.
    cv::Size paramSize;

    // 参数R.
    cv::Mat R;

    // 参数T.
    cv::Mat T;

    // 参数E.
    cv::Mat E;

    // 参数F.
    cv::Mat F;

    // 参数Q.
    cv::Mat Q;

    // 左相机的投影矩阵.
    cv::Mat LP;

    // 右相机的投影矩阵.
    cv::Mat RP;

    // 这组相机3d空间到某世界空间的变换矩阵,它应该等于camL里的的相机camTR4x4.
    cv::Mat camTR4x4;

    // 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec3d camPos;

    // 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec4d camRotate;

    /**
     * 得到一个相机的数组，index=0表示L相机，index=1表示R相机.
     *
     * @author daixian
     * @date 2020/3/30
     *
     * @returns The cameras.
     */
    std::array<pCamera, 2> getCameras() const
    {
        std::array<pCamera, 2> arr = {camL, camR};
        return arr;
    }

    /**
     * 根据一个相机文件载入尝试相机参数信息.
     *
     * @author daixian
     * @date 2020/5/3
     *
     * @param  path            参数文件的完整路径.
     * @param  kParamSize      (Optional) Size of the name target.
     * @param  nameCamMatrixL  (Optional) The name camera matrix l.
     * @param  nameDistCoeffsL (Optional) The name distance coeffs l.
     * @param  nameCamMatrixR  (Optional) The name camera matrix r.
     * @param  nameDistCoeffsR (Optional) The name distance coeffs r.
     * @param  nameR           (Optional) The name r.
     * @param  nameT           (Optional) The name t.
     * @param  nameE           (Optional) The name.
     * @param  nameF           (Optional) The name f.
     * @param  nameQ           (Optional) The name q.
     * @param  nameR1          (Optional) The first name r.
     * @param  nameP1          (Optional) The first name p.
     * @param  nameR2          (Optional) The second name r.
     * @param  nameP2          (Optional) The second name p.
     */
    void loadParam(const std::string& path, const std::string& kParamSize = "paramSize",
                   const std::string& nameCamMatrixL = "M1",
                   const std::string& nameDistCoeffsL = "D1",
                   const std::string& nameCamMatrixR = "M2",
                   const std::string& nameDistCoeffsR = "D2",
                   const std::string& nameR = "R",
                   const std::string& nameT = "T",
                   const std::string& nameE = "E",
                   const std::string& nameF = "F",
                   const std::string& nameQ = "Q",
                   const std::string& nameR1 = "R1",
                   const std::string& nameP1 = "P1",
                   const std::string& nameR2 = "R2",
                   const std::string& nameP2 = "P2")
    {
        using namespace cv;
        FileStorage fs(path, FileStorage::READ); //参数
        if (fs.isOpened()) {
            if (!kParamSize.empty()) {
                fs[kParamSize] >> this->paramSize;
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
            LogE("IStereo.loadParam():打开参数文件失败! path=%s", path.c_str());
        }
        fs.release();
    }
};

} // namespace dxlib