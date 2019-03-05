#pragma once

#include <atomic>
#include <Chrono>
#include <thread>

#ifdef DLOG_EXPORT
#include "../dlog/dlog.h"
#else
#define LogD printf
#define LogI printf
#define LogW printf
#define LogE printf
#endif // DLOG_EXPORT

//把userObj的指针转换成对象引用
//第一个参数T为要转换的类型，第二个参数tb为BaseThread指针
#ifndef TO_USER_OBJ
#define TO_USER_OBJ(T, tb) (*(T*)tb->userObj);
#endif

#define USE_BTGC 1

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 简单的基础线程类,它不能直接构造,必须使用静态的create函数来创建一个对象,
/// 当线程函数执行完毕之后自动释放.
/// create()函数的两个参数就表示线程的工作时候会调用的函数.
/// </summary>
///
/// <remarks> Dx, 2019/1/15. </remarks>
///-------------------------------------------------------------------------------------------------
class BaseThread
{
    typedef std::function<void(std::shared_ptr<BaseThread>&)> FunInit;
    typedef std::function<void(std::shared_ptr<BaseThread>&)> FunWorkOnce;
    typedef std::function<void(std::shared_ptr<BaseThread>&)> FunRelease;

#ifdef USE_BTGC
    /// <summary> BaseThread的垃圾堆. </summary>
    class BTGC
    {
      public:
        BTGC();
        ~BTGC();
        /// <summary> 添加进垃圾堆. </summary>
        void add(std::shared_ptr<BaseThread>& bt);
        /// <summary> 清空垃圾堆. </summary>
        void clear();

