#pragma once
#include "CameraImage.h"

namespace dxlib {

/**
 * 多线程相机采图.里面有一个vector记录相机，然后打开这些相机, 调用grab()函数对这些相机采图.
 * 
 * 
 * @author daixian
 * @date 2020/11/9
 */
class CameraGrabMT
{
  public:
    CameraGrabMT();
    CameraGrabMT(const std::vector<pCamera>& cps);
    ~CameraGrabMT();

    // 相机(其中camIndex就等于这个vector的index).
    std::vector<pCamera> vCameras;

    // 相机硬件设备
    std::vector<pCameraDevice> vDevices;

    /// <summary> 是否忽略失败的相机. </summary>
    //bool isIgnoreFailureCamera = true;

    /**
     * 设置相机,这里就保证了输入map然后改成了正确的vector的index.
     *
     * @author daixian
     * @date 2020/11/9
     *
     * @param [in] camMap The camera map.
     */
    void setCameras(const std::map<int, pCamera>& camMap);

    /**
     * 启动抓图.
     *
     * @author daixian
     * @date 2020/11/6
     */
    void startGrab();

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 进行一次抓图，依次对所有记录的相机进行采图，
    /// 输出的结果里面的vector图片的ImageItem的index应该是和相机的camIndex一致的.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <param name="result"> [out] 这是一个输出结果. </param>
    ///
    /// <returns> A pCameraImage. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool grab(pCameraImage& result);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 打开相机，如果有一个相机打开失败就会返回false，但是仍然会尝试去打开其他的相机.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <returns>
    /// 有一个相机打开失败就会返回false，全部打开成功会返回true.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool open();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭相机. </summary>
    ///
    /// <remarks> Dx, 2018/11/12. </remarks>
    ///
    /// <returns> 正常执行返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
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
    /// <summary> 清空,close的时候会自动调用. </summary>
    void clear();

    /**
     * Grab one camera
     *
     * @author daixian
     * @date 2020/7/4
     *
     * @param [in,out] result    The result.
     * @param [in,out] curCamera If non-null, the current camera.
     *
     * @returns 是否采图成功.
     */
    bool grabOneCamera(pCameraImage& result, Camera* curCamera);

    class Impl;
    Impl* _impl;
};

} // namespace dxlib
