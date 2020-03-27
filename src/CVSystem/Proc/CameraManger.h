#pragma once
#include "Camera.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 所有相机的参数，单例. </summary>
///
/// <remarks> Dx, 2018/11/8. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraManger
{
  public:
    CameraManger();
    ~CameraManger();

    static CameraManger* GetInst()
    {
        if (m_pInstance == NULL)
            m_pInstance = new CameraManger();
        return m_pInstance;
    }

    /// <summary> 所有相机的map，以camIndex为key. </summary>
    std::map<int, pCamera> camMap;

    /// <summary> 立体相机对. </summary>
    std::vector<pStereoCamera> vStereo;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 清空记录. </summary>
    ///
    /// <remarks> Dx, 2018/11/29. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void clear();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个相机，添加注意camIndex搞对. </summary>
    ///
    /// <remarks> Dx, 2018/11/29. </remarks>
    ///
    /// <param name="cp"> 要添加的相机. </param>
    ///
    /// <returns> A pCamera. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCamera add(pCamera cp);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个虚拟相机,虚拟相机参与计算逻辑. </summary>
    ///
    /// <remarks> Dx, 2020/3/27. </remarks>
    ///
    /// <param name="cp"> 要添加的相机. </param>
    ///
    /// <returns> A pCamera. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCamera addVirtual(pCamera cp);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 添加一个辅助相机,辅助相机的camIndex和非辅助相机的camIndex不能重复.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/11/16. </remarks>
    ///
    /// <param name="cp"> The cp. </param>
    ///
    /// <returns> A pCamera. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCamera addAssist(pCamera cp);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加立体相机对. </summary>
    ///
    /// <remarks> Surface, 2019/2/8. </remarks>
    ///
    /// <param name="sc"> The Screen to add. </param>
    ///-------------------------------------------------------------------------------------------------
    void add(pStereoCamera sc);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 通过camIndex得到一个相机. </summary>
    ///
    /// <remarks> Dx, 2020/3/17. </remarks>
    ///
    /// <param name="name"> The name. </param>
    ///
    /// <returns> 如果相机不存在那么返回null. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCamera getCamera(const int camIndex);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 通过相机名得到一个相机. </summary>
    ///
    /// <remarks> Dx, 2020/3/17. </remarks>
    ///
    /// <param name="name"> The name. </param>
    ///
    /// <returns> 如果相机不存在那么返回null. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCamera getCamera(const std::string& devName);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到一个立体相机. </summary>
    ///
    /// <remarks> Dx, 2018/11/29. </remarks>
    ///
    /// <returns> The stereo. </returns>
    ///-------------------------------------------------------------------------------------------------
    pStereoCamera getStereo(pCamera camera);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到一个立体相机. </summary>
    ///
    /// <remarks> Dx, 2018/11/29. </remarks>
    ///
    /// <returns> The stereo. </returns>
    ///-------------------------------------------------------------------------------------------------
    pStereoCamera getStereo(int camIndex);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置某一个相机的属性. </summary>
    ///
    /// <remarks> Dx, 2019/1/13. </remarks>
    ///
    /// <param name="camIndex"> 相机的index. </param>
    /// <param name="CAP_PROP"> The capability property. </param>
    /// <param name="value">    The value. </param>
    ///
    /// <returns> 如果存在这个相机就进行设置并且返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool setProp(int camIndex, cv::VideoCaptureProperties CAP_PROP, double value);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 是否所有的相机都已经打开. </summary>
    ///
    /// <remarks> Dx, 2019/10/25. </remarks>
    ///
    /// <returns> 如果相机全都打开了返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isAllCameraIsOpen();

  private:
    /// <summary> 单例. </summary>
    static CameraManger* m_pInstance;

    /// <summary> 初始化一个相机的Map. </summary>
    void initUndistortRectifyMap(pCamera& camera);
};

} // namespace dxlib