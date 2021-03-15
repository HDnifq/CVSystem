#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "IStereo.h"

namespace dxlib {

/**
 * 一个相机对.
 *
 * @author daixian
 * @date 2020/4/19
 */
class CameraPair : public IStereo
{
  public:
    // 相机的序号.
    int cpID = -1;

  private:
};
typedef std::shared_ptr<CameraPair> pCameraPair;

} // namespace dxlib
