#include "MultiCamera.h"

#include "DevicesHelper.h"
#include "iostream"
#include <map>
#include <windows.h>
#include "../Common/Common.h"
#include "../Common/CoolTime.hpp"
#include "../dlog/dlog.h"
#include "CameraManger.h"
#include "Event.h"
#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"
//都是c11的
#include <Chrono>
#include <thread>

namespace dxlib {

    struct MultiCamera::ReleaseInfo
    {
        /// <summary> 是否有必要检查这个release. </summary>
        std::atomic_bool isNeedCheck = false;

        /// <summary> 需要退出的线程队列. </summary>
        moodycamel::ConcurrentQueue<std::thread::id> needStopQueue;
    };

    MultiCamera::MultiCamera()
    {
        _releaseInfo = new ReleaseInfo();
    }

    MultiCamera::~MultiCamera()
    {
        release();
        delete _releaseInfo;
    }

    MultiCamera* MultiCamera::m_pInstance = NULL;

    //线程函数
    void MultiCamera::run()
    {
        std::unique_lock<std::mutex> lck(mtx_mt);
        while (cameraThread->isHasThread()) {//如果存在采图线程
            if (cameraThread->isThreadWaitingStart.load() == true) {
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));//等待相机线程进入了线程函数并且在等待了
            }

        }
        cv_ct.notify_all();//一声令下小线程们同时开始工作

        while (!isStop.load()) {
            LogD("MultiCamera.run():进入睡眠...");
            //反正基本就是等待小线程们的通知
            cv_mt.wait(lck);

            while (true) {
                //响应退出
                if (isStop.load()) {
                    //退出前disable一下
                    if(vProc.size() > activeProcIndex)//防止用户没有加入proc的情况，还是应该判断一下
                        vProc[activeProcIndex]->onDisable();
                    return;
                }

                //重设激活的proc
                if (_nextActiveProcIndex != activeProcIndex) { //如果变化了(这里或许需要原子操作)
                    if (vProc.size() > activeProcIndex)
                        vProc[activeProcIndex]->onDisable();
                    activeProcIndex = _nextActiveProcIndex;
                    if (vProc.size() > activeProcIndex)
                        vProc[activeProcIndex]->onEnable();
                }
                //重启当前的proc
                if (_isResetActiveProc) {
                    _isResetActiveProc = false;
                    if (vProc.size() > activeProcIndex) {
                        vProc[activeProcIndex]->onDisable();
                        vProc[activeProcIndex]->onEnable();
                    }
                }

                LogD("MultiCamera.run():在队列里提取图片！");
                //提取图片组帧
                pCameraImage cimg;
                if (cameraThread->try_dequeue(cimg)) {
                    //如果能提取出图片来
                    LogD("MultiCamera.run():提取%d个相机图像 fnum = %d , costTime=%2f", cimg->vImage.size(), cimg->fnum, cimg->costTime);
                    fps = fpsCalc.update(++frameCount);
                    if (frameCount != cimg->fnum) {
                        LogD("MultiCamera.run():有丢失帧，处理速度跟不上采图速度，frameCount=%d", frameCount);
                        frameCount = cimg->fnum;//还是让两个帧号保持一致
                    }

                    //选一个proc进行图像的处理
                    if (activeProcIndex < vProc.size()) {
                        int ckey = vProc[activeProcIndex]->process(cimg);
                        if (ckey != -1) {//如果有按键按下那么修改最近的按键值
                            Event::GetInst()->cvKey.exchange(ckey);
                        }
                    }

                    //干脆用这个线程来驱动检查事件
                    Event::GetInst()->checkMemEvent();

                    //检查一下自己的id在不在这个记录里
                    if (_releaseInfo->isNeedCheck.load()) {
                        std::thread::id ntid;
                        if (_releaseInfo->needStopQueue.try_dequeue(ntid)) {
                            if (ntid == std::this_thread::get_id()) {
                                LogI("MultiCamera.run():计算线程id=%d发现了自己应该返回了,return.", std::this_thread::get_id());
                                if (vProc.size() > activeProcIndex)//防止用户没有加入proc的情况，还是应该判断一下
                                    vProc[activeProcIndex]->onDisable();
                                if (_releaseInfo->needStopQueue.size_approx() == 0) {
                                    _releaseInfo->isNeedCheck = false;
                                }
                                return;
                            } else {
                                _releaseInfo->needStopQueue.enqueue(ntid);
                            }
                        }
                    }
                } else {
                    break;
                }
            }
        }

        LogI("MultiCamera.run():计算线程id=%d执行函数已经返回.", std::this_thread::get_id());
    }

    bool MultiCamera::openCamera(uint activeIndex, OpenCameraType openType)
    {
        if (cameraThread != nullptr || this->_thread != nullptr) {
            LogI("MultiCamera.openCamera():当前系统正在运行,需要先release(),函数直接返回...");
            return false;
        }

        //根据当前录入的相机的东西里的设置来打开相机
        std::map<int, pCamera>& camMap = CameraManger::GetInst()->camMap;

        cameraThread = new CameraThread(camMap);//这里重新new
        cameraThread->cv_mt = &cv_mt;
        cameraThread->mtx_ct = &mtx_ct;
        cameraThread->cv_ct = &cv_ct;

        isStop.exchange(false);
        //设置当前的帧处理
        this->setActiveProc(activeIndex);

        //如果成功打开了所有相机，启动计算
        if (cameraThread->open()) {

            if(openType == OpenCameraType::StartCalcThread ||
                    openType == OpenCameraType::ForceStartCalcThread) {
                LogI("MultiCamera.openCamera():相机打开完成！创建计算线程。");
                //综合分析计算线程
                this->_thread = new std::thread(&MultiCamera::run, this);
            } else if(openType == OpenCameraType::NotStartCalcThread) {
                LogI("MultiCamera.openCamera():相机打开完成！但不创建计算线程。");
            }
            return true;

        } else {
            //如果相机打开失败
            LogE("MultiCamera.openCamera():相机打开失败！");
            if (openType == OpenCameraType::ForceStartCalcThread) {
                LogI("MultiCamera.openCamera():强制创建计算线程...");
                //综合分析计算线程
                this->_thread = new std::thread(&MultiCamera::run, this);
            }
            return false;
        }
    }

    void MultiCamera::release(bool isDeleteProc)
    {
        if (isStop.load() == true || cameraThread == nullptr) {
            LogI("MultiCamera.release():已经停止，函数直接返回.");
            return;
        }

        isStop.exchange(true);

        //停止自己的线程
        cv_mt.notify_all();//激活一下锁

        if (this->_thread != nullptr) {

            //标记自己已经停止
            LogI("MultiCamera.release():标记自己停止...执行release()线程id=%d,计算线程id=%d", std::this_thread::get_id(), this->_thread->get_id());
        
            //添加一条记录，表示这个id的线程需要退出哦
            if (std::this_thread::get_id() == this->_thread->get_id()) {
                _releaseInfo->isNeedCheck = true;
                _releaseInfo->needStopQueue.enqueue(this->_thread->get_id());
            }

            if (this->_thread->joinable() && std::this_thread::get_id() != this->_thread->get_id()) {
                LogI("MultiCamera.release():当前线程进入join()");
                this->_thread->join();//等待综合分析线程退出
            }
            if(std::this_thread::get_id() != this->_thread->get_id()) {
                LogI("MultiCamera.release():delete 综合分析线程...");
                delete this->_thread;
            }
            this->_thread = nullptr;

            LogI("MultiCamera.release():综合分析线程已经关闭！");
        }
        //释放采图线程
        LogI("MultiCamera.release():释放采图线程...");
        cameraThread->close();
        delete cameraThread;
        cameraThread = nullptr;

        //重置这些状态吧
        frameCount = 0;
        fps = 0;
        fpsCalc.reset();

        if (isDeleteProc) {
            LogI("MultiCamera.release():释放所有proc...");
            vProc.clear();
        }
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
            if (index == activeProcIndex) {//如果相等就是重置
                _isResetActiveProc = true;
            } else {//如果不相等就是重设一个active的proc
                _nextActiveProcIndex = index;
            }
        }
    }

    void MultiCamera::setIsGrab(bool isGrab)
    {
        if (this->cameraThread != nullptr) { //如果不为空
            cameraThread->isGrab = isGrab;
        }
    }

}