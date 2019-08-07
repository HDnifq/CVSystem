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
    bool isIgnoreFailureCamera{true};

    /// <summary> 已采集的帧数. </summary>
    long fnumber = 0;

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

#pragma region 拆解的单个相机的采图(为了实现多线程)

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 采图拆解的3步(第一步开始采图,构造结果对象). </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="result"> [out] The result. </param>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool startGrabImage(pCameraImage& result);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 采图拆解的3步(对一个相机抓图). </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="result">   [out] The result. </param>
    /// <param name="camIndex"> 相机的index. </param>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool grabWithCamIndex(pCameraImage& result, int camIndex);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 采图拆解的3步(结束这次抓图). </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="result"> [out] The result. </param>
    ///
    /// <returns>
    /// True if it succeeds, false if it fails.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool endGrabImage(pCameraImage& result);

#pragma endregion

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开相机. </summary>
    ///
    /// <remarks> Dx, 2019/3/5. </remarks>
    ///
    /// <param name="isIgnoreFailure"> (Optional) 是否忽略失败的相机. </param>
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
