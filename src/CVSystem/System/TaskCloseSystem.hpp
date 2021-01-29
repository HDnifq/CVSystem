#pragma once

#include <chrono>
#include <memory>

#include "MultiCamera.h"

#include "dlog/dlog.h"

#include "Poco/ThreadPool.h"
#include "Poco/TaskManager.h"

#include "FrameProc.h"

#include <atomic>

namespace dxlib {

/**
 * 当工作线程自己关闭自己的时候那么只能使用这个异步的关闭了
 *
 * @author daixian
 * @date 2020/11/27
 */
class TaskCloseSystem : public Poco::Runnable
{
  public:
    TaskCloseSystem(bool isClearProc) : isClearProc(isClearProc)
    {}

    virtual ~TaskCloseSystem()
    {}

    // 是否清空proc
    bool isClearProc = false;

    /**
     * 线程执行函数.
     *
     * @author daixian
     * @date 2020/11/27
     */
    virtual void run()
    {
        LogI("TaskCloseSystem.run():执行异步关闭系统..");
        MultiCamera::GetInst()->stop();
        MultiCamera::GetInst()->closeCamera();
        if (isClearProc) {
            MultiCamera::GetInst()->clearProc();
        }
    }

  private:
};
} // namespace dxlib