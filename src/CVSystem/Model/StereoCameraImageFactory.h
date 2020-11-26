#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于立体相机的一般CameraImage图片构造,实际上包含了阻塞的读取相机图片.
 *
 * @author daixian
 * @date 2020/11/12
 */
class StereoCameraImageFactory : public ICameraImageFactory
{
  public:
    StereoCameraImageFactory(const pCameraDevice& device, const std::vector<pCamera>& cameras)
    {
        //给基类成员赋值
        this->device = device;
        this->cameras = cameras;
    }

    virtual ~StereoCameraImageFactory()
    {
    }

    /**
     * 这个函数实际上包含了阻塞的读取相机图片.
     *
     * @author daixian
     * @date 2020/11/16
     *
     * @returns A std::vector<CameraImage>
     */
    virtual std::vector<CameraImage> Create();

  private:
};
} // namespace dxlib