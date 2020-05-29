#pragma once

#include <opencv2/opencv.hpp>
#include "dlog/dlog.h"

#include "../Model/Camera.h"
#include "../Model/StereoCamera.h"
#include "../Model/CameraPair.h"

namespace dxlib {

/**
 * 一个相机一次采图的结果.
 *
 * @author daixian
 * @date 2018/11/27
 */
struct ImageItem
{
    // 采图图片.
    cv::Mat image;

    // 对应相机（如果这里为null，那么基本上应该认为这个item是无效的）.
    Camera* camera = nullptr;

    // 是否采图成功.
    bool isSuccess = false;

    // 这一帧的采集开始时间戳.
    clock_t grabStartTime = 0;

    // 这一帧的采集结束时间戳.
    clock_t grabEndTime = 0;

    /**
     * 采图的消耗时间(毫秒).
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns 消耗时间(毫秒).
     */
    float costTime()
    {
        return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
    }
};

/**
 * 若干个相机一次的采图结果.
 *
 * @author daixian
 * @date 2018/11/14
 */
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

    // 相机帧序号.
    unsigned int fnum = 0;

    // 这一帧里这一组相机的原始图像(其中强制实现了index就是camIndex,如果记录).
    std::vector<ImageItem> vImage;

    // 立体相机信息. key是scId,后面是LR两个相机指针
    std::map<int, std::array<Camera*, 2>> stereoInfo;

    // 这一帧的采集开始时间戳.
    clock_t grabStartTime = 0;

    // 这一帧的采集结束时间戳.
    clock_t grabEndTime = 0;

    // 这一帧的开始处理的时间戳.
    clock_t procStartTime = 0;

    // 这一帧的结束处理的时间戳.
    clock_t procEndTime = 0;

    /**
     * 这一帧的采集消耗时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float grabCostTime()
    {
        return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
    }

    /**
     * 等待处理的消耗时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float waitProcTime()
    {
        return (float)(procStartTime - grabEndTime) / CLOCKS_PER_SEC * 1000;
    }

    /**
     * 处理消耗的时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float procCostTime()
    {
        return (float)(procEndTime - procStartTime) / CLOCKS_PER_SEC * 1000;
    }

    /**
     * vImage并不是一个以camIndex为序号的数据结构，所以用这个函数来查找一个ImageItem.
     *
     * @author daixian
     * @date 2018/12/19
     *
     * @param  camIndex camIndex.
     *
     * @returns 如果没有找到那么就返回null.
     */
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

    /**
     * 使用camIndex搜索一个ImageItem.
     *
     * @author daixian
     * @date 2019/8/5
     *
     * @param          camIndex 相机的index.
     * @param [in,out] item     结果.
     *
     * @returns 如果成功返回true.
     */
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

    /**
     * 第一张有内容的图片的大小,用来简单的获得当前工作图片大小.
     *
     * @author daixian
     * @date 2020/4/16
     *
     * @returns A cv::Size.
     */
    cv::Size firstImageSize()
    {
        for (int i = 0; i < vImage.size(); i++) {
            if (vImage[i].isSuccess && !vImage[i].image.empty()) {
                return cv::Size(vImage[i].image.cols, vImage[i].image.rows);
            }
        }
        return cv::Size(0, 0);
    }
};
/** 定义这个智能指针类型. */
typedef std::shared_ptr<CameraImage> pCameraImage;

} // namespace dxlib