#pragma once

#define XUEXUE_JSON_SUPPORT_OPENCV
#define XUEXUE_JSON_SUPPORT_EIGEN
#include "xuexuejson/Serialize.hpp"

#include "../../Moudel/Camera.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 代表一个相机的Dto.
/// </summary>
///
/// <remarks> Dx, 2020/4/30. </remarks>
///-------------------------------------------------------------------------------------------------
class CameraDto : XUEXUE_JSON_OBJECT
{
  public:
    CameraDto() {}
    ~CameraDto() {}

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 根据一个相机数据来设置值. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="cam"> The camera. </param>
    ///-------------------------------------------------------------------------------------------------
    CameraDto(pCamera cam)
    {
        camIndex = cam->camIndex;
        devName = cam->devName;
        isVirtualCamera = cam->isVirtualCamera;
        physicalDevName = cam->physicalDevName;

        devID = cam->devID;
        size = cam->size;
        paramSize = cam->size;
        isStereoCamera = cam->isStereoCamera;
        isNoSendToProc = cam->isNoSendToProc;
        scID = cam->scID;
        stereoCamIndexL = cam->stereoCamIndexL;
        stereoCamIndexR = cam->stereoCamIndexR;
    }

    /// <summary> 相机的逻辑编号0-3，它代表一个编程逻辑上的编号，不是相机的设备index. </summary>
    int camIndex;

    /// <summary> 这个相机的设备名. </summary>
    std::string devName;

    /// <summary> 虚拟相机. </summary>
    bool isVirtualCamera;

    /// <summary>
    /// 如果它是一个虚拟相机，那么它可能是一个立体相机的一半，这里则记录它的实际物理相机的名字.
    /// </summary>
    std::string physicalDevName;

    /// <summary> (这个是用户只读的)设备的id顺序，物理硬件上的index. </summary>
    int devID;

    /// <summary> 相机的分辨率size，打开相机之后会按照这个设置来尝试设置相机分辨率. </summary>
    cv::Size size;

    /// <summary> 相机的参数对应的分辨率size,默认和相机size相同,读参数的时候注意修改设置. </summary>
    cv::Size paramSize;

    /// <summary> 是否是立体相机. </summary>
    bool isStereoCamera;

    /// <summary> 是否不要传图到后面给Proc,如果设置它为true,那么需要把这个相机的index放到后面. </summary>
    bool isNoSendToProc;

    /// <summary> 立体相机对的序号(区分多组立体相机). </summary>
    int scID;

    /// <summary> 双目相机里的L相机的camIndex. </summary>
    int stereoCamIndexL;

    /// <summary> 双目相机里的R相机的camIndex. </summary>
    int stereoCamIndexR;

    XUEXUE_JSON_OBJECT_M12(camIndex, devName, isVirtualCamera, physicalDevName, devID, size, paramSize,
                           isStereoCamera, isNoSendToProc, scID, stereoCamIndexL, stereoCamIndexR)
  private:
};

} // namespace dxlib
