﻿#pragma once
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

    /// <summary> 辅助的相机的map如同帧双目相机,通常不直接参与计算. </summary>
    std::map<int, pCamera> camMapAssist;

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
    /// <param name="cp">              要添加的相机. </param>
    /// <param name="isVirtualCamera"> (Optional) 是否这是一个虚拟相机. </param>
    ///-------------------------------------------------------------------------------------------------
    pCamera add(pCamera cp, bool isVirtualCamera = false);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个辅助相机. </summary>
    ///
    /// <remarks> Dx, 2019/11/16. </remarks>
    ///
    /// <param name="cp"> The cp. </param>
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
    bool setProp(int camIndex, int CAP_PROP, double value);

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