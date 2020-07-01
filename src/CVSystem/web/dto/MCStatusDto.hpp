#pragma once

#include "CameraDto.hpp"
#include "../../System/MultiCamera.h"
#include "../../System/CameraManger.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 代表一个相机的Dto.
/// </summary>
///
/// <remarks> Dx, 2020/4/30. </remarks>
///-------------------------------------------------------------------------------------------------
class MCStatusDto : XUEXUE_JSON_OBJECT
{
  public:
    MCStatusDto() {}
    ~MCStatusDto() {}

    MCStatusDto(MultiCamera* mc, CameraManger* cm)
    {
        isCameraOpened = mc->isCameraOpened();
        isGrab = mc->isGrab();
        isRunning = mc->isRunning();
        activeProcName = mc->activeProcName();

        frameCount = mc->frameCount();
        fps = mc->fps();

        auto mcProcs = mc->getAllProc();
        for (size_t i = 0; i < mcProcs.size(); i++) {
            procs.push_back(mcProcs[i]->name());
        }
        //记录所有相机
        for (auto& kvp : cm->camMap) {
            CameraDto cdto(kvp.second);
            cameras.push_back(cdto);
        }
    }

    /// <summary> 是否相机已经打开. </summary>
    bool isCameraOpened = false;

    /// <summary> 是否采图. </summary>
    bool isGrab = false;

    /// <summary> 是否当前计算线程正在工作. </summary>
    bool isRunning = false;

    /// <summary> 当前执行的proc的名字,如果没有proc就返回null. </summary>
    std::string activeProcName;

    /// <summary> 所有录入的proc. </summary>
    std::vector<std::string> procs;

    /// <summary> 当前帧数. </summary>
    uint frameCount = 0;

    /// <summary> 当前的fps. </summary>
    float fps = 0;

    /// <summary> 当前录入的所有相机. </summary>
    std::vector<CameraDto> cameras;

    XUEXUE_JSON_OBJECT_M8(isCameraOpened, isGrab, isRunning, activeProcName, procs, frameCount, fps, cameras)
  private:
};

} // namespace dxlib