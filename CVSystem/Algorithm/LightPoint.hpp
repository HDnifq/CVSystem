#pragma once

#define _USE_MATH_DEFINES //定义了之后才有M_PI的宏定义
#include <math.h>
#include <opencv2/opencv.hpp>
//#include <Eigen/Geometry>
//#include <unsupported/Eigen/EulerAngles>
//#include <opencv2/core/eigen.hpp>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 处理提炼亮点. </summary>
///
/// <remarks> Dx, 2019/7/11. </remarks>
///-------------------------------------------------------------------------------------------------
class LightPoint
{
  public:
    LightPoint();
    ~LightPoint();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 控制参数. </summary>
    ///
    /// <remarks> Dx, 2019/7/11. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Param
    {
      public:
        /// <summary> 高亮门限. </summary>
        int highLightThr = 200;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测结果. </summary>
    ///
    /// <remarks> Dx, 2019/7/11. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Result
    {
      public:
        cv::Mat grayImage;
        cv::Mat thrImage;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void detection(const cv::Mat& image, const LightPoint::Param& param, LightPoint::Result& result)
    {
        //转换一次黑白图像
        cvtColor(image, result.grayImage, CV_BGR2GRAY);

        //门限结果输出到thr(50)
        cv::threshold(result.grayImage, result.thrImage, param.highLightThr, 255, cv::THRESH_BINARY); //70
    }

  private:
};

LightPoint::LightPoint()
{
}

LightPoint::~LightPoint()
{
}
} // namespace dxlib
