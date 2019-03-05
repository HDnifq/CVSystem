#pragma once
#include "CameraImage.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个相机的采图方法. </summary>
///
/// <remarks> Dx, 2019/3/5. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraGrab
{
  public:
    CameraGrab();
    CameraGrab(const std::vector<pCamera>& cps);
    ~CameraGrab();

    /// <summary> 相机参数. </summary>
    std::vector<pCamera> vCameras;

    /// <summary> 已采集的帧数. </summary>
    long fnumber = 0;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置相机. </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <param name="camMap"> [in,out] The camera map. </param>
    ///-------------------------------------------------------------------------------------------------
    void setCameras(const std::map<int, pCamera>& camMap);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 进行一次抓图. </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <returns> A pCameraImage. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool grab(pCameraImage& result);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开相机. </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
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

  private:
    /// <summary> 统计FPS需要的变量. </summary>
    clock_t _lastTime = 0;

    /// <summary> 统计FPS需要的变量. </summary>
    long _lastfnumber = 0;

    /// <summary> 统计一下当前的FPS. </summary>
    void updateFPS();

    /// <summary> 清空,close的时候会自动调用. </summary>
    void clear();
};

} // namespace dxlib
