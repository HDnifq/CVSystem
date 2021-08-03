#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于单相机的一般CameraImage图片构造,实际上包含了阻塞的读取相机图片.
 *
 * @author daixian
 * @date 2020/11/12
 */
class MonoCameraImageFactory : public ICameraImageFactory
{
  public:
    MonoCameraImageFactory(const pCameraDevice& device, const std::vector<pCamera>& cameras);
    MonoCameraImageFactory(const pCameraDevice& device, const pCamera& cameras);

    virtual ~MonoCameraImageFactory()
    {
    }

    // 是否忽略硬件的失败
    //bool isIgnoreDeviceFailure = true;

    /**
     * 类型字符串.
     *
     * @author daixian
     * @date 2020/12/7
     *
     * @returns A std::string.
     */
    virtual std::string Type()
    {
        return "MonoCameraImageFactory";
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
