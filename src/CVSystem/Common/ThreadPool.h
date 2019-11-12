#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <string>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 线程消息,创建一个线程消息然后丢进去给线程池执行. </summary>
///
/// <remarks> Dx, 2019/8/7. </remarks>
///-------------------------------------------------------------------------------------------------
class ThreadMsg
{
  public:
    typedef std::function<void()> FunInit;
    typedef std::function<void(std::atomic_bool&, std::shared_ptr<ThreadMsg>)> FunDoWork;
    typedef std::function<void()> FunRelease;

    ThreadMsg() {}
    //ThreadMsg(FunInit pInit = nullptr, FunDoWork pDoWork = nullptr, FunRelease pRelease = nullptr)
    //    : init(pInit), doWork(pDoWork), release(pRelease) {}
    ThreadMsg(const std::string& name) : name(name) {}
    ~ThreadMsg() {}

    /// <summary> 一个序号id.</summary>
    size_t id;

    /// <summary> 名字.</summary>
    std::string name{"unmanned"};

    /// <summary> 这个工作消息还要不要工作.</summary>
    std::atomic_bool isRun{true};

    /// <summary> 执行的线程的ID.</summary>
    std::thread::id exeThreadID;

    /// <summary> 执行委托init.</summary>
    FunInit init{nullptr};

    /// <summary> 执行委托doWork.</summary>
    FunDoWork doWork{nullptr};

    /// <summary> 执行委托release. </summary>
    FunRelease release{nullptr};

    /// <summary> 执行完这个消息之后再执行的下一个消息. </summary>
    std::shared_ptr<ThreadMsg> nextMsg{nullptr};

#pragma region 执行状态

    std::atomic_bool isInitDone{false};
    std::atomic_bool isDoWorkDone{false};
    std::atomic_bool isReleaseDone{false};

#pragma endregion

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 等待这个消息执行完毕. </summary>
    ///
    /// <remarks> Dx, 2019/8/8. </remarks>
    ///-------------------------------------------------------------------------------------------------
    std::shared_ptr<ThreadMsg> waitDone()
    {
        //如果是自己执行的时候要停止自己
        if (std::this_thread::get_id() == exeThreadID) {
            return nextMsg;
        }
        else {
            while (true) {
                if (isReleaseDone.load()) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        return nextMsg;
    }

  private:
};
typedef std::shared_ptr<ThreadMsg> pThreadMsg;

///-------------------------------------------------------------------------------------------------
/// <summary> 全局的线程池. </summary>
///
/// <remarks> Dx, 2019/5/20. </remarks>
///-------------------------------------------------------------------------------------------------
class ThreadPool
{
  private:
    /// <summary> The instance. </summary>
    static ThreadPool* m_pInstance;

  public:
    ThreadPool();
    ~ThreadPool();

    /// <summary> 单例. </summary>
    static ThreadPool* GetInst()
    {
        return m_pInstance;
    }

    /// <summary> 当前这个线程池是否工作. </summary>
    std::atomic_bool isRun{true};

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 创建一个新的msg，会给它分配一个ID. </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <returns> A pThreadMsg. </returns>
    ///-------------------------------------------------------------------------------------------------
    pThreadMsg creatMsg();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 加入一个要处理的消息,这里好像必须要使用引用传值,否则绑定的匿名函数就没了. </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="msg"> [in] The message. </param>
    ///-------------------------------------------------------------------------------------------------
    void post(std::shared_ptr<ThreadMsg>& msg);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 标记停止当前的所有工作. </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void stop()
    {
        isRun = false;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 阻塞等待所有的工作执行完毕，注意送进去处理的消息不要自己等待自己处理完毕.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="timeSec"> (Optional) 等待的秒数,如果等于0或者小于0表示无限等待. </param>
    ///-------------------------------------------------------------------------------------------------
    void waitDone(int timeSec = 0);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 整个的关闭所有线程池，一般不应该调用这个. </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void dispose();

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 重新初始化,调用之前应该先调用dispose(),一般也不调用这个.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <param name="num_threads"> (Optional)线程池线程的个数. </param>
    ///-------------------------------------------------------------------------------------------------
    void reset(size_t num_threads = 4);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 等待执行的消息的计数. </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    int waitExecuteCount();

#if defined(_WIN32) || defined(_WIN64)

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 当前的线程设置高优先级.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/8/7. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void SetCurThreadPriorityHigh();

#endif

  private:
    class Impl;
    Impl* _impl{nullptr};
};
} // namespace dxlib
