#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于单相机的一般CameraImage图片构造,实际上包含了阻塞的读取相机图片.
 *
 * @author daixian
 * @date 2020/11/12
 */
class MonoCameraImageFactory : ICameraImageFactory
{
  public:
    MonoCameraImageFactory()
    {
    }

    virtual ~MonoCameraImageFactory()
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