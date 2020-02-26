#pragma once
#include "CameraManger.h"
#include "MultiCamera.h"
#include "DevicesHelper.h"
#include "CamImageProc.hpp"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个最简单的模块. </summary>
///
/// <remarks> Surface, 2019/1/20. </remarks>
///-------------------------------------------------------------------------------------------------
class SimpleModule
{
  public:
    SimpleModule()
    {}
    ~SimpleModule()
    {}

    void init()
    {
        LogI("SimpleModule.init():初始化设置...");
        CameraManger::GetInst()->clear();

        //得到第一个相机名,然后添加
        DevicesHelper::GetInst()->listDevices();
        if (DevicesHelper::GetInst()->devList.size() == 0) {
            return;
        }
        std::wstring camName = DevicesHelper::GetInst()->devList.begin()->second;
        CameraManger::GetInst()->add(pCamera(new Camera(0, camName, cv::Size(1280, 720), 16)));

        //加入proc
        MultiCamera::GetInst()->clearProc();
        MultiCamera::GetInst()->addProc(pFrameProc(new CamImageProc()));
    }

    void start()
    {
        LogI("SimpleModule.start():打开相机！");
        MultiCamera::GetInst()->openCamera(); //打开相机
        MultiCamera::GetInst()->start(0);
    }

  private:
};
} // namespace dxlib