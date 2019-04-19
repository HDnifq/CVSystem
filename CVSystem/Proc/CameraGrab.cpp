#include "CameraGrab.h"
#include <Chrono>
#include <boost/timer.hpp>
#include <memory>
#include "dlog/dlog.h"

namespace dxlib {

CameraGrab::CameraGrab()
{
}

CameraGrab::CameraGrab(const std::vector<pCamera>& cps)
{
    for (auto& item : cps) {
        if (!item->isVirtualCamera)
            vCameras.push_back(item);
    }
}

CameraGrab::~CameraGrab()
{
}

void CameraGrab::setCameras(const std::map<int, pCamera>& camMap)
{
    vCameras.clear();
    for (auto& kvp : camMap) {
        if (!kvp.second->isVirtualCamera)
            vCameras.push_back(kvp.second);
    }
}

bool CameraGrab::grab(pCameraImage& cimg)
{
    //执行完毕之后就更新相机的属性状态
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        vCameras[camIndex]->applyCapProp();
    }
    //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
    ++fnumber;

    //采图：一个结构体包含4个相机的图
    cimg = pCameraImage(new CameraImage());
    cimg->fnum = fnumber;
    cimg->grabStartTime = clock();
    cimg->vImage.resize(vCameras.size()); //先直接创建算了

    //对所有相机采图
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        try {
            ImageItem& item = cimg->vImage[camIndex];
            item.camera = vCameras[camIndex].get(); //标记camera来源

            if (!vCameras[camIndex]->isOpened()) {
                LogE("CameraGrab.grab():cam %d 相机没有打开！", camIndex);
                return false;
            }
            item.grabStartTime = clock();
            if (vCameras[camIndex]->capture->read(item.image)) {

                item.isSuccess = true;
                item.grabEndTime = clock();
                LogD("CameraGrab.grab():cam %d 采图完成！fnumber=%d", vCameras[camIndex]->camIndex, fnumber);
            }
            else {
                item.isSuccess = false;
                item.grabEndTime = clock();
                LogE("CameraGrab.grab():cam %d 采图read失败！", camIndex);
                return false;
            }
        }
        catch (const std::exception& e) {
            LogE("CameraGrab.grab():异常 %s", e.what());
            return false;
        }
    }
    cimg->grabEndTime = clock();

    updateFPS();

    return true;
}

bool CameraGrab::open()
{
    if (vCameras.size() == 0) {
        LogW("CameraGrab.open():事先没有录入有效的相机（vCameras.size()=0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        LogI("CameraGrab.open():尝试打开相机 %s ...", vCameras[camIndex]->devNameA.c_str());
        boost::timer t;

        //打开相机
        if (vCameras[camIndex]->openCamera()) {
            double costTime = t.elapsed();
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            vCameras[camIndex]->capture->read(img);
            LogI("CameraGrab.open():成功打开一个相机%s，耗时%.2f秒", vCameras[camIndex]->devNameA.c_str(), costTime); //打开相机大致耗时0.2s
        }
        else {
            isSuccess = false;
        }
    }
    return isSuccess;
}

bool CameraGrab::close()
{
    for (size_t i = 0; i < vCameras.size(); i++) {
        LogI("CameraGrab.close():释放相机%s ...", vCameras[i]->devNameA.c_str());
        vCameras[i]->releaseCamera();
    }
    clear();
    return true;
}

void CameraGrab::clear()
{
    fnumber = 0;
    _lastTime = 0;
    _lastfnumber = 0;
    vCameras.clear();
}

void CameraGrab::updateFPS()
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

} // namespace dxlib