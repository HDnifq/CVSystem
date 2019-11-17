#include "CameraGrab.h"
#include <chrono>
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
        //保证resize
        if (vCameras.size() <= kvp.second->camIndex) {
            vCameras.resize(kvp.second->camIndex + 1); //这里resize一个指针数组那么会创建一些空指针
        }
        vCameras[kvp.second->camIndex] = kvp.second;
    }
}

void CameraGrab::setCamerasAssist(const std::map<int, pCamera>& camMap)
{
    vCameraAssist.clear();
    for (auto& kvp : camMap) {
        //保证resize
        if (vCameraAssist.size() <= kvp.second->camIndex) {
            vCameraAssist.resize(kvp.second->camIndex + 1); //这里resize一个指针数组那么会创建一些空指针
        }
        vCameraAssist[kvp.second->camIndex] = kvp.second;
    }
}

bool CameraGrab::grab(pCameraImage& cimg)
{
    bool isSuccess = true;
    //执行完毕之后就更新相机的属性状态
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        if (vCameras[camIndex] != nullptr) {
            vCameras[camIndex]->applyCapProp();
        }
    }
    //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
    ++fnumber;

    //构造采图结果：一个结构体包含4个相机的图
    cimg = pCameraImage(new CameraImage());
    cimg->fnum = fnumber;
    cimg->grabStartTime = clock();
    cimg->vImage.resize(vCameras.size());            //先直接创建算了
    cimg->vImageAssist.resize(vCameraAssist.size()); //先直接创建算了

    //对所有相机采图
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        try {
            if (vCameras[camIndex] == nullptr) {
                continue;
            }
            Camera* curCamera = vCameras[camIndex].get();
            if (curCamera->isVirtualCamera) {
                continue;
            }

            if (!curCamera->isOpened()) {
                //LogE("CameraGrab.grab():cam %d 相机没有打开！", camIndex);
                continue;
            }
            grabOneCamra(cimg, vCameras[camIndex].get());
        }
        catch (const std::exception& e) {
            LogE("CameraGrab.grab():异常 %s", e.what());
            isSuccess = false;
        }
    }

    //对所有辅助相机采图
    for (size_t camIndex = 0; camIndex < vCameraAssist.size(); camIndex++) {
        try {
            if (vCameraAssist[camIndex] == nullptr) {
                continue;
            }
            Camera* curCamera = vCameraAssist[camIndex].get();
            if (curCamera->isVirtualCamera) {
                continue;
            }

            if (!curCamera->isOpened()) {
                //LogE("CameraGrab.grab():cam %d 相机没有打开！", camIndex);
                continue;
            }
            grabOneCamra(cimg, vCameraAssist[camIndex].get());
        }
        catch (const std::exception& e) {
            LogE("CameraGrab.grab():异常 %s", e.what());
            isSuccess = false;
        }
    }

    cimg->grabEndTime = clock();

    updateFPS();

    return isSuccess;
}

void CameraGrab::grabOneCamra(pCameraImage& cimg, Camera* curCamera)
{
    int camIndex = curCamera->camIndex;
    ImageItem* pitem = nullptr;
    if (!curCamera->isAssist) {
        pitem = &(cimg->vImage[camIndex]);
    }
    else {
        pitem = &(cimg->vImageAssist[camIndex]);
    }
    ImageItem& item = *pitem;
    item.camera = curCamera; //标记camera来源

    //如果不是stereo相机这样
    if (!curCamera->isStereoCamera) {

        item.grabStartTime = clock();
        if (curCamera->capture->read(item.image)) {
            //如果是需要处理,特殊相机旋转了180度的
            if (curCamera->isAutoProcSpecialTpye && curCamera->specialTpye == Camera::SpecialTpye::Rotate180) {
                cv::Mat normalImage;
                cv::rotate(item.image, normalImage, cv::ROTATE_180);
                item.image = normalImage;
            }
            item.isSuccess = true;
            item.grabEndTime = clock();
            LogD("CameraGrab.grabOneCamra():cam %d 采图完成！fnumber=%d", curCamera->camIndex, fnumber);
        }
        else {
            item.isSuccess = false;
            item.grabEndTime = clock();
            LogE("CameraGrab.grabOneCamra():cam %d 采图read失败！", camIndex);
        }
    }
    else {
        //如果是stereo相机
        int camIndexL = curCamera->stereoCamIndexL;
        int camIndexR = curCamera->stereoCamIndexR;
        CV_Assert(camIndexL >= 0 && camIndexR >= 0);

        ImageItem& itemL = cimg->vImage[camIndexL];
        ImageItem& itemR = cimg->vImage[camIndexR];
        itemL.camera = vCameras[camIndexL].get(); //标记camera来源(这里规定只能是vCameras来源)
        itemR.camera = vCameras[camIndexR].get(); //标记camera来源(这里规定只能是vCameras来源)
        item.grabStartTime = itemL.grabStartTime = itemR.grabStartTime = clock();

        if (curCamera->capture->read(item.image)) {
            item.isSuccess = itemL.isSuccess = itemR.isSuccess = true;
            item.grabEndTime = itemL.grabEndTime = itemR.grabEndTime = clock();

            int w = item.image.cols;
            int h = item.image.rows;
            itemL.image = cv::Mat(item.image, cv::Rect(0, 0, w / 2, h));     //等于图的左半边
            itemR.image = cv::Mat(item.image, cv::Rect(w / 2, 0, w / 2, h)); //等于图的右半边
            LogD("CameraGrab.grabOneCamra():cam %d 采图完成！fnumber=%d", curCamera->camIndex, fnumber);

            //如果不传图到后面的处理,那么直接干掉这个
            if (curCamera->isNoSendToProc) {
                item.image.release();
                item.isSuccess = false;
            }
        }
        else {
            item.isSuccess = itemL.isSuccess = itemR.isSuccess = false;
            item.grabEndTime = itemL.grabEndTime = itemR.grabEndTime = clock();

            LogE("CameraGrab.grabOneCamra():cam %d 采图read失败！", camIndex);
        }
    }
}

