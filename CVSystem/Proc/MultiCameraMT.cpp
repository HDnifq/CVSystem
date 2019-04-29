#include "MultiCameraMT.h"

#include "DevicesHelper.h"
#include <iostream>
#include <map>
#include "../Common/StringHelper.h"
#include "../Common/CoolTime.hpp"
#include "CameraManger.h"
#include "../Common/Event.h"

#include <chrono>
#include <thread>

namespace dxlib {

MultiCameraMT::MultiCameraMT()
{
}

MultiCameraMT::~MultiCameraMT()
{
    close();
}

MultiCameraMT* MultiCameraMT::m_pInstance = NULL;

/// <summary> 提供给BaseThread的bing执行事件. </summary>
void MultiCameraMT::init(std::shared_ptr<BaseThread>& tb)
{
    LogI("MultiCameraMT.init():开始执行init委托!");

    while (cameraThread->isHasThread()) { //如果存在采图线程
        //如果这个线程已经是处在要关闭状态了，那么这个函数就赶紧退出吧
        if (!tb->isRun()) {
            return;
        }

        if (cameraThread->isThreadWaitingStart.load() == true) {
            break;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); //等待相机线程进入了线程函数并且在等待了
        }
    }
    cv_ct.notify_all(); //一声令下小线程们同时开始工作
    LogI("MultiCameraMT.init():init委托结束!");
}

/// <summary> 提供给BaseThread的bing执行事件. </summary>
void MultiCameraMT::release(std::shared_ptr<BaseThread>& tb)
{
    LogI("MultiCameraMT.release():开始执行release委托!");

    if (vProc.size() > _activeProcIndex) //防止用户没有加入proc的情况，还是应该判断一下
        vProc[_activeProcIndex]->onDisable();

    //执行一次关闭
    close(false);
    LogI("MultiCameraMT.release():release委托执行完毕.");
}

/// <summary> 提供给BaseThread的bing执行事件. </summary>
void MultiCameraMT::workonce(std::shared_ptr<BaseThread>& tb)
{
    std::unique_lock<std::mutex> lck(mtx_mt);

    LogD("MultiCameraMT.workonce():进入睡眠...");
    //反正基本就是等待小线程们的通知
    cv_mt.wait(lck);

    while (true) {
        //如果这个线程已经是处在要关闭状态了，那么这个函数就赶紧退出吧
        if (!tb->isRun()) {
            return;
        }

        //重设激活的proc
        if (_nextActiveProcIndex != _activeProcIndex) { //如果变化了(这里或许需要原子操作)‘
            LogD("MultiCameraMT.workonce():执行设置_activeProcIndex");
            if (vProc.size() > _activeProcIndex)
                vProc[_activeProcIndex]->onDisable();
            _activeProcIndex = _nextActiveProcIndex;
            if (vProc.size() > _activeProcIndex)
                vProc[_activeProcIndex]->onEnable();
        }
        //重启当前的proc
        if (_isResetActiveProc) {
            _isResetActiveProc = false;
            LogD("MultiCameraMT.workonce():执行重启当前的proc");
            if (vProc.size() > _activeProcIndex) {
                vProc[_activeProcIndex]->onDisable();
                vProc[_activeProcIndex]->onEnable();
            }
        }

        LogD("MultiCameraMT.workonce():在队列里提取图片！");
        //提取图片组帧
        pCameraImage cimg;
        if (cameraThread->try_dequeue(cimg)) {
            //如果能提取出图片来
            cimg->procStartTime = clock(); //标记处理开始时间

            fps = _fpsCalc.update(++frameCount);
            if (frameCount != cimg->fnum) {
                LogD("MultiCameraMT.workonce():有丢失帧，处理速度跟不上采图速度，frameCount=%d", frameCount);
                frameCount = cimg->fnum; //还是让两个帧号保持一致
            }

            //选一个proc进行图像的处理
            if (_activeProcIndex < vProc.size()) {
                LogD("MultiCameraMT.workonce():执行proc %d！", _activeProcIndex);
                int ckey = -1;
                vProc[_activeProcIndex]->process(cimg, ckey);
                if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                    Event::GetInst()->cvKey.exchange(ckey);
                }
            }

            //干脆用这个线程来驱动检查事件
            Event::GetInst()->checkMemEvent();

            cimg->procEndTime = clock(); //标记处理结束时间
        }
        else {
            break;
        }
    }
}

