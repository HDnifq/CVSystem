#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "Poco/ThreadPool.h"
#include "Poco/TaskManager.h"

#include "FrameProc.h"

namespace dxlib {

class IGrabTask : public Poco::Runnable
{
  public:
    IGrabTask() {}
    virtual ~IGrabTask() {}

    // 是否运行
    std::atomic_bool isRun{true};

    // 是否执行采图(如果不采图了那么会降低cpu开销).
    std::atomic_bool isGrab{true};

    // 处理的fps.
    float fps = 0;

    // 当前按下的按键记录（看后面要不要删掉）.
    std::atomic_int cvKey{-1};

    // 是否是主任务(只有主任务才执行事件)
    bool isMainTask = false;

    // 是否执行处理
    bool isDoProc = false;

    // 处理的忽略帧,会直接丢弃这一帧,同时递减这个Count.
    int discardFrameCount = 0;

    // 帧标记信息(相当于忽略帧的细分功能)
    std::vector<std::string> frameFlag;

    // 帧标记信息的计数.
    std::vector<int> frameFlagCount;

    // proc对象.
    pFrameProc proc = nullptr;

    // 新的要执行的,它保持为null.在切换到了新的proc之后会把它设置会null.
    pFrameProc _nextProc = nullptr;

    // Runnable
    virtual void run() = 0;

    /**
     * @brief 设置新的帧处理.
     * @param newProc
     */
    void setNewFrameProc(pFrameProc newProc)
    {
        if (_nextProc != nullptr) {
            LogE("IGrabTask.setNewFrameProc():任务还没有来得及响应切换,设置的太快了!");
        }
        _nextProc = newProc;
    }

    /**
     * 设置一个帧标记.
     *
     * @author daixian
     * @date 2021/7/22
     *
     * @param  flag  帧标记信息.
     * @param  count 持续的帧数.
     */
    void setFrameFlag(const std::string& flag, int count)
    {
        frameFlag.push_back(flag);
        frameFlagCount.push_back(count);
    }

    /**
     * 检察并清空帧Flag,如果frameFlagCount已经到零了就清空这一组.
     *
     * @author daixian
     * @date 2021/7/22
     */
    void clearFrameFlag()
    {
        for (size_t i = 0; i < frameFlagCount.size(); i++) {
            if (frameFlagCount[i] <= 0) {
                frameFlagCount.erase(frameFlagCount.begin() + i);
                frameFlag.erase(frameFlag.begin() + i);
                i--;
            }
        }
    }

  private:
};
} // namespace dxlib
