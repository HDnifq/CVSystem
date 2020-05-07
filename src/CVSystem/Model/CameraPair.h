#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个相机对. </summary>
///
/// <remarks> Surface, 2020/4/19. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraPair
{
  public:
    /// <summary> 相机的序号. </summary>
    int cpID = -1;

    /// <summary> 这个立体相机组的名字. </summary>
    std::string name;

    /// <summary> The camera l. </summary>
    pCamera camL;

    /// <summary> The camera r. </summary>
    pCamera camR;

    /// <summary> 参数R. </summary>
    cv::Mat R;

    /// <summary> 参数T. </summary>
    cv::Mat T;

    /// <summary> 参数E. </summary>
    cv::Mat E;

    /// <summary> 参数F. </summary>
    cv::Mat F;

    /// <summary> 参数Q. </summary>
    cv::Mat Q;

    /// <summary> 左相机的投影矩阵. </summary>
    cv::Mat LP;

    /// <summary> 右相机的投影矩阵. </summary>
    cv::Mat RP;

    /// <summary> 这组相机3d空间到某世界空间的变换矩阵,它应该等于camL里的的相机camTR4x4. </summary>
    cv::Mat camTR4x4;

    /// <summary> 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec3d camPos;

    /// <summary> 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec4d camRotate;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 根据一个相机文件载入尝试相机参数信息. </summary>
    ///
    /// <remarks> Surface, 2020/5/3. </remarks>
    ///
    /// <param name="path"> 参数文件的完整路径. </param>
    ///-------------------------------------------------------------------------------------------------
    void loadParam(const std::string& path, const std::string& nameTargetSize = "targetSize",
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
