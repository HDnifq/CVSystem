#pragma once

#include "opencv2/opencv.hpp"

//#define XUEXUE_JSON_SUPPORT_OPENCV
//#define XUEXUE_JSON_SUPPORT_EIGEN
//#include "xuexuejson/JsonMapper.hpp"
#include "CameraParam.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个立体相机的参数. </summary>
///
/// <remarks> Dx, 2020/1/13. </remarks>
///-------------------------------------------------------------------------------------------------
class StereoCameraParam
{
  public:
    StereoCameraParam() {}

    ~StereoCameraParam() {}

    /// <summary> 参数的分辨率. </summary>
    cv::Size paramSize;

    /// <summary> L相机的参数. </summary>
    pCameraParam camParamL;

    /// <summary> R相机的参数. </summary>
    pCameraParam camParamR;

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
};
/// <summary>定义这个智能指针类型. </summary>
typedef std::shared_ptr<StereoCameraParam> pStereoCameraParam;

} // namespace dxlib
