#pragma once

#include "opencv2/opencv.hpp"

#define XUEXUE_JSON_SUPPORT_OPENCV
#define XUEXUE_JSON_SUPPORT_EIGEN
#include "xuexuejson/Serialize.hpp"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个Mono相机的参数. </summary>
///
/// <remarks> Dx, 2020/1/13. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraParam
{
  public:
    CameraParam() {}

    CameraParam(const cv::Size& targetSize,
                const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
                const cv::Mat& projection = cv::Mat(),
                const cv::Mat& R = cv::Mat(), const cv::Mat& P = cv::Mat())
        : paramSize(targetSize),
          camMatrix(cameraMatrix), distCoeffs(distCoeffs),
          projection(projection),
          R(R), P(P)
    {
    }

    ~CameraParam() {}

    /// <summary> 相机的参数对应的分辨率size,默认和相机size相同,读参数的时候注意修改设置. </summary>
    cv::Size paramSize;

    /// <summary> 相机的内参数矩阵. </summary>
    cv::Mat camMatrix;

    /// <summary> The distance coeffs. </summary>
    cv::Mat distCoeffs;

    /// <summary> (因为一个相机可以对应多组立体相机组,所以这个属性应该去掉)相机的投影矩阵,可由立体相机的RT得到. </summary>
    cv::Mat projection;

    /// <summary> 校正畸变的参数R. </summary>
    cv::Mat R;

    /// <summary> 校正畸变的参数P. </summary>
    cv::Mat P;

    /// <summary> 相机3d空间到某世界空间的变换矩阵. </summary>
    cv::Mat camTR4x4;

    /// <summary> 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec3d camPos;

    /// <summary> 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec4d camRotate;

    /// <summary> remap函数使用的. </summary>
    cv::Mat rmap1;

    /// <summary> remap函数使用的. </summary>
    cv::Mat rmap2;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 初始化undistort rectify map. </summary>
    ///
    /// <remarks> Dx, 2020/1/13. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void initUndistortRectifyMap()
    {
        if (rmap1.empty() || rmap2.empty()) {
            cv::initUndistortRectifyMap(camMatrix, distCoeffs, R, P, paramSize, CV_16SC2, rmap1, rmap2);
        }
    }
};
/// <summary>定义这个智能指针类型. </summary>
typedef std::shared_ptr<CameraParam> pCameraParam;

} // namespace dxlib
