#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"

namespace dxlib {

/**
 * 一个相机对.
 *
 * @author daixian
 * @date 2020/4/19
 */
class CameraPair
{
  public:
    // 相机的序号.
    int cpID = -1;

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
     * 根据一个相机文件载入尝试相机参数信息.
     *
     * @author daixian
     * @date 2020/5/3
     *
     * @param  path            参数文件的完整路径.
     * @param  paramSize       (Optional) Size of the name target.
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
                   const std::string& nameP2 = "P2");

  private:
};
typedef std::shared_ptr<CameraPair> pCameraPair;

} // namespace dxlib
