#pragma once
#include "../Model/CameraImageGroup.h"

namespace dxlib {

/**
 * 画面帧处理接口对象.
 *
 * @author daixian
 * @date 2018/11/16
 */
class FrameProc
{
  public:
    virtual ~FrameProc(){};

    /**
     * 处理函数接口,返回值作为一个key值,如果没有按下按键,那么这里返回-1(和waitkey()的无按键返回值相同).
     *
     * @author daixian
     * @date 2019/3/19
     *
     * @param          camImage 采集的处理图像.
     * @param [in,out] key      这里waitkey()得到的按键key值.
     */
    virtual void process(pCameraImageGroup camImage, int& key) = 0;

    /**
     * 浅睡眠,此时相机没有抓图,没有采图.但是会定时300ms传出这个事件来可以执行一些函数.
     *
     * @author daixian
     * @date 2019/4/14
     *
     * @param [in,out] key The key.
     */
    virtual void onLightSleep(int& key) {}

    /**
     * 被使能的时候的响应.
     *
     * @author daixian
     * @date 2018/10/18
     */
    virtual void onEnable() = 0;

    /**
     * 不使能的时候的响应.
     *
     * @author daixian
     * @date 2018/9/28
     */
    virtual void onDisable() = 0;

    /**
     * 当前proc的标示名.
     *
     * @author daixian
     * @date 2019/3/19
     *
     * @returns 当前proc的标示名.
     */
    virtual const char* name()
    {
        return "unmanned proc";
    }
};
typedef std::shared_ptr<FrameProc> pFrameProc;

} // namespace dxlib