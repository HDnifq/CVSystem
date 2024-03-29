﻿#pragma once

#include "Camera.h"
#include "CameraDevice.h"
#include "CameraImage.h"

namespace dxlib {

/**
 * CameraDevice的采图绑定方法.
 * 如立体相机中包含把一张原始相机图片处理然后分配到左右两个Camera然后生成CameraImage.
 *
 * @author daixian
 * @date 2020/11/9
 */
class ICameraImageFactory
{
  public:
    // 析构函数
    virtual ~ICameraImageFactory(){};

    // ImageFactory的id.它会等于device的id.
    int id = -1;

    // 工厂使用的相机设备,device中含有cv::VideoCapture成员.
    pCameraDevice device;

    // 是否硬件设备已经错误
    bool isDeviceError = false;

    // 工厂对应的逻辑相机(一个或者一对)
    std::vector<pCamera> cameras;

    /**
     * 这个CameraImageFactory的类型
     *
     * @author daixian
     * @date 2020/12/7
     *
     * @returns 它的实际类名吧.
     */
    virtual std::string Type() = 0;

    /**
     * 创建一组CameraImage,CameraDevice和Camera是一对多的关系.它实际表示了device到Camera的图片映射方法.
     * 目前这个函数实际上包含了阻塞的读取相机图片.
     *
     * @author daixian
     * @date 2020/11/12
     *
     * @returns 一个std::vector<CameraImage>数组,是采图的输出结果,它的数量和cameras的数量相同.
     */
    virtual std::vector<CameraImage> Create() = 0;
};
// 定义这个智能指针类型.
typedef std::shared_ptr<ICameraImageFactory> pCameraImageFactory;

} // namespace dxlib
