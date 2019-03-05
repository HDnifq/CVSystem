#include "CameraThread.h"
#include <Chrono>
#include <boost/timer.hpp>
#include <memory>
#include "../Common/Common.h"
#include "DevicesHelper.h"
#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"

namespace dxlib {

#pragma region 队列实现

struct CameraThread::QueueData
{
    /// <summary> 采集的帧队列. </summary>
    moodycamel::ConcurrentQueue<pCameraImage> frameQueue;
};

bool CameraThread::try_dequeue(pCameraImage& cimg)
{
    return this->queueData->frameQueue.try_dequeue(cimg);
}

#pragma endregion

CameraThread::CameraThread(const pCamera& cp)
{
    this->queueData = new QueueData();

    if (!cp->isVirtualCamera)
        vCameras.push_back(cp);
}

CameraThread::CameraThread(const std::vector<pCamera>& cps)
{
    this->queueData = new QueueData();

    for (auto& item : cps) {
        if (!item->isVirtualCamera)
            vCameras.push_back(item);
    }
}

CameraThread::CameraThread(const std::map<int, pCamera>& cps)
{
    this->queueData = new QueueData();

    //使用这个那么没有办法保证一个vCameras的顺序等于index
    for (auto& kvp : cps) {
        if (!kvp.second->isVirtualCamera)
            vCameras.push_back(kvp.second);
    }
}

CameraThread::~CameraThread()
{
    if (this->_thread != nullptr)
        close();
    delete this->queueData;
}

void CameraThread::dowork()
{
    isThreadRunning = true;

    //等待外面主线程一声令下大家一起开始采
    if (mtx_ct != nullptr) { //如果有设置过锁才等
        std::unique_lock<std::mutex> lck(*mtx_ct);

        LogI("CameraThread.dowork():Camera线程id= %d 在等待外面启动通知嗯...", std::this_thread::get_id());
        isThreadWaitingStart = true; //标记线程是否在工作（放在这里防止外面过早通知里面解锁）
        cv_ct->wait(lck);
        LogI("CameraThread.dowork():收到通知,线程启动啦,开始采图!");
    }
    else {
        LogI("CameraThread.dowork():没有设置等待直接启动啦,开始采图!");
    }
    isThreadWaitingStart = false; //已经开始工作了

    //重置然后开始
    reset();

    while (!isStop.load()) {

        //执行完毕之后就更新相机的属性状态
        for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
            vCameras[camIndex]->applyCapProp();
        }

        if (this->isGrab.load()) { //如果采图(用于睡眠模式的控制)
            //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
            ++fnumber;

            //采图：一个结构体包含4个相机的图
            pCameraImage cimg(new CameraImage());
            cimg->fnum = fnumber;
            cimg->grabStartTime = clock();
            cimg->vImage.resize(vCameras.size()); //先直接创建算了
            //这个线程对所有相机采图
            bool isHavaImg = false;
            for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
                try {
                    cimg->vImage[camIndex].camera = vCameras[camIndex].get(); //标记camera来源

                    if (!vCameras[camIndex]->isOpened()) {
                        LogE("CameraThread.dowork():cam %d 相机没有打开！", camIndex);
                        continue;
                    }
                    cimg->vImage[camIndex].grabStartTime = clock();
                    if (vCameras[camIndex]->capture->read(cimg->vImage[camIndex].image)) {
                        isHavaImg = true;
                        cimg->vImage[camIndex].grabEndTime = clock();
                        LogD("CameraThread.dowork():cam %d 采图完成！fnumber=%d", vCameras[camIndex]->camIndex, fnumber);
                    }
                    else {
                        LogE("CameraThread.dowork():cam %d 采图read失败！", camIndex);
                    }
                }
                catch (const std::exception& e) {
                    LogW("CameraThread.dowork():异常 %s", e.what());
                }
            }
            cimg->grabEndTime = clock();

            queueData->frameQueue.enqueue(cimg);
            limitQueue(this->queueMaxLen);

            updateFPS();

            //如果有定义需要一个通知外面处理线程采图ok的话
            if (cv_mt != nullptr)
                cv_mt->notify_one();

            //如果没有一个相机采到图
            if (isHavaImg == false) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        else { //如果不采图就睡眠200ms
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            //就算不采图那么也还是传出去一下，如果有定义需要一个通知外面处理线程采图ok的话
            if (cv_mt != nullptr)
                cv_mt->notify_one();
        }
    }