bool MultiCameraMT::openCamera(uint activeIndex, OpenCameraType openType)
{
    if (_isRun.load() || _isOpening.load()) {
        LogI("MultiCameraMT.openCamera():当前系统正在运行,需要先close(),函数直接返回...");
        return false;
    }
    BaseThread::GC(); //随便释放一下

    _isOpening.exchange(true); //标记正在打开了
    LogI("MultiCameraMT.openCamera():开始打开相机...");

    //根据当前录入的相机的东西里的设置来打开相机
    std::map<int, pCamera>& camMap = CameraManger::GetInst()->camMap;

    cameraThread = new CameraThread(camMap); //这里重新new
    cameraThread->cv_mt = &cv_mt;
    cameraThread->mtx_ct = &mtx_ct;
    cameraThread->cv_ct = &cv_ct;

    //设置当前的帧处理
    this->setActiveProc(activeIndex);

    //如果成功打开了所有相机，启动计算
    if (cameraThread->open()) {

        if (openType == OpenCameraType::StartCalcThread ||
            openType == OpenCameraType::ForceStartCalcThread) {
            LogI("MultiCameraMT.openCamera():相机打开完成！创建计算线程。");
            //综合分析计算线程
            this->_thread = BaseThread::creat(std::bind(&MultiCameraMT::init, this, std::placeholders::_1),
                                              std::bind(&MultiCameraMT::workonce, this, std::placeholders::_1),
                                              std::bind(&MultiCameraMT::release, this, std::placeholders::_1));
        }
        else if (openType == OpenCameraType::NotStartCalcThread) {
            LogI("MultiCameraMT.openCamera():相机打开完成！但不创建计算线程。");
        }
        _isRun.exchange(true);
        _isOpening.exchange(false);
        return true;
    }
    else {
        //如果相机打开失败
        LogE("MultiCameraMT.openCamera():相机打开失败！");
        if (openType == OpenCameraType::ForceStartCalcThread) {
            LogI("MultiCameraMT.openCamera():强制创建计算线程...");
            //综合分析计算线程
            /*              this->_thread = BaseThread::creat(std::bind(&MultiCameraMT::init, this, std::placeholders::_1),
                                                                std::bind(&MultiCameraMT::workonce, this, std::placeholders::_1),
                                                                std::bind(&MultiCameraMT::release, this, std::placeholders::_1));*/
        }
        _isRun = false;
        _isOpening = false;
        return false;
    }
}

void MultiCameraMT::close(bool isDeleteProc)
{
    if (!_isRun.load() || _isStopping.load()) {
        LogI("MultiCameraMT.close():已经停止或正在停止，函数直接返回...");
        return;
    }
    _isStopping = true;

    //停止自己的线程
    cv_mt.notify_all(); //激活一下锁

    if (this->_thread != nullptr) {

        //标记自己已经停止
        LogI("MultiCameraMT.close():执行close()");
        this->_thread->stop();
        this->_thread = nullptr; //这里不要急着干掉引用,不然引起线程自己释放自己...

        LogI("MultiCameraMT.close():综合分析线程已经关闭！");
    }
    //释放采图线程
    LogI("MultiCameraMT.close():释放采图线程...");
    cameraThread->close();
    delete cameraThread;
    cameraThread = nullptr;

    //重置这些状态吧
    frameCount = 0;
    fps = 0;
    _fpsCalc.reset();

    if (isDeleteProc) {
        LogI("MultiCameraMT.close():释放所有proc...");
        vProc.clear();
    }

    _isRun = false;
    _isStopping = false;
    LogI("MultiCameraMT.close():终于执行完了,close()返回...");
}

void MultiCameraMT::addProc(const pFrameProc& proc)
{
    this->vProc.push_back(proc);
}

void MultiCameraMT::addProc(FrameProc* proc)
{
    this->vProc.push_back(pFrameProc(proc));
}

void MultiCameraMT::setActiveProc(uint index)
{
    //这个函数现在做了修改不再由外部线程去执行enable和disable
    if (index < vProc.size()) {
        if (index == _activeProcIndex) { //如果相等就是重置
            _isResetActiveProc = true;
        }
        else { //如果不相等就是重设一个active的proc
            _nextActiveProcIndex = index;
        }
    }
}

void MultiCameraMT::setIsGrab(bool isGrab)
{
    if (this->cameraThread != nullptr) { //如果不为空
        cameraThread->isGrab = isGrab;
    }
}

} // namespace dxlib