#pragma once

#include "CameraImage.h"
#include "StereoCamera.h"
#include "CameraPair.h"

namespace dxlib {

/**
 * 若干个相机一次的采图结果.这个对象一般作为传递给下一个处理单元的接口.
 *
 * @author daixian
 * @date 2018/11/14
 */
class CameraImageGroup
{
  public:
    //CameraImageGroup(std::vector<pCamera>& vCameras)
    //{
    //    vImage.resize(vCameras.size()); //先直接创建算了
    //    //记录所有的camera
    //    for (size_t i = 0; i < vCameras.size(); i++) {
    //        vImage[i].camera = vCameras[i].get();
    //    }
    //}
    CameraImageGroup() {}

    // 相机帧序号.
    unsigned int fnum = 0;

    // 这一帧里这一组相机的原始图像(其中强制实现了index就是camIndex,如果记录).
    std::vector<CameraImage> vImage;

    // 立体相机采图的图片
    std::vector<std::array<CameraImage, 2>> vStereoImage;

    // 这一帧的最早采集开始时间戳.
    clock_t grabStartTime = 0;

    // 这一帧的最后采集结束时间戳.
    clock_t grabEndTime = 0;

    // 这一帧的开始处理的时间戳.
    clock_t procStartTime = 0;

    // 这一帧的结束处理的时间戳.
    clock_t procEndTime = 0;

    // 帧标记信息(相当于忽略帧的细分功能)
    std::vector<std::string> frameFlag;

    /**
     * 向数据结构中添加一项相机图片的记录.
     *
     * @author daixian
     * @date 2020/11/8
     *
     * @param  camImage The camera image.
     */
    void addCameraImage(const CameraImage& camImage);

    /**
     * 这一帧的采集消耗时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float grabCostTime();

    /**
     * 等待处理的消耗时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float waitProcTime();

    /**
     * 处理消耗的时间 ms.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns A float.
     */
    float procCostTime();

    /**
     * vImage并不一定是一个以camIndex为序号的数据结构，所以用这个函数来查找一个ImageItem.
     *
     * @author daixian
     * @date 2018/12/19
     *
     * @param  camIndex camIndex.
     *
     * @returns 如果没有找到那么就返回null.
     */
    const CameraImage& getImage(int camIndex) const;

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

    /**
     * 是否所有的相机都关闭了.用户在proc处理里进行判断,如果所有相机都关闭了那么可以自行睡眠.
     *
     * @author daixian
     * @date 2020/7/4
     *
     * @returns 如果所有的相机都关闭了就返回true.
     */
    bool isAllCameraClosed()
    {
        //for (int i = 0; i < vImage.size(); i++) {
        //    if (vImage[i].camera->isOpened()) {
        //        return false;
        //    }
        //}
        return false;
    }

    /**
     * 是否包含这个帧flag.
     *
     * @author daixian
     * @date 2021/7/22
     *
     * @param  flag 要比较的帧标记字符串.
     *
     * @returns 如果包含则返回true.
     */
    bool hasFrameFlag(const std::string& flag)
    {
        for (size_t i = 0; i < frameFlag.size(); i++) {
            if (frameFlag[i] == flag) {
                return true;
            }
        }
    }
};

// 定义这个智能指针类型.
typedef std::shared_ptr<CameraImageGroup> pCameraImageGroup;

} // namespace dxlib
