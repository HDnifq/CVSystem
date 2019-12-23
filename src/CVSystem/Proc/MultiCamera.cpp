﻿#include "MultiCamera.h"
#include "iostream"
#include <map>
#include "../Common/StringHelper.h"
#include "../Common/CoolTime.hpp"
#include "../Common/Event.h"
#include "CameraManger.h"

namespace dxlib {

MultiCamera::MultiCamera()
{
}

MultiCamera::~MultiCamera()
{
    close();
}

MultiCamera* MultiCamera::m_pInstance = NULL;

/// <summary> 提供给BaseThread的init执行事件. </summary>
void MultiCamera::init(std::shared_ptr<BaseThread>& tb)
{
    LogI("MultiCamera.init():开始执行init委托!");
    LogI("MultiCamera.init():init委托结束!");
}

/// <summary> 提供给BaseThread的release执行事件. </summary>
void MultiCamera::release(std::shared_ptr<BaseThread>& tb)
{
    LogI("MultiCamera.release():开始执行release委托!");

    if (vProc.size() > _activeProcIndex) //防止用户没有加入proc的情况，还是应该判断一下
        vProc[_activeProcIndex]->onDisable();

    //执行一次关闭
    close(false);
    LogI("MultiCamera.release():release委托执行完毕.");
}

/// <summary> 提供给BaseThread的bing执行事件. </summary>
void MultiCamera::workonce(std::shared_ptr<BaseThread>& tb)
{
    while (true) {
        //如果这个线程已经是处在要关闭状态了，那么这个函数就赶紧退出吧
        if (!tb->isRun()) {
            return;
        }

        //重设激活的proc
        if (_nextActiveProcIndex != _activeProcIndex) { //如果变化了(这里或许需要原子操作)‘
            LogD("MultiCamera.workonce():执行设置_activeProcIndex");
            if (vProc.size() > _activeProcIndex)
                vProc[_activeProcIndex]->onDisable();
            _activeProcIndex = _nextActiveProcIndex;
            if (vProc.size() > _activeProcIndex)
                vProc[_activeProcIndex]->onEnable();
        }
        //重启当前的proc
        if (_isResetActiveProc) {
            _isResetActiveProc = false;
            LogD("MultiCamera.workonce():执行重启当前的proc");
            if (vProc.size() > _activeProcIndex) {
                vProc[_activeProcIndex]->onDisable();
                vProc[_activeProcIndex]->onEnable();
            }
        }

        LogD("MultiCamera.workonce():在队列里提取图片！");

        if (_isGrab.load()) {
            //提取图片组帧
            pCameraImage cimg;
            if (_cameraGrab.grab(cimg)) {
                //如果采图成功了
                cimg->procStartTime = clock(); //标记处理开始时间

                fps = _fpsCalc.update(++frameCount);

                if (frameCount != cimg->fnum) {
                    LogD("MultiCamera.workonce():有丢失帧，处理速度跟不上采图速度，frameCount=%d", frameCount);
                    frameCount = cimg->fnum; //还是让两个帧号保持一致
                }

                //选一个proc进行图像的处理
                if (_activeProcIndex < vProc.size()) {
                    LogD("MultiCamera.workonce():执行%d号proc ！", _activeProcIndex);
                    int ckey = -1; //让proc去自己想检测keydown就keydown
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
                LogE("MultiCamera.workonce():采图失败了！");
                break;
            }
        }
        else {
            //姑且也检查一下事件
            Event::GetInst()->checkMemEvent();

            //选一个proc进行图像的处理
            if (_activeProcIndex < vProc.size()) {
                LogD("MultiCamera.workonce():执行%d号proc ！", _activeProcIndex);
                int ckey = -1; //让proc去自己想检测keydown就keydown
                vProc[_activeProcIndex]->onLightSleep(ckey);
                if (ckey != -1) { //如果有按键按下那么修改最近的按键值
                    Event::GetInst()->cvKey.exchange(ckey);
                }
            }

            //如果不抓图那么就睡眠
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
}

bool MultiCamera::openCamera(uint activeIndex)
{
    if (_isRun.load() || _isOpening.load()) {
        LogI("MultiCamera.openCamera():当前系统正在运行,需要先close(),函数直接返回...");
        return false;
    }
    BaseThread::GC(); //随便释放一下

    _isOpening.exchange(true); //标记正在打开了
    LogI("MultiCamera.openCamera():开始打开相机...");

    //根据当前录入的相机的东西里的设置来打开相机
    _cameraGrab.setCameras(CameraManger::GetInst()->camMap);
    _cameraGrab.setCamerasAssist(CameraManger::GetInst()->camMapAssist);

    if (this->vProc.size() == 0) {
        LogW("MultiCamera.openCamera():当前没有添加处理proc...");
    }

    //设置当前的帧处理
    this->setActiveProc(activeIndex);

    bool isSuccess = _cameraGrab.open();
    if (isSuccess) {
        LogI("MultiCamera.openCamera():cameraGrab相机打开成功！");
    }
    else {
        LogE("MultiCamera.openCamera():cameraGrab相机打开有失败的相机!");
    } //综合分析计算线程
    LogI("MultiCamera.openCamera():创建综合分析计算线程！");
    this->_thread = BaseThread::creat(std::bind(&MultiCamera::init, this, std::placeholders::_1),
                                      std::bind(&MultiCamera::workonce, this, std::placeholders::_1),
                                      std::bind(&MultiCamera::release, this, std::placeholders::_1));

    this->_thread->setPriorityHigh();
    _isRun.exchange(true);
    _isOpening.exchange(false);

    return isSuccess;
}

void MultiCamera::close(bool isDeleteProc)
{
    if (!_isRun.load() || _isStopping.load()) {
        LogI("MultiCamera.close():已经停止或正在停止，函数直接返回...");
        return;
    }
    _isStopping = true;

    //停止自己的线程
    if (this->_thread != nullptr) {

        //标记自己已经停止
        LogI("MultiCamera.close():执行close()");
        this->_thread->stop();
        this->_thread = nullptr; //这里不要急着干掉引用,不然引起线程自己释放自己...

        LogI("MultiCamera.close():综合分析线程已经关闭！");
    }
    //释放采图
    LogI("MultiCamera.close():释放采图...");
    _cameraGrab.close();

    //重置这些状态吧
    frameCount = 0;
    fps = 0;
    _fpsCalc.reset();

    if (isDeleteProc) {
        LogI("MultiCamera.close():释放所有proc...");
        vProc.clear();
    }

    _isRun = false;
    _isStopping = false;
    LogI("MultiCamera.close():终于执行完了,close()返回...");
}

void MultiCamera::addProc(const pFrameProc& proc)
{
    this->vProc.push_back(proc);
}

void MultiCamera::addProc(FrameProc* proc)
{
    this->vProc.push_back(pFrameProc(proc));
}

void MultiCamera::setActiveProc(uint index)
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
} // namespace dxlib