﻿#include "CameraThread.h"
#include <Chrono>
#include <boost/timer.hpp>
#include "../dlog/dlog.h"
#include "../Common/Common.h"
#include "DevicesHelper.h"

namespace dxlib {

    ImageItem* CameraImage::getItem(int camIndex)
    {
        //如果vector的index是能够对应上的，那么就直接返回这一项
        if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
            return &vImage[camIndex];
        }

        //如果对应不上那么只能遍历搜索
        for (size_t i = 0; i < vImage.size(); i++) {
            if (vImage[i].camera->camIndex == camIndex) {
                return &vImage[i];
            }
        }
        LogE("CameraImage.getItem():输入的camIndex %d 未能查找到!", camIndex);
        return nullptr;
    }

    CameraThread::CameraThread(const pCamera& cp)
    {
        vCameras.push_back(cp);
    }

    CameraThread::CameraThread(const std::vector<pCamera>& cps)
    {
        vCameras = cps;
    }

    CameraThread::CameraThread(const std::map<int, pCamera>& cps)
    {
        //使用这个那么没有办法保证一个vCameras的顺序等于index
        for (auto& kvp : cps) {
            vCameras.push_back(kvp.second);
        }
    }

    CameraThread::~CameraThread()
    {
        close();
    }

    void CameraThread::dowork()
    {
        //等待外面主线程一声令下大家一起开始采
        if(mtx_ct != nullptr) {         //如果有设置过才等
            LogI("CameraThread.dowork():线程id= %d 在等待一声令下开始", std::this_thread::get_id());
            std::unique_lock<std::mutex> lck(*mtx_ct);
            cv_ct->wait(lck);
        }

        //重置然后开始
        reset();

        while (!isStop.load()) {
            if(this->isGrab.load()) {//如果采图(用于睡眠模式的控制)
                //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
                ++fnumber;

                pCameraImage cimg(new CameraImage());//一个结构体包含4个相机的图
                cimg->fnum = fnumber;
                cimg->startTime = clock();
                cimg->vImage.resize(vCameras.size());//先直接创建算了
                //这个线程对所有相机采图
                bool isHavaImg = false;
                for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
                    try {
                        cimg->vImage[camIndex].camera = vCameras[camIndex].get();//标记camera来源

                        if (!vCameras[camIndex]->capture->isOpened()) {
                            LogE("CameraThread.dowork():cam %d 相机没有打开！", camIndex);
                            continue;
                        }
                        if (vCameras[camIndex]->capture->read(cimg->vImage[camIndex].image)) {
                            isHavaImg = true;
                            LogD("CameraThread.dowork():cam %d 采图完成！fnumber=%d", vCameras[camIndex]->camIndex, fnumber);
                        } else {
                            LogE("CameraThread.dowork():cam %d 采图read失败！", camIndex);
                        }
                    } catch (const std::exception& e) {
                        LogW("CameraThread.dowork():异常 %s", e.what());
                    }
                }

                cimg->costTime = (float)(clock() - cimg->startTime) / CLOCKS_PER_SEC;//计算一个采图消耗时间
                frameQueue.enqueue(cimg);

                limitQueue(this->queueMaxLen);
                updateFPS();

                //如果有定义需要一个通知外面处理线程采图ok的话
                if (cv_mt != nullptr)
                    cv_mt->notify_one();

                //如果没有一个相机采到图
                if (isHavaImg == false) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

            } else {//如果不采图就睡眠200ms
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                //就算不采图那么也还是传出去一下，如果有定义需要一个通知外面处理线程采图ok的话
                if (cv_mt != nullptr)
                    cv_mt->notify_one();
            }
        }
    }

    bool CameraThread::open(bool isRunInError)
    {
        if (this->_thread != nullptr) { //重复打开就直接返回
            LogW("CameraThread.open():重复打开相机，采图线程正在工作，不做操作直接返回！");
            return false;
        }
        bool isSuccess = true;
        for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
            LogI("CameraThread.open():尝试打开相机%s...", ws2s(vCameras[camIndex]->devName));
            boost::timer t;
            if (openCamera(vCameras[camIndex])) {
                double costTime = t.elapsed();

                //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
                cv::Mat img;
                vCameras[camIndex]->capture->read(img);
                //this->_thread->detach();
                LogI("CameraThread.open():成功打开一个相机%s，耗时%.3f秒", ws2s(vCameras[camIndex]->devName), costTime);//打开相机大致耗时0.2s
            } else {
                isSuccess = false;
            }
        }
        if(isSuccess || isRunInError) {//通常来说不在打不开相机的情况下工作
            if (!isSuccess) {
                LogW("CameraThread.open():有相机打开失败，但是设置了仍然启动采图线程！");
            } else {
                LogI("CameraThread.open():采图线程启动！");
            }
            this->isStop.exchange(false);//标记启动
            this->isGrab.exchange(true);//标记采图
            this->_thread = new std::thread(&CameraThread::dowork, this);
        } else {
            LogW("CameraThread.open():相机打开失败，isRunInError定义为false，所以未启动采图线程！");
        }
        return isSuccess;
    }

    bool CameraThread::close()
    {
        this->isStop.exchange(true);

        //等待线程执行完毕
        if (this->_thread != nullptr) {
            if (this->_thread->joinable()) {
                this->_thread->join();
            }
            delete this->_thread;
            this->_thread = nullptr;

            LogI("CameraThread.close():采图线程已经停止%s！");
        }

        for (size_t i = 0; i < vCameras.size(); i++) {
            if(vCameras[i]->capture != nullptr) {
                vCameras[i]->capture->release();
            }
            if (vCameras[i] != nullptr) {
                vCameras[i]->FPS = 0;
            }
        }

        reset();//清空队列
        return true;
    }

    #pragma region 私有函数

    bool CameraThread::openCamera(pCamera& cameraParam)
    {
        //如果存在VideoCapture那么就释放
        if (cameraParam->capture != nullptr) {
            cameraParam->capture->release();
        }
        cameraParam->capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture());

        //列出设备
        DevicesHelper::GetInst()->listDevices();
        cameraParam->devID = DevicesHelper::GetInst()->getIndexWithName(cameraParam->devName);//记录devID
        if (cameraParam->devID != -1) { //如果打开失败会返回-1

            // In case a resource was already
            // associated with the VideoCapture instance
            cameraParam->capture->release();

            int count = 0;
            while (count < 3) { //重试3次
                try {
                    //调用opencv的打开相机
                    if (cameraParam->capture->open(cameraParam->devID)) {
                        //打开成功的话就设置相机
                        if (!cameraParam->capture->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'))) {
                            LogE("CameraThread.openCamera(): Set FOURCC fail");
                        }
                        if (!cameraParam->capture->set(CV_CAP_PROP_BRIGHTNESS, cameraParam->brightness)) {
                            LogE("CameraThread.openCamera(): Set BRIGHTNESS fail");
                            //设置亮度是不能容忍失败的
                            cameraParam->capture->release();//关闭相机
                            continue;
                        }
                        if (!cameraParam->capture->set(CV_CAP_PROP_FPS, 50)) {
                            LogE("CameraThread.openCamera(): Set FPS fail");
                        }
                        if (!cameraParam->capture->set(CV_CAP_PROP_FRAME_HEIGHT, cameraParam->size.height)) {
                            LogE("CameraThread.openCamera(): 设置分辨率HEIGHT失败");
                        }
                        if (!cameraParam->capture->set(CV_CAP_PROP_FRAME_WIDTH, cameraParam->size.width)) {
                            LogE("CameraThread.openCamera(): 设置分辨率WIDTH失败");
                        }
                        break;//打开成功
                    } else {
                        LogW("CameraThread.openCamera():打开摄像头失败，重试。。。");
                    }
                } catch (const std::exception& e) {
                    LogE("CameraThread.openCamera():打开摄像头异常:%s ", e.what());
                }
                count++;

                cameraParam->capture->release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));//休眠1000ms

            }

            if (count == 3) {//失败超过3次
                LogE("CameraThread.openCamera():打开摄像头失败!");
                return false;
            }

            int w = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_FRAME_WIDTH));
            int h = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_FRAME_HEIGHT));
            int fps = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_FPS));
            int mode = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_MODE));
            int ex = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_FOURCC));
            char fourcc[] = { (char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), 0 };

            int brightness = static_cast<int>(cameraParam->capture->get(CV_CAP_PROP_BRIGHTNESS));

            LogI("CameraThread.openCamera()相机当前w =%d h=%d fps=%d mode=%d fourcc=%s brightness=%d", w, h, fps, mode, fourcc, brightness);

            return true;
        } else {
            LogE("CameraThread.openCamera():列出摄像头失败，相机数为0!");
            return false;
        }
    }

    void CameraThread::updateFPS()
    {
        clock_t now = clock();
        double costTime = (double)(now - _lastTime) / CLOCKS_PER_SEC;
        if (costTime > 1.0) { //如果经过了1秒
            //FPS写入到了cameraParam
            for (size_t i = 0; i < vCameras.size(); i++) {
                vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f;//这里把float截断后两位
            }

            _lastTime = now; //记录现在的最近一次的时间
            _lastfnumber = fnumber;//记录现在最近的帧数
        } else if (costTime > 0.5) { //在0.5秒之后可以开始计算了
            //FPS写入到了cameraParam
            for (size_t i = 0; i < vCameras.size(); i++) {
                vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f;//这里把float截断后两位
            }
        }
    }

    void CameraThread::reset()
    {
        fnumber = 0;
        _lastTime = clock(); //记录现在的最近一次的时间
        _lastfnumber = 0;//记录现在最近的帧数

        pCameraImage head;
        while (frameQueue.try_dequeue(head)) {
        }
    }

    void CameraThread::limitQueue(uint maxlen)
    {
        if (frameQueue.size_approx() > 2) {
            //LogI("CameraThread.limitQueue():处理速度有点没跟上... QueueLen=%d", frameQueue.size_approx());
        }
        while (frameQueue.size_approx() > maxlen) {
            pCameraImage head;
            frameQueue.try_dequeue(head);
            LogD("CameraThread.limitQueue():进行了一帧的丢弃！");
        }
    }

    #pragma endregion
}