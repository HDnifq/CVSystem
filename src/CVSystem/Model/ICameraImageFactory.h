#pragma once

#include "Camera.h"
#include "CameraDevice.h"
#include "CameraImage.h"

namespace dxlib {

/**
 * CameraDevice的采图绑定方法.把一张原始图片处理然后分配到Camera然后生成CameraImage.
 *
 * @author daixian
 * @date 2020/11/9
 */
class ICameraImageFactory
{
  public:
    // 创建一组CameraImage,CameraDevice和Camera是一对多的关系.它实际表示了device到Camera的映射方法.
    virtual std::vector<CameraImage> Create(CameraDevice* device, std::vector<Camera*> camera) = 0;
};

} // namespace dxlib