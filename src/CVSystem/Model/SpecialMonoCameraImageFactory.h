#pragma once
#include "ICameraImageFactory.h"

namespace dxlib {

/**
 * 用于单相机的一般CameraImage图片构造,实际上包含了阻塞的读取相机图片.
 *
 * @author daixian
 * @date 2020/11/12
 */
class SpecialMonoCameraImageFactory : public ICameraImageFactory
{
  public:
    SpecialMonoCameraImageFactory(const pCameraDevice& device, const std::vector<pCamera>& cameras);
    SpecialMonoCameraImageFactory(const pCameraDevice& device, const pCamera& cameras);

    virtual ~SpecialMonoCameraImageFactory()
    {
    }

    enum class Mode
    {
        HalfLeft, //左半边
        HalfRight //右半边
    };

    // 工作模式
    Mode mode = Mode::HalfLeft;

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
        return "SpecialMonoCameraImageFactory";
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