bool CameraGrab::startGrabImage(pCameraImage& cimg)
{
    //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
    ++fnumber;

    //构造采图结果：一个结构体包含4个相机的图
    cimg = pCameraImage(new CameraImage());
    cimg->fnum = fnumber;
    cimg->grabStartTime = clock();
    cimg->vImage.resize(vCameras.size());            //先直接创建算了
    cimg->vImageAssist.resize(vCameraAssist.size()); //先直接创建算了
    return true;
}

bool CameraGrab::grabWithCamIndex(pCameraImage& cimg, int camIndex)
{
    if (camIndex >= vCameras.size()) {
        LogE("CameraGrab.grabWithCamIndex():输入camIndex不正确 camIndex=%d！", camIndex);
        return false;
    }
    try {
        if (vCameras[camIndex] == nullptr) {
            return true;
        }
        if (!vCameras[camIndex]->isOpened()) {
            //LogE("CameraGrab.grabWithCamIndex():cam %d 相机没有打开！", camIndex);
            return false;
        }
        ImageItem& item = cimg->vImage[camIndex];
        item.camera = vCameras[camIndex].get(); //标记camera来源

        item.grabStartTime = clock();
        if (vCameras[camIndex]->capture->read(item.image)) {

            item.isSuccess = true;
            item.grabEndTime = clock();
            LogD("CameraGrab.grabWithCamIndex():cam %d 采图完成！fnumber=%d", vCameras[camIndex]->camIndex, fnumber);
            return true;
        }
        else {
            item.isSuccess = false;
            item.grabEndTime = clock();
            LogE("CameraGrab.grabWithCamIndex():cam %d 采图read失败！", camIndex);
            return false;
        }
    }
    catch (const std::exception& e) {
        LogE("CameraGrab.grabWithCamIndex():异常 %s", e.what());
    }
    return false;
}

bool CameraGrab::endGrabImage(pCameraImage& result)
{
    result->grabEndTime = clock();
    updateFPS();
    return true;
}

bool CameraGrab::open()
{
    if (vCameras.size() == 0 && vCameraAssist.size() == 0) {
        LogW("CameraGrab.open():事先没有录入有效的相机（vCameras.size()=0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        auto& camera = vCameras[camIndex];
        if (camera == nullptr) {
            continue;
        }
        if (camera->isVirtualCamera) {
            LogI("CameraGrab.open():相机 %s 是虚拟相机，不需要打开...", camera->devNameA.c_str());
            continue;
        }
        LogI("CameraGrab.open():尝试打开相机 %s ...", camera->devNameA.c_str());
        boost::timer t;
        //打开相机
        if (camera->open()) {
            double costTime = t.elapsed();
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            camera->capture->read(img);
            LogI("CameraGrab.open():成功打开一个相机%s，耗时%.2f秒", camera->devNameA.c_str(), costTime); //打开相机大致耗时0.2s
        }
        else {
            isSuccess = false;
        }
    }

    for (size_t camIndex = 0; camIndex < vCameraAssist.size(); camIndex++) {
        auto& camera = vCameraAssist[camIndex];
        if (camera == nullptr) {
            continue;
        }
        if (camera->isVirtualCamera) {
            LogI("CameraGrab.open():相机 %s 是虚拟相机，不需要打开...", camera->devNameA.c_str());
            continue;
        }
        LogI("CameraGrab.open():尝试打开相机 %s ...", camera->devNameA.c_str());
        boost::timer t;
        //打开相机
        if (camera->open()) {
            double costTime = t.elapsed();
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            camera->capture->read(img);
            LogI("CameraGrab.open():成功打开一个相机%s，耗时%.2f秒", camera->devNameA.c_str(), costTime); //打开相机大致耗时0.2s
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
        if (vCameras[i] != nullptr) {
            LogI("CameraGrab.close():释放相机%s ...", vCameras[i]->devNameA.c_str());
            vCameras[i]->release();
        }
    }
    for (size_t i = 0; i < vCameraAssist.size(); i++) {
        if (vCameraAssist[i] != nullptr) {
            LogI("CameraGrab.close():释放相机%s ...", vCameraAssist[i]->devNameA.c_str());
            vCameraAssist[i]->release();
        }
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
    vCameraAssist.clear();
}

void CameraGrab::updateFPS()
{
    clock_t now = clock();
    double costTime = (double)(now - _lastTime) / CLOCKS_PER_SEC;
    if (costTime > 1.0) { //如果经过了1秒
        //FPS写入到了camera
        for (size_t i = 0; i < vCameras.size(); i++) {
            if (vCameras[i] != nullptr)
                vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位
        }

        _lastTime = now;        //记录现在的最近一次的时间
        _lastfnumber = fnumber; //记录现在最近的帧数
    }
    else if (costTime > 0.5) { //在0.5秒之后可以开始计算了
        //FPS写入到了camera
        for (size_t i = 0; i < vCameras.size(); i++) {
            if (vCameras[i] != nullptr)
                vCameras[i]->FPS = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位
        }
    }
}

} // namespace dxlib