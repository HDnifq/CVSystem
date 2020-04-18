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

    /// <summary> 所有相机的map，以camIndex为key.它实际已经等价于vCamera </summary>
    std::map<int, pCamera> camMap;

    /// <summary> 立体相机对. </summary>
    std::vector<pStereoCamera> vStereo;

    /// <summary> 所有相机对的数据. </summary>
    std::vector<pCameraPair> vCameraPair;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 清空记录. </summary>
    ///
    /// <remarks> Dx, 2018/11/29. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void clear();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个相机，会自动递增分配一个camIndex. </summary>
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
    /// <summary> 添加立体相机对. </summary>
    ///
    /// <remarks> Surface, 2019/2/8. </remarks>
    ///
    /// <param name="sc"> The Screen to add. </param>
    ///-------------------------------------------------------------------------------------------------
    pStereoCamera add(pStereoCamera sc);

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
    /// <summary> 得到当前所有相机的vector数组. </summary>
    ///
    /// <remarks> Surface, 2020/4/18. </remarks>
    ///
    /// <returns> The vector camera. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::vector<pCamera>& getCameraVec();

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
    /// <summary>
    /// 得到一个相机对,因为这个相机对可能有随意组合的情况,所以需要两个参数.
    /// </summary>
    ///
    /// <remarks> Surface, 2020/4/19. </remarks>
    ///
    /// <param name="cameraL"> The camera l. </param>
    /// <param name="cameraR"> The camera r. </param>
    ///
    /// <returns> The camera pair. </returns>
    ///-------------------------------------------------------------------------------------------------
    pCameraPair getCameraPair(pCamera cameraL, pCamera cameraR);

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

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测相机的录入. </summary>
    ///
    /// <remarks> Dx, 2020/3/30. </remarks>
    ///
    /// <returns> 检察相机的录入是否有问题. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool checkInputData();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Loads a JSON. </summary>
    ///
    /// <remarks> Surface, 2020/4/19. </remarks>
    ///
    /// <param name="path"> Full pathname of the file. </param>
    ///-------------------------------------------------------------------------------------------------
    void loadJson(std::string path);

  private:
    /// <summary> 单例. </summary>
    static CameraManger* m_pInstance;

    /// <summary> 初始化一个相机的Map. </summary>
    void initUndistortRectifyMap(pCamera& camera);

    /// <summary> 名字和相机指针的map. </summary>
    std::map<std::string, pCamera> mNamePCamera;

    /// <summary> 相机index的数组. </summary>
    std::vector<pCamera> vCamera;
};

} // namespace dxlib