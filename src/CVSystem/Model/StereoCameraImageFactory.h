#pragma once
#include "ICameraImageFactory.h"
#include "dlog/dlog.h"

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
    /**
     * 构造,现在规定一个物理相机只可以对应一个立体相机对.
     *
     * @author daixian
     * @date 2020/12/7
     *
     * @param  device       The device.
     * @param  stereoCamera The stereo camera.
     */
    StereoCameraImageFactory(const pCameraDevice& device, const std::array<pCamera, 2>& stereoCamera);

    /**
     * Destructor
     *
     * @author daixian
     * @date 2020/12/7
     */
    virtual ~StereoCameraImageFactory()
    {
    }

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
        return "StereoCameraImageFactory";
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
    // 立体相机对
    std::array<pCamera, 2> stereoCamera;
};
} // namespace dxlib