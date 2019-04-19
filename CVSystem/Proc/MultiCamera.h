#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include "dlog/dlog.h"
#include "../Common/BaseThread.h"
#include "../Common/FPSCalc.hpp"
#include "FrameProc.h"
#include "CameraGrab.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 使用单线程来采图加处理的MultiCameraMT. </summary>
///
/// <remarks> Dx, 2019/3/5. </remarks>
///-------------------------------------------------------------------------------------------------
class MultiCamera
{
  public:
    MultiCamera();
    ~MultiCamera();
    static MultiCamera* GetInst()
    {
        if (m_pInstance == NULL)
            m_pInstance = new MultiCamera();
        return m_pInstance;
    }

    /// <summary> 帧处理计数. </summary>
    uint frameCount = 0;

    /// <summary> 处理的fps. </summary>
    float fps = 0;

    /// <summary> (直接暴露出来使用)一次只使能一个处理. </summary>
    std::vector<pFrameProc> vProc;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 打开相机会启动线程函数,目前一般尝试创建一个线程来计算,
    /// 这里需要做一个预先的激活proc的设置（activeIndex赋值）,
    /// 以便第一帧运行就能正确的执行到想要的proc.
    /// 如果相机打开失败的话那么就不会创建分析线程。采图线程也不会创建了。.
    /// </summary>
    ///
    /// <remarks> Dx, 2018/2/1. </remarks>
    ///
    /// <param name="activeIndex"> (Optional)设置当前激活的Proc. </param>
    /// <param name="openType">    (Optional)为true则创建计算线程，为false则需要手动的运行run函数. </param>
    ///
    /// <returns> 打开成功返回ture. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool openCamera(uint activeIndex = 0);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭所有相机. </summary>
    ///
    /// <remarks> Dx, 2018/3/1. </remarks>
    ///
    /// <param name="isDeleteProc"> (Optional)是否要释放所有proc. </param>
    ///-------------------------------------------------------------------------------------------------
    void close(bool isDeleteProc = false);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个Proc. </summary>
    ///
    /// <remarks> Dx, 2019/1/10. </remarks>
    ///
    /// <param name="proc"> The proc. </param>
    ///-------------------------------------------------------------------------------------------------
    void addProc(const pFrameProc& proc);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个Proc. </summary>
    ///
    /// <remarks> Dx, 2019/1/10. </remarks>
    ///
    /// <param name="proc"> [in,out] If non-null, the proc. </param>
    ///-------------------------------------------------------------------------------------------------
    void addProc(FrameProc* proc);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>设置当前激活的Proc,这个函数会有效的标记activeProcIndex. </summary>
    ///
    /// <remarks> Dx, 2018/1/30. </remarks>
    ///
    /// <param name="index"> Zero-based index of the. </param>
    ///-------------------------------------------------------------------------------------------------
    void setActiveProc(uint index);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 当前激活的Proc的index. </summary>
    ///
    /// <remarks> Surface, 2019/3/20. </remarks>
    ///
    /// <returns> 当前激活的Proc的index. </returns>
    ///-------------------------------------------------------------------------------------------------
    uint activeProcIndex()
    {
        return _activeProcIndex;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 当前激活的Proc. </summary>
    ///
    /// <remarks> Dx, 2019/3/24. </remarks>
    ///
    /// <returns> 当前激活的Proc的指针,为空表示没有proc. </returns>
    ///-------------------------------------------------------------------------------------------------
    FrameProc* activeProc()
    {
        if (_activeProcIndex < vProc.size()) {
            return vProc[_activeProcIndex].get();
        }
        return nullptr;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 重置当前生效的proc,会对当前proc重新执行一次disable和enable.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/3/21. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void resetActiveProc()
    {
        _isResetActiveProc = true;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置当前是否采图，主要是为了在不需要采图的手减少开销. </summary>
    ///
    /// <remarks>
    /// 如果不采图那么处理线程就会一直不停的休眠 Dx, 2018/11/20.
    /// </remarks>
    ///
    /// <param name="isGrab"> 如果是真就是采图. </param>
    ///-------------------------------------------------------------------------------------------------
    void setIsGrab(bool isGrab)
    {
        _isGrab = isGrab;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 当前的抓图状态,如果不采图那么处理线程就会一直不停的休眠. </summary>
    ///
    /// <remarks> Surface, 2019/3/20. </remarks>
    ///
    /// <returns> 如果是真就是采图. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isGrab()
    {
        return _isGrab.load();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 是否当前正在正常工作. </summary>
    ///
    /// <remarks> Dx, 2019/1/16. </remarks>
    ///
    /// <returns> True if run, false if not. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isRun()
    {
        return _isRun.load();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 当前执行的proc的名字,如果没有proc就返回null. </summary>
    ///
    /// <remarks> Surface, 2019/3/20. </remarks>
    ///
    /// <returns> 前执行的proc的名字. </returns>
    ///-------------------------------------------------------------------------------------------------
    const char* activeProcName()
    {
        if (vProc.size() > _activeProcIndex)
            return vProc[_activeProcIndex]->name();
        else
            return nullptr;
    }

  private:
    static MultiCamera* m_pInstance;

    /// <summary> (直接暴露出来，只读取)当前激活的处理Index. </summary>
    uint _activeProcIndex = 0;

    /// <summary> 相机采图类. </summary>
    CameraGrab _cameraGrab;

    /// <summary> 综合分析线程. </summary>
    pBaseThread _thread = nullptr;

    /// <summary> 是否停止. </summary>
    std::atomic_bool _isRun = false;

    /// <summary> 是否正在打开. </summary>
    std::atomic_bool _isOpening = false;

    /// <summary> 是否正在停止. </summary>
    std::atomic_bool _isStopping = false;

    /// <summary> 是否采图. </summary>
    std::atomic_bool _isGrab = true;

    /// <summary> 设置的下一个激活index. </summary>
    uint _nextActiveProcIndex = 0;

    /// <summary> 是否对当前proc重新执行一次disable和enable. </summary>
    bool _isResetActiveProc = false;

    /// <summary> 提供给BaseThread的bing执行事件. </summary>
    void init(std::shared_ptr<BaseThread>& tb);

    /// <summary> 提供给BaseThread的bing执行事件. </summary>
    void workonce(std::shared_ptr<BaseThread>& tb);

    /// <summary> 提供给BaseThread的bing执行事件. </summary>
    void release(std::shared_ptr<BaseThread>& tb);

    //计算fps的辅助
    FPSCalc _fpsCalc;
};

} // namespace dxlib