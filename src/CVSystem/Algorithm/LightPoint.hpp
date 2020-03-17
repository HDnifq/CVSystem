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

        /// <summary> 轮廓的最大大小. </summary>
        int contMaxSize = 20;

        /// <summary> 轮廓的最小大小. </summary>
        int contMinSize = 4;
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

        /// <summary> 高亮的轮廓结果. </summary>
        std::vector<std::vector<cv::Point>> contours;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void detection(const cv::Mat& image, const LightPoint::Param& param, LightPoint::Result& result)
    {
        cv::Mat grayImage;
        cv::Mat thrImage;
        std::vector<std::vector<cv::Point>>& contours = result.contours;
        //转换一次黑白图像
        cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        //门限结果输出到thrImage
        cv::threshold(grayImage, thrImage, param.highLightThr, 255, cv::THRESH_BINARY);

        cv::findContours(thrImage,
                         contours,               //轮廓的结果数组
                         cv::RETR_EXTERNAL,      //获取外轮廓
                         cv::CHAIN_APPROX_NONE); //获取每个轮廓的每个像素

        for (size_t i = 0; i < contours.size(); i++) {
            //根据大小筛除轮廓
            if (contours[i].size() < param.contMinSize && contours[i].size() > param.contMaxSize) {
                continue;
            }
        }
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
