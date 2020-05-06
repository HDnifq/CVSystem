#include "MultiCamera.h"

#include "dlog/dlog.h"

#include "../Common/StringHelper.h"
#include "../Common/CoolTime.hpp"
#include "../Common/Event.h"
#include "../Common/FPSCalc.hpp"

#include "CameraManger.h"
#include "CameraGrab.h"

#include <map>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 线程函数对象，执行一个帧处理. </summary>
///
/// <remarks> Dx, 2020/2/26. </remarks>
///-------------------------------------------------------------------------------------------------
class FrameProcRunnable : public Poco::Runnable
{
  public:
    FrameProcRunnable() {}
    ~FrameProcRunnable() {}

    /// <summary> proc对象. </summary>
    pFrameProc proc;

    /// <summary> 相机采图类. </summary>
    CameraGrab* _cameraGrab = nullptr;

    /// <summary> 是否保持运行. </summary>
    std::atomic_bool _isRun{true};

    /// <summary> 是否采图. </summary>
    std::atomic_bool _isGrab{true};

    /// <summary> 处理的fps. </summary>
    float fps = 0;

    /// <summary> 帧处理计数. </summary>
    uint frameCount = 0;

    /// <summary> 当前按下的按键记录（看后面要不要删掉）. </summary>
    std::atomic_int cvKey{-1};

    virtual void run()
    {
        proc->onEnable();

        while (true) {
            //如果这个线程已经是处在要关闭状态了，那么这个函数就赶紧退出吧
            if (!_isRun.load()) {
                break;
            }

            LogD("MultiCamera.run():相机采图并处理.frameCount=%u", this->frameCount);
            if (_isGrab.load()) {
                //提取图片组帧
                pCameraImage cimg;
                if (_cameraGrab != nullptr) {
                    _cameraGrab->grab(cimg);

                    cimg->fnum = frameCount;
                    cimg->procStartTime = clock(); //标记处理开始时间

                    fps = _fpsCalc.update(++frameCount);

                    LogD("MultiCamera.run():执行proc!");
                    int ckey = -1; //让proc去自己想检测keydown就keydown
                    proc->process(cimg, ckey);
                    if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                        cvKey.exchange(ckey);
                    }

                    //干脆用这个线程来驱动检查事件
                    Event::GetInst()->checkMemEvent();
                    cimg->procEndTime = clock(); //标记处理结束时间
                    LogD("MultiCamera.run():采图耗时%.0fms,处理耗时%.0fms", cimg->grabCostTime(), cimg->procCostTime());
                }
                else {

                    LogE("MultiCamera.run():_cameraGrab为NULL!");
                    _isGrab.exchange(false); //那就不采图了算了

                    //break;
                }
            }
            else {
                //姑且也检查一下事件
                Event::GetInst()->checkMemEvent();

                //选一个proc进行图像的处理
                LogD("MultiCamera.run():执行proc!");
                int ckey = -1; //让proc去自己想检测keydown就keydown
                proc->onLightSleep(ckey);
                if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                    cvKey.exchange(ckey);
                }

                //如果不抓图那么就睡眠
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }

        //执行完了准备退出
        proc->onDisable();
    }

  private:
    // 计算fps的辅助.
    FPSCalc _fpsCalc;
};

class MultiCamera::Impl
{
  public:
    Impl()
    {
    }
    ~Impl()
    {
    }

    /// <summary> 一次只使能一个处理. </summary>
    std::vector<pFrameProc> vProc;

    /// <summary> 当前激活的处理Index. </summary>
    uint _activeProcIndex = 0;

    /// <summary> 相机采图类. </summary>
    CameraGrab _cameraGrab;

    /// <summary> 工作线程 </summary>
    Poco::Thread* pThread = nullptr;

    /// <summary> 当前的执行对象. </summary>
    FrameProcRunnable* curRunnable = nullptr;

    /// <summary> 是否相机已经打开. </summary>
    std::atomic_bool _isOpened{false};

    /// <summary> 是否正在打开相机. </summary>
    std::atomic_bool _isOpening{false};

    /// <summary> 是否相机正在关闭. </summary>
    std::atomic_bool _isCloseing{false};

    /// <summary> 是否正在停止. </summary>
    std::atomic_bool _isStopping{false};

  private:
};

MultiCamera::MultiCamera()
{
    _impl = new MultiCamera::Impl();
}

MultiCamera::~MultiCamera()
{
    delete _impl;
}

MultiCamera* MultiCamera::m_pInstance = NULL;

uint MultiCamera::activeProcIndex()
{
    return _impl->_activeProcIndex;
}

FrameProc* MultiCamera::activeProc()
{
    if (_impl->_activeProcIndex < _impl->vProc.size()) {
        return _impl->vProc[_impl->_activeProcIndex].get();
    }
    return nullptr;
}

void MultiCamera::addProc(const pFrameProc& proc)
{
    _impl->vProc.push_back(proc);
}

void MultiCamera::addProc(FrameProc* proc)
{
    _impl->vProc.push_back(pFrameProc(proc));
}

FrameProc* MultiCamera::getProc(uint index)
{
    if (index < _impl->vProc.size()) {
        return _impl->vProc[index].get();
    }
    else {
        return nullptr;
    }
}

