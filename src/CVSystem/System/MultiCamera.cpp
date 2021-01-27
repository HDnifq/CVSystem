#include "MultiCamera.h"

#include "dlog/dlog.h"

#include "../Common/CoolTime.hpp"
#include "../Common/Event.h"
#include "../Common/FPSCalc.hpp"

#include "../Model/CameraDevice.h"
#include "../Model/Camera.h"
#include "../Model/ICameraImageFactory.h"

#include "../Model/CameraImageQueue.h"

#include "CameraManger.h"

#include <map>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RunnableAdapter.h"

#include "TaskGrabOneCamera.hpp"
#include "TaskGrabManyCamera.hpp"

namespace dxlib {

class MultiCamera::Impl
{
  public:
    Impl()
    {
    }
    ~Impl()
    {
    }

    // 一次只使能一个处理.
    std::vector<pFrameProc> vProc;

    // 当前激活的处理Index.
    uint _activeProcIndex = 0;

    // 相机(其中camIndex就等于这个vector的index).
    std::vector<pCamera> vCameras;

    // 相机硬件设备
    std::vector<pCameraDevice> vDevices;

    // 相机图片采集器
    std::vector<pCameraImageFactory> vCameraImageFactory;

    // 主相机采图.
    IGrabTask* mainGrabTask = nullptr;

    // 所有相机采图
    std::vector<IGrabTask*> vGrabTasks;

    // 采图队列
    CameraImageQueue imageQueue;

    // 工作线程池
    Poco::ThreadPool threadPool{4, 16};

    // 是否相机已经打开.
    std::atomic_bool _isOpened{false};

    // 是否正在打开相机.
    std::atomic_bool _isOpening{false};

    // 是否相机正在关闭.
    std::atomic_bool _isCloseing{false};

