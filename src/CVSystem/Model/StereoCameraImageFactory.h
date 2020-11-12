#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于立体相机的一般CameraImage图片构造.
 *
 * @author daixian
 * @date 2020/11/12
 */
class StereoCameraImageFactory : ICameraImageFactory
{
  public:
    StereoCameraImageFactory()
    {
    }

    ~StereoCameraImageFactory()
    {
    }

    virtual std::vector<CameraImage> Create();

  private:
};
} // namespace dxlib