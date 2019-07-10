#pragma once

#define _USE_MATH_DEFINES //定义了之后才有M_PI的宏定义
#include <math.h>
#include <opencv2/opencv.hpp>
//#include <Eigen/Geometry>
//#include <unsupported/Eigen/EulerAngles>
//#include <opencv2/core/eigen.hpp>

namespace dxlib {

class LightPoint
{
  public:
    LightPoint();
    ~LightPoint();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void detection(cv::Mat image)
    {


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
