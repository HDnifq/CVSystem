#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于单相机的一般CameraImage图片构造.
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

    ~MonoCameraImageFactory()
    {
    }

    virtual std::vector<CameraImage> Create();

  private:
};
} // namespace dxlib