#pragma once

#include "FrameProc.h"

namespace dxlib {

/**
 * 使用单线程来采图加处理的MultiCamera.
 *
 * @author daixian
 * @date 2019/3/5
 */
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

    /**
     * 打开相机,它只打开相机,不会启动计算线程.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @returns 打开成功返回ture.
     */
    bool openCamera();

    /**
     * 关闭所有相机.
     *
     * @author daixian
     * @date 2020/2/26
     */
    void closeCamera();

    /**
     * 启动计算线程.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @param  activeProcindex (Optional) 活动的proc的index.
     */
    void start(uint activeProcindex = 0);

    /**
     * 停止计算线程.
     *
     * @author daixian
     * @date 2020/2/26
     */
    void stop();

    /**
     * 添加一个Proc.
     *
     * @author daixian
     * @date 2019/1/10
     *
     * @param  proc proc的智能指针.
     */
    void addProc(const pFrameProc& proc);

    /**
     * 添加一个Proc.
     *
     * @author daixian
     * @date 2019/1/10
     *
     * @param [in] proc proc的指针,会用它创建智能指针.
     */
    void addProc(FrameProc* proc);

    /**
     * 得到一个Proc.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @param  index proc的index.
     *
     * @returns 处理对象.
     */
    FrameProc* getProc(uint index);

    /**
     * 当前激活的Proc的index.
     *
     * @author daixian
     * @date 2019/3/20
     *
     * @returns 当前激活的Proc的index.
     */
    uint activeProcIndex();

    /**
     * 得到所有的proc.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @returns 处理对象.
     */
    std::vector<pFrameProc> getAllProc();

    /**
     * 当前激活的Proc.
     *
     * @author daixian
     * @date 2019/3/24
     *
     * @returns 当前激活的Proc的指针,为空表示没有proc.
     */
    FrameProc* activeProc();

    /**
     * 清空所有Proc
     *
     * @author daixian
     * @date 2019/3/21
     */
    void clearProc();

    /**
     * 设置当前是否采图，主要是为了在不需要采图的手减少开销.
     * (如果不采图那么处理线程就会一直不停的休眠)
     *
     * @author daixian
     * @date 2018/11/20
     *
     * @param  isGrab 如果是真就是采图.
     */
    void setIsGrab(bool isGrab);

    /**
     * 当前的抓图状态,如果不采图那么处理线程就会一直不停的休眠.
     *
     * @author daixian
     * @date 2019/3/20
     *
     * @returns 如果是真就是采图.
     */
    bool isGrab();

    /**
     * 是否相机已经打开.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @returns 相机已经打开返回true.
     */
    bool isCameraOpened();

    /**
     * 是否当前计算线程正在工作.
     *
     * @author daixian
     * @date 2019/1/16
     *
     * @returns 计算线程正在工作返回true.
     */
    bool isRunning();

    /**
     * 当前执行的proc的名字,如果没有proc就返回null.
     *
     * @author daixian
     * @date 2019/3/20
     *
     * @returns 前执行的proc的名字.
     */
    const char* activeProcName();

    /**
     * 当前帧数.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @returns 当前帧计数.
     */
    uint frameCount();

    /**
     * 当前工作的fps.
     *
     * @author daixian
     * @date 2020/2/26
     *
     * @returns fps.
     */
    float fps();

  private:
    static MultiCamera* m_pInstance;

    // 隐藏成员.
    class Impl;
    Impl* _impl;
};

} // namespace dxlib