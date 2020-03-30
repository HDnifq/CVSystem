#pragma once

#include <opencv2/opencv.hpp>
#include "Camera.h"
#include "dlog/dlog.h"

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

    /// <summary> 对应相机（如果这里为null，那么基本上应该认为这个item是无效的）. </summary>
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
    CameraImage(std::vector<pCamera>& vCameras)
    {
        vImage.resize(vCameras.size()); //先直接创建算了
        //记录所有的camera
        for (size_t i = 0; i < vCameras.size(); i++) {
            vImage[i].camera = vCameras[i].get();
        }
    }

    /// <summary> 相机帧序号. </summary>
    unsigned int fnum = 0;

    /// <summary> 这一帧里这一组相机的原始图像(其中强制实现了index就是camIndex,如果记录). </summary>
    std::vector<ImageItem> vImage;

    /// <summary> 立体相机信息. key是scId,后面是LR两个相机指针 </summary>
    std::map<int, std::array<Camera*, 2>> stereoInfo;

    /// <summary> 这一帧的采集开始时间戳. </summary>
    clock_t grabStartTime = 0;

    /// <summary> 这一帧的采集结束时间戳. </summary>
    clock_t grabEndTime = 0;

    /// <summary> 这一帧的开始处理的时间戳. </summary>
    clock_t procStartTime = 0;

    /// <summary> 这一帧的结束处理的时间戳. </summary>
    clock_t procEndTime = 0;

    /// <summary> 这一帧的采集消耗时间 ms. </summary>
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
    ImageItem getItem(int camIndex) const
    {
        //如果vector的index是能够对应上的，那么就直接返回这一项
        if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
            return vImage[camIndex];
        }
        //如果对应不上那么只能遍历搜索
        for (int i = 0; i < vImage.size(); i++) {
            if (vImage[i].camera->camIndex == camIndex) {
                return vImage[i];
            }
        }
        LogE("CameraImage.getItem():输入的camIndex未能查找到!camIndex=%d", camIndex);
        return ImageItem();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Gets an item. </summary>
    ///
    /// <remarks> Dx, 2019/8/5. </remarks>
    ///
    /// <param name="camIndex"> Zero-based index of the
    ///                         camera. </param>
    /// <param name="item">     [in,out] The item. </param>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool getItem(int camIndex, ImageItem& item) const
    {
        //如果vector的index是能够对应上的，那么就直接返回这一项
        if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
            item = vImage[camIndex];
            return true;
        }

        //如果对应不上那么只能遍历搜索
        for (int i = 0; i < vImage.size(); i++) {
            if (vImage[i].camera->camIndex == camIndex) {
                item = vImage[camIndex];
                return true;
            }
        }
        LogE("CameraImage.getItem():输入的camIndex未能查找到!camIndex=%d", camIndex);
        return false;
    }
};
/// <summary>定义这个智能指针类型. </summary>
typedef std::shared_ptr<CameraImage> pCameraImage;

} // namespace dxlib