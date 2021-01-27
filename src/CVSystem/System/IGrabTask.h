#pragma once

#include <chrono>
#include <memory>

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

    // proc对象.
    pFrameProc proc = nullptr;

    // Runnable
    virtual void run() = 0;

  private:
};

} // namespace dxlib