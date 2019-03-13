﻿#pragma once
#include "CameraImage.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 画面帧处理接口对象. </summary>
///
/// <remarks> Surface, 2018/11/16. </remarks>
///-------------------------------------------------------------------------------------------------
class FrameProc
{
  public:
    virtual ~FrameProc(){};

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 处理函数接口,返回值作为一个key值,如果没有按下按键,那么这里返回-1(和waitkey()的无按键返回值相同).
    /// </summary>
    ///-------------------------------------------------------------------------------------------------
    virtual int process(pCameraImage camImage) = 0;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 被使能的时候的响应. </summary>
    ///
    /// <remarks> Xian Dai, 2018/10/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    virtual void onEnable() = 0;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 不使能的时候的响应. </summary>
    ///
    /// <remarks> Xian Dai, 2018/9/28. </remarks>
    ///-------------------------------------------------------------------------------------------------
    virtual void onDisable() = 0;
};
typedef std::shared_ptr<FrameProc> pFrameProc;
} // namespace dxlib