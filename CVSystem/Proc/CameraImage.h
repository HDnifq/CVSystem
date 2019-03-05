#pragma once

#include <opencv2/opencv.hpp>
#include "Camera.h"
#include "../dlog/dlog.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个相机一次采图的结果. </summary>
///
/// <remarks> Dx, 2018/11/27. </remarks>
///-------------------------------------------------------------------------------------------------
struct ImageItem
{
    /// <summary> 采图图片. </summary>
    cv::Mat image;

    /// <summary> 对应相机. </summary>
    Camera* camera = nullptr;

    /// <summary> 是否采图成功. </summary>
    bool isSuccess = false;

    /// <summary> 这一帧的采集开始时间戳. </summary>
    clock_t grabStartTime = 0;

    /// <summary> 这一帧的采集结束时间戳. </summary>
    clock_t grabEndTime = 0;

    /// <summary> 采图的消耗时间. </summary>
    float costTime()
    {
        return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
    }
};

///-------------------------------------------------------------------------------------------------
/// <summary> 若干个相机一次的采图结果. </summary>
///
/// <remarks> Dx, 2018/11/14. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraImage
{
  public:
    /// <summary> 相机帧序号. </summary>
    int fnum = 0;

    /// <summary> 这一帧里这一组相机的原始图像(但是index并不是camIndex). </summary>
    std::vector<ImageItem> vImage;

    /// <summary> 这一帧的采集开始时间戳. </summary>
    clock_t grabStartTime = 0;

    /// <summary> 这一帧的采集结束时间戳. </summary>
    clock_t grabEndTime = 0;

    /// <summary> 这一帧的开始处理的时间戳. </summary>
    clock_t procStartTime = 0;

    /// <summary> 这一帧的结束处理的时间戳. </summary>
    clock_t procEndTime = 0;

    /// <summary> 这一帧的采集消耗时间. </summary>
    float grabCostTime()
    {
        return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
    }

    /// <summary> 等待处理的消耗时间 ms. </summary>
    float waitProcTime()
    {
        return (float)(procStartTime - grabEndTime) / CLOCKS_PER_SEC * 1000;
    }

    /// <summary> 处理消耗的时间 ms. </summary>
    float procCostTime()
    {
        return (float)(procEndTime - procStartTime) / CLOCKS_PER_SEC * 1000;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// vImage并不是一个以camIndex为序号的数据结构，所以用这个函数来查找一个ImageItem.
    /// </summary>
    ///
    /// <remarks> Dx, 2018/12/19. </remarks>
    ///
    /// <param name="camIndex"> camIndex. </param>
    ///
    /// <returns> 如果没有找到那么就返回null. </returns>
    ///-------------------------------------------------------------------------------------------------
    const ImageItem* getItem(int camIndex) const
    {
        //如果vector的index是能够对应上的，那么就直接返回这一项
        if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
            return &vImage[camIndex];
        }

        //如果对应不上那么只能遍历搜索
        for (size_t i = 0; i < vImage.size(); i++) {
            if (vImage[i].camera->camIndex == camIndex) {
                return &vImage[i];
            }
        }
        LogE("CameraImage.getItem():输入的camIndex %d 未能查找到!", camIndex);
        return nullptr;
    }
};
/// <summary>定义这个智能指针类型. </summary>
typedef std::shared_ptr<CameraImage> pCameraImage;

} // namespace dxlib