      private:
        struct Data;
        Data* data;
    };
    static BTGC* btgc;
#endif // USE_BTGC

  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 构造:创建一个BaseThread对象,创建的同时启动.
    /// this->_thread = BaseThread::creat(std::bind(&MultiCameraMT::init, this, std::placeholders::_1),
    ///                                   std::bind(&MultiCameraMT::workonce, this, std::placeholders::_1),
    ///                                   std::bind(&MultiCameraMT::release, this, std::placeholders::_1));
    /// </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///
    /// <param name="pInit">     [in] 函数指针委托,没有可以使用null. </param>
    /// <param name="pWorkOnce"> [in] 函数指针委托. </param>
    /// <param name="pRelease">  [in] 函数指针委托. </param>
    /// <param name="userObj">   [in] 用户的绑定数据,为了开发方便. </param>
    ///
    /// <returns> 具有该对象的持有. </returns>
    ///-------------------------------------------------------------------------------------------------
    static std::shared_ptr<BaseThread> creat(FunInit pInit, FunWorkOnce pWorkOnce, FunRelease pRelease = nullptr, void* userObj = nullptr)
    {
        std::shared_ptr<BaseThread> spbt = std::shared_ptr<BaseThread>(new BaseThread(pInit, pWorkOnce, pRelease));
        spbt->userObj = userObj;
        spbt->start(spbt);
        return spbt;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 手动找个时间进行GC,避免出现线程自己释放自己. </summary>
    ///
    /// <remarks> Dx, 2019/1/16. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void GC()
    {
#ifdef USE_BTGC
        btgc->clear();
#endif // USE_BTGC
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 析构函数. </summary>
    ///
    /// <remarks> Dx, 2019/1/16. </remarks>
    ///-------------------------------------------------------------------------------------------------
    ~BaseThread()
    {
        if (std::this_thread::get_id() == _thread->get_id()) { //如果是自己析构自己
            LogE("BaseThread.~BaseThread():自己析构自己,糟糕..('▽'*)!,没办法了,只能试试直接返回了");
            return;
        }
        int count = 0;
        while (_thread == nullptr) { //如果这里为null那么应该是赋值不是原子操作导致此时读值还为null
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            count++;
            if (count > 10) { //尝试10次
                LogE("BaseThread.~BaseThread():线程的引用好像丢了...('▽'*)");
                return;
            }
        }

        if (_isThreadFunReturn.load() == false) {
            LogE("BaseThread.~BaseThread():出乎意料呀(〃'▽'〃),此时线程函数还没执行完 id=%d!", _thread->get_id());
        }

        //这里必须要调用一次这个join()函数否则会出错
        if (_thread->joinable()) {
            _thread->join();
        }
        //由于doWork()函数持有了自身的引用，那么这个函数只有当工作线程执行完线程函数之后(当有其他人持有之后还会更后)才会进入
        LogI("BaseThread.~BaseThread():线程释放完毕id=%d! ", _thread->get_id());
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 停止工作. </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void stop(bool isWait = true)
    {
        _isRun = false;
        _init = nullptr;
        _workOnce = nullptr;

        //如果是当前线程执行的Stop，那么就直接执行一次
        if (std::this_thread::get_id() == _thread->get_id()) {
            if (!_isReleaseProc.load() && _release != nullptr) {
                _isReleaseProc = true;
                _release(_self);
            }
            _release = nullptr;
        }
        else { //否则就等待线程执行完毕
            if (isWait) {
                while (_isThreadFunReturn.load() == false) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 查询线程函数是否已经执行完毕返回了. </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///
    /// <returns> 如果线程函数已经执行完毕了那么就返回ture. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isThreadFunReturn()
    {
        return _isThreadFunReturn.load();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 查询此时控制标志isRun的值,如果isRun为false那么就表示这个线程已经进入了待关闭状态. </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///
    /// <returns> isRun的值. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isRun()
    {
        return _isRun.load();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 查询这个线程的id. </summary>
    ///
    /// <remarks> Dx, 2019/1/16. </remarks>
    ///
    /// <returns> 返回线程id. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::thread::id get_id()
    {
        if (_thread != nullptr) {
            return _thread->get_id();
        }
        return std::thread::id();
    }

    /// <summary> 用户绑定这个对象的数据，为了方便一些lamuda表达式的匿名函数. </summary>
    void* userObj = nullptr;

  private:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> 私有构造函数. </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///
    /// <param name="pInit">     [in] 函数指针委托. </param>
    /// <param name="pWorkOnce"> [in] 函数指针委托. </param>
    /// <param name="pRelease">  [in] 函数指针委托. </param>
    ///-------------------------------------------------------------------------------------------------
    BaseThread(FunInit pInit, FunWorkOnce pWorkOnce, FunRelease pRelease)
        : _init(pInit), _workOnce(pWorkOnce), _release(pRelease)
    {
    }

    /// <summary> 是否工作的控制标记. </summary>
    std::atomic_bool _isRun = false;

    /// <summary> 实际线程. </summary>
    std::shared_ptr<std::thread> _thread = nullptr;

    /// <summary> 执行委托 Init. (不是原子操作，所以用线程参数传值补救一下) </summary>
    FunInit _init = nullptr;

    /// <summary> 执行委托 WorkOnce. (不是原子操作，所以用线程参数传值补救一下) </summary>
    FunWorkOnce _workOnce = nullptr;

    /// <summary> 执行委托 Release. (不是原子操作，所以用线程参数传值补救一下)</summary>
    FunRelease _release = nullptr;

    /// <summary> 确保只执行一次release的标记</summary>
    std::atomic_bool _isReleaseProc = false;

    /// <summary> 是否线程函数已经执行完毕了. </summary>
    std::atomic_bool _isThreadFunReturn = false;

    /// <summary> 自身引用的持有. </summary>
    std::shared_ptr<BaseThread> _self;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 启动线程. </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///
    /// <param name="spbt"> The spbt. </param>
    ///-------------------------------------------------------------------------------------------------
    void start(const std::shared_ptr<BaseThread>& spbt)
    {
        _isRun = true;
        _self = spbt; //不是原子操作（有隐患）
        //要注意this->thread的赋值并不是一个原子操作
        this->_thread = std::shared_ptr<std::thread>(new std::thread(&BaseThread::doWork, this, spbt, _init, _workOnce, _release)); //成员函数可能都有一个默认对象函数指针
        LogI("BaseThread.start():创建线程id=%d!", _thread->get_id());
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 工作线程执行的函数(使用传参持有一个自身的引用). </summary>
    ///
    /// <remarks> Dx, 2019/1/15. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void doWork(std::shared_ptr<BaseThread> bt, FunInit init, FunWorkOnce workOnce, FunRelease release)
    {
        bt->_self = bt; //记录一个自身的持有

        //哎，没办法，这里赋值一下再，后面就自求多福吧
        _init = init;
        _workOnce = workOnce;
        _release = release;
        //这个的后面都使用成员对象，方便响应stop的时候置为了null

        //执行一次初始化 Init
        if (bt->_isRun.load() && _init != nullptr)
            _init(bt);

        //不停的执行执行委托工作 WorkOnce
        while (_isRun.load()) {
            if (_workOnce != nullptr)
                _workOnce(bt);
            else
                break; //连工作委托都没有那么就直接退出吧
        }

        if (!_isReleaseProc.load() && _release != nullptr) {
            _isReleaseProc = true;
            _release(bt);
        }

        //归还自身引用持有
        bt->_self = nullptr;

        //执行到这里之后,然而此时出栈导致bt释放
#ifdef USE_BTGC
        btgc->add(bt);
#endif // USE_BTGC \
    //标记一下线程真的已经退出了
        _isThreadFunReturn = true;
    }
};
typedef std::shared_ptr<BaseThread> pBaseThread;
} // namespace dxlib