std::vector<pFrameProc> MultiCamera::getAllProc()
{
    return _impl->vProc;
}

void MultiCamera::clearProc()
{
    if (isRunning()) {
        LogW("MultiCamera.clearProc():当前有计算线程正在工作,不能清空..");
        return;
    }
    _impl->vProc.clear();
    _impl->_activeProcIndex = 0;
}

void MultiCamera::setIsGrab(bool isGrab)
{
    if (_impl->curRunnable != nullptr) {
        _impl->curRunnable->_isGrab = isGrab;
    }
}

bool MultiCamera::isGrab()
{
    if (_impl->curRunnable != nullptr) {
        return _impl->curRunnable->_isGrab.load();
    }
    return false;
}

bool MultiCamera::isCameraOpened()
{
    return _impl->_isOpened.load();
}

bool MultiCamera::isRunning()
{
    if (_impl->pThread != nullptr && _impl->pThread->isRunning())
        return true;
    else {
        return false;
    }
}

const char* MultiCamera::activeProcName()
{
    if (_impl->vProc.size() > _impl->_activeProcIndex)
        return _impl->vProc[_impl->_activeProcIndex]->name();
    else
        return "";
}

bool MultiCamera::openCamera()
{
    if (_impl->_isOpened.load() || _impl->_isOpening.load()) {
        LogI("MultiCamera.openCamera():当前相机已经打开,需要先close(),函数直接返回...");
        return false;
    }
    LogI("MultiCamera.openCamera():开始打开相机...");
    _impl->_isOpening.exchange(true); //标记正在打开了

    //根据当前录入的相机的东西里的设置来打开相机
    _impl->_cameraGrab.setCameras(CameraManger::GetInst()->camMap);

    bool isSuccess = _impl->_cameraGrab.open();
    if (isSuccess) {
        LogI("MultiCamera.openCamera():cameraGrab相机打开成功！");

        //这里随便检察一下
        if (_impl->vProc.size() == 0) {
            LogW("MultiCamera.openCamera():当前没有添加处理proc...");
        }
    }
    else {
        LogE("MultiCamera.openCamera():cameraGrab相机打开有失败的相机!");
    }

    _impl->_isOpened.exchange(true);
    _impl->_isOpening.exchange(false);

    return isSuccess;
}

void MultiCamera::closeCamera()
{
    if (!_impl->_isOpened.load() || _impl->_isCloseing.load()) {
        LogI("MultiCamera.closeCamera():已经停止或正在停止，函数直接返回...");
        return;
    }
    _impl->_isCloseing.exchange(true);

    //释放采图
    LogI("MultiCamera.closeCamera():关闭相机...");
    _impl->_cameraGrab.close();

    _impl->_isOpened.exchange(false);
    _impl->_isCloseing.exchange(false);
    LogI("MultiCamera.closeCamera():终于执行完了,close()返回...");
}

void MultiCamera::start(uint activeProcindex)
{
    if (this->isRunning()) {
        LogW("MultiCamera.start():当前计算线程正在执行,可能导致泄漏!");
    }
    LogI("MultiCamera.start():创建综合分析计算线程!");
    if (activeProcindex >= _impl->vProc.size()) {
        LogE("MultiCamera.start():启动失败，输入activeProcindex=%u过大,当前vProc的size为%zu!", activeProcindex, _impl->vProc.size());
        return;
    }
    _impl->_activeProcIndex = activeProcindex;

    _impl->curRunnable = new FrameProcRunnable();
    _impl->curRunnable->_cameraGrab = &(_impl->_cameraGrab);
    _impl->curRunnable->proc = _impl->vProc[_impl->_activeProcIndex];

    _impl->pThread = new Poco::Thread();
    _impl->pThread->setName(_impl->curRunnable->proc->name());
    _impl->pThread->setPriority(Poco::Thread::Priority::PRIO_HIGHEST);

    _impl->pThread->start(*(_impl->curRunnable));
}

void MultiCamera::stop()
{
    //停止自己的线程
    if (_impl->curRunnable != nullptr) {

        //标记处理工作停止
        _impl->curRunnable->_isRun.exchange(false);
        LogI("MultiCamera.stop():标记综合分析线程关闭！");
    }

    //开始等待线程结束，这里总是会遇到一个自己调用close的问题
    if (_impl->pThread != nullptr) {
        //如果不是自己关自己（Poco::Thread::current()这个函数会返回null，如果不是Poco的线程的话）
        if (Poco::Thread::current() == nullptr || Poco::Thread::current()->id() != _impl->pThread->id()) {
            _impl->pThread->join(); //等待它结束
            delete _impl->pThread;
            delete _impl->curRunnable;
            _impl->pThread = nullptr;
            _impl->curRunnable = nullptr;
        }
        else {
            LogW("MultiCamera.stop():是线程自己关闭自己！");
        }
    }
    else {
    }
}

uint MultiCamera::frameCount()
{
    if (_impl->curRunnable != nullptr) {
        return _impl->curRunnable->frameCount;
    }
    return 0;
}

float MultiCamera::fps()
{
    if (_impl->curRunnable != nullptr) {
        return _impl->curRunnable->fps;
    }
    return 0;
}

} // namespace dxlib