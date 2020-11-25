#pragma once
#include "../Model/CameraImageGroup.h"
#include "../Model/ICameraImageFactory.h"

namespace dxlib {

/**
 * 多线程相机采图.里面有一个vector记录相机，然后打开这些相机, 调用grab()函数对这些相机采图.
 * TODO: 实际可以写CameraImageFactory模拟对象来做单元测试.
 * 
 * 
 * @author daixian
 * @date 2020/11/9
 */
class CameraGrabMT
{
  public:
    CameraGrabMT();
    CameraGrabMT(const std::vector<pCamera>& vcameras,
                 const std::vector<pCameraDevice>& vdevices,
                 const std::vector<pCameraImageFactory>& vcameraImageFactory);

    ~CameraGrabMT();

    // 相机(其中camIndex就等于这个vector的index).
    std::vector<pCamera> vCameras;

    // 相机硬件设备
    std::vector<pCameraDevice> vDevices;

    // 相机图片采集器
    std::vector<pCameraImageFactory> vCameraImageFactory;

    /**
     * 异步的启动抓图.
     *
     * @author daixian
     * @date 2020/11/6
     */
    void startGrab();

    /**
     * 由外面的线程对图片队列进行一次抓图，依次对所有记录的相机进行采图，输出的结果里面的vector图片的ImageItem的index应该是和相机的camIndex一致的.
     *
     * @author daixian
     * @date 2020/11/26
     *
     * @param [out] result 这是一个输出结果.
     *
     * @returns 如果为true表示抓图完成.
     */
    bool tryGet(pCameraImageGroup& result);

    /**
     * 打开相机，如果有一个相机打开失败就会返回false，但是仍然会尝试去打开其他的相机.
     *
     * @author daixian
     * @date 2020/11/26
     *
     * @returns 有一个相机打开失败就会返回false，全部打开成功会返回true.
     */
    bool open();

    /**
     * 关闭相机.
     *
     * @author daixian
     * @date 2020/11/26
     *
     * @returns 正常执行返回true.
     */
    bool close();

    /**
     * 是否所有的相机都关闭了
     *
     * @author daixian
     * @date 2020/7/4
     *
     * @returns 如果所有的相机都关闭了就返回true.
     */
    bool isAllCameraClosed();

  private:
    /**
     * 清空,close的时候会自动调用.
     *
     * @author daixian
     * @date 2020/11/16
     */
    void clear();

    class Impl;
    Impl* _impl;
};

} // namespace dxlib