    isThreadRunning = false;
}

bool CameraThread::open(bool isRunInError)
{
    if (this->_thread != nullptr) { //重复打开就直接返回
        LogW("CameraThread.open():重复打开相机，采图线程正在工作，不做操作直接返回！");
        return false;
    }
    if (vCameras.size() == 0) {
        LogW("CameraThread.open():事先没有录入有效的相机（vCameras.size()=0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        LogI("CameraThread.open():尝试打开相机 %s ...", vCameras[camIndex]->devNameA.c_str());
        boost::timer t;

        //打开相机
        if (vCameras[camIndex]->openCamera()) {
            double costTime = t.elapsed();

            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            vCameras[camIndex]->capture->read(img);
            //this->_thread->detach();
            LogI("CameraThread.open():成功打开一个相机%s，耗时%.2f秒", vCameras[camIndex]->devNameA.c_str(), costTime); //打开相机大致耗时0.2s
        }
        else {
            isSuccess = false;
        }
    }
    //如果相机都打开了，才会去启动线程
    if (isSuccess || isRunInError) { //通常来说不在打不开相机的情况下工作
        if (!isSuccess) {
            LogW("CameraThread.open():有相机打开失败，但是设置了仍然启动采图线程！");
        }
        else {
            LogI("CameraThread.open():采图线程启动！");
        }
        this->isStop.exchange(false); //标记启动
        this->isGrab.exchange(true);  //标记采图
        this->_thread = new std::thread(&CameraThread::dowork, this);
    }
    else {
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

        LogI("CameraThread.close():采图线程已经停止！");
    }

    for (size_t i = 0; i < vCameras.size(); i++) {
        LogI("CameraThread.close():释放相机%s ...", vCameras[i]->devNameA.c_str());
        vCameras[i]->releaseCamera();
    }

    reset(); //清空队列
    return true;
}

#pragma region 私有函数

void CameraThread::updateFPS()
{
    clock_t now = clock();
    double costTime = (double)(now - _lastTime) / CLOCKS_PER_SEC;
    if (costTime > 1.0) { //如果经过了1秒
        //FPS写入到了camera
        for (size_t i = 0; i < vCameras.size(); i++) {
            vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位
        }

        _lastTime = now;        //记录现在的最近一次的时间
        _lastfnumber = fnumber; //记录现在最近的帧数
    }
    else if (costTime > 0.5) { //在0.5秒之后可以开始计算了
        //FPS写入到了camera
        for (size_t i = 0; i < vCameras.size(); i++) {
            vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位
        }
    }
}

void CameraThread::reset()
{
    fnumber = 0;
    _lastTime = clock(); //记录现在的最近一次的时间
    _lastfnumber = 0;    //记录现在最近的帧数

    pCameraImage head;
    while (queueData->frameQueue.try_dequeue(head)) {
    }
}

void CameraThread::limitQueue(uint maxlen)
{
    if (queueData->frameQueue.size_approx() > 2) {
        //LogI("CameraThread.limitQueue():处理速度有点没跟上... QueueLen=%d", frameQueue.size_approx());
    }
    while (queueData->frameQueue.size_approx() > maxlen) {
        pCameraImage head;
        queueData->frameQueue.try_dequeue(head);
        LogD("CameraThread.limitQueue():进行了一帧的丢弃！");
    }
}

#pragma endregion
} // namespace dxlib