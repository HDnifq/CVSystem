#pragma once

#include <opencv2/opencv.hpp>

#include "Camera.h"
#include "CameraDevice.h"

namespace dxlib {

/**
 * 一个相机一次采图的结果.
 *
 * @author daixian
 * @date 2018/11/27
 */
struct CameraImage
{
    // 采图图片.
    cv::Mat image;

    // 对应相机（如果这里为null，那么基本上应该认为这个item是无效的）.
    Camera* camera = nullptr;

    // 实际的采图设备的指针.
    CameraDevice* device = nullptr;

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
    float costTime();
};
typedef std::shared_ptr<CameraImage> pCameraImage;

} // namespace dxlib