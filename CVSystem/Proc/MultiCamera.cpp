#include "MultiCamera.h"

#include "DevicesHelper.h"
#include "iostream"
#include <map>
#include <windows.h>
#include "../Common/Common.h"
#include "../Common/CoolTime.hpp"
#include "../dlog/dlog.h"
#include "Camera.h"

//都是c11的
#include <Chrono>
#include <thread>

namespace dxlib {

    MultiCamera::MultiCamera()
    {

    }

    MultiCamera::~MultiCamera()
    {
        release();
    }

    MultiCamera* MultiCamera::m_pInstance = NULL;

    MultiCamera* MultiCamera::GetInst()
    {
        if (m_pInstance == NULL) m_pInstance = new MultiCamera();
        return m_pInstance;
    }

    //线程函数
    void MultiCamera::run()
    {
        std::unique_lock<std::mutex> lck(mtx_mt);
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
                if (cameraThread->frameQueue.try_dequeue(cimg)) {
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
                            key.exchange(ckey);
                        }
                        //如果有定义按键事件响应那么就执行这个函数（一般不使用这个）
                        //if (procKeyEvent != nullptr) {
                        //    procKeyEvent(this, key);
                        //}
                    }
                } else {
                    break;
                }
            }
        }
    }

    void MultiCamera::openCamera(uint activeIndex, bool isStartThread)
    {
        //根据当前录入的相机的东西里的设置来打开相机
        auto camMap = CameraManger::GetInst()->camMap;

        cameraThread = new CameraThread(camMap);//这里重新new
        cameraThread->cv_mt = &cv_mt;
        cameraThread->mtx_ct = &mtx_ct;
        cameraThread->cv_ct = &cv_ct;

        if (cameraThread->open()) { //如果成功打开了所有相机，启动相机
            isStop.exchange(false);

            //设置当前的帧处理
            this->setActiveProc(activeIndex);

            if(isStartThread) {
                LogI("MultiCamera.openCamera():相机打开完成！isStartThread=true,创建计算线程。");
                //综合分析计算线程
                this->_thread = new std::thread(&MultiCamera::run, this);
            } else {
                LogI("MultiCamera.openCamera():相机打开完成！isStartThread=false,不创建计算线程。");
            }
        } else {
            LogE("MultiCamera.openCamera():相机打开失败！");
        }
    }

    void MultiCamera::release(bool isDeleteProc)
    {
        if (isStop.load() == true || cameraThread == nullptr) {
            LogI("MultiCamera.release():已经停止，函数直接返回.");
            return;
        }

        //标记自己已经停止
        LogI("MultiCamera.release():标记自己停止...");
        isStop.exchange(true);

        //停止自己的线程
        cv_mt.notify_all();//激活一下锁

        if (this->_thread != nullptr) {
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

        fpsCalc.reset();

        if (isDeleteProc) {
            LogI("MultiCamera.release():释放所有proc...");
            vProc.clear();
        }
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