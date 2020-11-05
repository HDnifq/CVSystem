#pragma once
#include "CameraImage.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 一个相机的采图方法，里面有一个vector记录相机，然后打开这些相机,
/// 调用grab()函数对这些相机采图.
/// </summary>
///
/// <remarks> Dx, 2019/3/5. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraGrab
{
  public:
    CameraGrab();
    CameraGrab(const std::vector<pCamera>& cps);
    ~CameraGrab();

    /// <summary> 相机参数(其中camIndex就等于这个vector的index). </summary>
    std::vector<pCamera> vCameras;

    /// <summary> 是否忽略失败的相机. </summary>
    //bool isIgnoreFailureCamera = true;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置相机,这里就保证了输入map然后改成了正确的vector的index. </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <param name="camMap"> [in] The camera map. </param>
    ///-------------------------------------------------------------------------------------------------
    void setCameras(const std::map<int, pCamera>& camMap);

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