    // 是否正在停止.
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

MultiCamera* MultiCamera::m_pInstance = new MultiCamera();

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
    for (size_t i = 0; i < _impl->vGrabTasks.size(); i++) {
        _impl->vGrabTasks[i]->isGrab = isGrab;
    }
}

bool MultiCamera::isGrab()
{
    if (_impl->mainGrabTask != nullptr) {
        return _impl->mainGrabTask->isGrab.load();
    }
    return false;
}

bool MultiCamera::isCameraOpened()
{
    return _impl->_isOpened.load();
}

bool MultiCamera::isRunning()
{
    if (!_impl->vGrabTasks.empty())
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

    //拷贝CameraManger里的数据
    _impl->vCameras = CameraManger::GetInst()->vCamera;
    _impl->vDevices = CameraManger::GetInst()->vDevice;
    _impl->vCameraImageFactory = CameraManger::GetInst()->vCameraImageFactory;

    //初始化imageQueue
    _impl->imageQueue.Clear();
    for (size_t i = 0; i < _impl->vCameras.size(); i++) {
        _impl->imageQueue.AddCamera(_impl->vCameras[i].get());
    }

    //根据当前录入的相机的东西里的设置来打开相机

    if (_impl->vDevices.size() == 0) {
        LogW("MultiCamera.openCamera():事先没有录入有效的相机（vCameras.size()==0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < _impl->vDevices.size(); camIndex++) {
        auto& device = _impl->vDevices[camIndex];
        if (device == nullptr) {
            LogI("MultiCamera.openCamera():相机index%d 为null", camIndex);
            continue;
        }

        LogI("MultiCamera.openCamera():尝试打开相机 %s ...", device->devName.c_str());
        clock_t startTime = clock();
        //打开相机
        if (device->open()) {
            double costTime = (clock() - startTime) / CLOCKS_PER_SEC * 1000.;
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            device->capture->read(img);
            if (!img.empty())
                LogI("MultiCamera.openCamera():成功打开一个相机%s，耗时%.2f毫秒", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
            else
                LogE("MultiCamera.openCamera():成功打开一个相机%s，耗时%.2f毫秒,但是尝试读取一帧图片失败!", device->devName.c_str(), costTime); //打开相机大致耗时0.2s
        }
        else {
            isSuccess = false;
        }
    }

    _impl->_isOpened.exchange(true);
    _impl->_isOpening.exchange(false);

    return isSuccess;
}

void MultiCamera::closeCamera()
{
    if (this->isRunning()) {
        LogW("MultiCamera.closeCamera():当前计算线程正在执行，可能不应该就这样停止，函数直接返回...");
        return;
    }

    if (!_impl->_isOpened.load() || _impl->_isCloseing.load()) {
        LogI("MultiCamera.closeCamera():已经停止或正在停止，函数直接返回...");
        return;
    }
    _impl->_isCloseing.exchange(true);

    //释放采图任务
    LogI("MultiCamera.closeCamera():关闭相机...");
    for (size_t i = 0; i < _impl->vDevices.size(); i++) {
        if (_impl->vDevices[i] != nullptr) {
            LogI("MultiCamera.close():释放相机设备%s ...", _impl->vDevices[i]->devName.c_str());
            _impl->vDevices[i]->release();
        }
    }

    _impl->_isOpened.exchange(false);
    _impl->_isCloseing.exchange(false);
    LogI("MultiCamera.closeCamera():终于执行完了,close()返回...");
}

void MultiCamera::startMT(uint activeProcindex)
{
    if (this->isRunning()) {
        LogW("MultiCamera.startMT():当前计算线程正在执行,可能导致泄漏!");
    }

    if (_impl->vProc.empty()) {
        LogE("MultiCamera.startMT():启动失败，输vProc为空,需要先添加proc对象");
        return;
    }
    if (activeProcindex >= _impl->vProc.size()) {
        LogE("MultiCamera.startMT():启动失败，输入activeProcindex=%u过大,当前vProc的size为%zu!", activeProcindex, _impl->vProc.size());
        return;
    }
    _impl->_activeProcIndex = activeProcindex; //记录一下
    _impl->vGrabTasks.clear();

    LogI("MultiCamera.startMT():启动各个采图Task...有%d个Task", _impl->vCameraImageFactory.size());
    if (_impl->vCameraImageFactory.size() == 0) {
        LogE("MultiCamera.startMT():启动采图Task失败,未添加CameraImageFactory!可能程序未对接新接口...");
    }
    for (size_t i = 0; i < _impl->vCameraImageFactory.size(); i++) {
        auto& pCameraImageFactory = _impl->vCameraImageFactory[i];
        try {
            TaskGrabOneCamera* task = new TaskGrabOneCamera(pCameraImageFactory->device->devName,
                                                            &_impl->imageQueue,
                                                            pCameraImageFactory.get());
            //把第一个设为主采图.
            if (i == 0) {
                task->isMainTask = true;
                _impl->mainGrabTask = task;
            }

            task->isDoProc = true;
            task->proc = _impl->vProc[activeProcindex];

            _impl->vGrabTasks.push_back(task);
            _impl->threadPool.startWithPriority(Poco::Thread::Priority::PRIO_HIGHEST, *task);
        }
        catch (const std::exception& e) {
            LogE("MultiCamera.startMT():异常 %s", e.what());
        }
    }
}

void MultiCamera::start(uint activeProcindex)
{
    if (this->isRunning()) {
        LogW("MultiCamera.start():当前计算线程正在执行,可能导致泄漏!");
    }

    if (_impl->vProc.empty()) {
        LogE("MultiCamera.start():启动失败，输vProc为空,需要先添加proc对象");
        return;
    }
    if (activeProcindex >= _impl->vProc.size()) {
        LogE("MultiCamera.start():启动失败，输入activeProcindex=%u过大,当前vProc的size为%zu!", activeProcindex, _impl->vProc.size());
        return;
    }
    _impl->_activeProcIndex = activeProcindex; //记录一下
    _impl->vGrabTasks.clear();

    LogI("MultiCamera.start():启动采图Task...有%d个CameraImageFactory", _impl->vCameraImageFactory.size());
    if (_impl->vCameraImageFactory.size() == 0) {
        LogE("MultiCamera.start():启动采图Task失败,未添加CameraImageFactory!可能程序未对接新接口...");
    }
    try {
        TaskGrabManyCamera* task = new TaskGrabManyCamera("TaskGrabManyCamera",
                                                          &_impl->imageQueue,
                                                          _impl->vCameraImageFactory);
        task->isMainTask = true;
        _impl->mainGrabTask = task;
        task->isDoProc = true;
        task->proc = _impl->vProc[activeProcindex];
        _impl->vGrabTasks.push_back(task);
        _impl->threadPool.startWithPriority(Poco::Thread::Priority::PRIO_HIGHEST, *task);
    }
    catch (const std::exception& e) {
        LogE("MultiCamera.start():异常 %s", e.what());
    }
}

void MultiCamera::stop()
{
    for (size_t i = 0; i < _impl->vGrabTasks.size(); i++) {
        _impl->vGrabTasks[i]->isRun = false;
    }
    LogI("MultiCamera.stop():等待所有采图任务退出...");
    //TODO:这里可能要考虑自己关闭自己的问题.
    _impl->threadPool.joinAll();
    //删除这些线程
    for (size_t i = 0; i < _impl->vGrabTasks.size(); i++) {
        delete _impl->vGrabTasks[i];
    }

    _impl->vGrabTasks.clear();
    _impl->mainGrabTask = nullptr;

    ////开始等待线程结束，这里总是会遇到一个自己调用close的问题
    //if (_impl->pThread != nullptr) {
    //    //如果不是自己关自己（Poco::Thread::current()这个函数会返回null，如果不是Poco的线程的话）
    //    if (Poco::Thread::current() == nullptr || Poco::Thread::current()->id() != _impl->pThread->id()) {
    //        _impl->pThread->join(); //等待它结束
    //        delete _impl->pThread;
    //        delete _impl->curRunnable;
    //        _impl->pThread = nullptr;
    //        _impl->curRunnable = nullptr;
    //    }
    //    else {
    //        LogW("MultiCamera.stop():是线程自己关闭自己！");
    //    }
    //}
    //else {
    //}
}

uint MultiCamera::frameCount()
{
    if (_impl->mainGrabTask != nullptr) {
        return _impl->imageQueue.frameCount;
    }
    return 0;
}

float MultiCamera::fps()
{
    if (_impl->mainGrabTask != nullptr) {
        return _impl->mainGrabTask->fps;
    }
    return 0;
}

} // namespace dxlib