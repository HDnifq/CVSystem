#include "CameraGrab.h"
#include <chrono>
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
    std::size_t size = 0;
    for (auto& kvp : camMap) {
        if (kvp.second->camIndex + 1 > size) {
            size = kvp.second->camIndex + 1;
        }
    }

    //检察相机的camMap中的key是否是按照数字顺序排列的
    if (size != camMap.size()) {
        LogW("CameraGrab::setCameras():注意!录入的camMap的index有跳跃,因此CameraGrab中有空相机,camMap.size()= %zu ,size= %zu", camMap.size(), size);
    }
    else {
        LogI("CameraGrab::setCameras():camMap记录相机的个数和CameraGrab一致,count=%d", size);
    }

    //保证resize
    vCameras.resize(size);

    for (auto& kvp : camMap) {
        CV_Assert(kvp.first == kvp.second->camIndex);
        vCameras[kvp.second->camIndex] = kvp.second;
    }

    //检察isNoSendToProc的设置情况
    size_t index = vCameras.size();
    for (size_t i = 0; i < vCameras.size(); i++) {
        if (vCameras[i]->isNoSendToProc) {
            index = i;
            LogI("CameraGrab::setCameras():注意:相机%s设置了不发送处理(isNoSendToProc=true)!", vCameras[i]->devName.c_str());
        }
        else {
            if (i > index) {
                LogE("CameraGrab::setCameras():相机%s的index在某个isNoSendToProc的相机之后!", vCameras[i]->devName.c_str());
            }
        }
    }
}

//传递进来的参数是nullptr
bool CameraGrab::grab(pCameraImage& cimg)
{
    int success = 0;
    int fail = 0;
    cimg = nullptr;

    //执行完毕之后就更新相机的属性状态
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        if (vCameras[camIndex] != nullptr) {
            vCameras[camIndex]->applyCapProp();
        }
    }
    //构造采图结果：一个结构体包含4个相机的图
    cimg = pCameraImage(new CameraImage(vCameras));
    //cimg->fnum = fnumber;//这个帧号在这个函数外面标记
    cimg->grabStartTime = clock();

    //对所有相机采图
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        try {
            grabOneCamera(cimg, vCameras[camIndex].get());
            success++;
        }
        catch (const std::exception& e) {
            LogE("CameraGrab.grab():异常 %s", e.what());
            fail++;
        }
    }

    //统计采图成功和失败的
    for (size_t i = 0; i < cimg->vImage.size(); i++) {
        if (cimg->vImage[i].isSuccess) {
            success++;
        }
        else {
            fail++;
        }
    }

    if (cimg->vImage.size() > 0) {
        //如果不传图到后面的处理,那么直接干掉这个
        while (cimg->vImage.back().camera->isNoSendToProc) {
            cimg->vImage.pop_back();
        }
    }

    cimg->grabEndTime = clock();

    //如果所有的采图都成功才返回true
    if (success > 0 && fail == 0) {
        return true;
    }
    else {
        return false;
    }
}

void CameraGrab::grabOneCamera(pCameraImage& cimg, Camera* curCamera)
{
    if (curCamera == nullptr) {
        return;
    }
    int camIndex = curCamera->camIndex;
    //当前要写入的[相机-图像]
    ImageItem& item = cimg->vImage[camIndex];

    if (curCamera->isVirtualCamera) {
        //item.isSuccess = false;//(这个失败是默认值)
        return;
    }

    if (!curCamera->isOpened()) {
        //item.isSuccess = false;//标记采图失败
        return;
    }

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
            LogD("CameraGrab.grabOneCamera():相机%s采图完成！", curCamera->devName.c_str());
        }
        else {
            item.isSuccess = false;
            item.grabEndTime = clock();
            LogE("CameraGrab.grabOneCamera():相机%s采图read失败！", curCamera->devName.c_str());
        }
    }
    else {
        //如果是stereo相机
        int camIndexL = curCamera->stereoCamIndexL;
        int camIndexR = curCamera->stereoCamIndexR;
        CV_Assert(camIndexL >= 0 && camIndexR >= 0);
        CV_Assert(camIndexL < cimg->vImage.size() && camIndexR < cimg->vImage.size());

        ImageItem& itemL = cimg->vImage[camIndexL];
        ImageItem& itemR = cimg->vImage[camIndexR];
        itemL.camera = vCameras[camIndexL].get(); //标记camera来源(这里规定只能是vCameras来源)
        itemR.camera = vCameras[camIndexR].get(); //标记camera来源(这里规定只能是vCameras来源)

        //加入自己的steroInfo
        cimg->stereoInfo[curCamera->scID] = {itemL.camera, itemR.camera};

        item.grabStartTime = itemL.grabStartTime = itemR.grabStartTime = clock();

        cv::Mat imgNew;
        if (curCamera->capture->read(imgNew)) {
            item.isSuccess = itemL.isSuccess = itemR.isSuccess = true;
            item.grabEndTime = itemL.grabEndTime = itemR.grabEndTime = clock();

            int w = imgNew.cols;
            int h = imgNew.rows;
            if (w != curCamera->size.width || h != curCamera->size.height) {
                LogE("CameraGrab.grabOneCamera():相机%s采图分辨率错误!设定值(%d,%d)=>(%d,%d)",
                     curCamera->devName.c_str(),
                     curCamera->size.width,
                     curCamera->size.height,
                     w,
                     h);
            }

            item.image = imgNew;
            //这里实际上没有拷贝的
            itemL.image = cv::Mat(imgNew, cv::Rect(0, 0, w / 2, h));     //等于图的左半边
            itemR.image = cv::Mat(imgNew, cv::Rect(w / 2, 0, w / 2, h)); //等于图的右半边
            LogD("CameraGrab.grabOneCamera():Stereo相机%s采图完成！", curCamera->devName.c_str());
        }
        else {
            item.isSuccess = itemL.isSuccess = itemR.isSuccess = false;
            item.grabEndTime = itemL.grabEndTime = itemR.grabEndTime = clock();

            LogE("CameraGrab.grabOneCamera():Stereo相机%s采图read失败！", curCamera->devName.c_str());
        }
    }
}

//bool CameraGrab::startGrabImage(pCameraImage& cimg)
//{
//    //递增帧序号（首先上来就递增，所以出图的第一帧从1开始）
//    ++fnumber;
//
//    //构造采图结果：一个结构体包含4个相机的图
//    cimg = pCameraImage(new CameraImage(vCameras, vCameraAssist));
//    cimg->fnum = fnumber;
//    cimg->grabStartTime = clock();
//    return true;
//}

//bool CameraGrab::grabWithCamIndex(pCameraImage& cimg, int camIndex)
//{
//    if (camIndex >= vCameras.size()) {
//        LogE("CameraGrab.grabWithCamIndex():输入camIndex不正确 camIndex=%d！", camIndex);
//        return false;
//    }
//    try {
//        if (vCameras[camIndex] == nullptr) {
//            return true;
//        }
//        if (!vCameras[camIndex]->isOpened()) {
//            //LogE("CameraGrab.grabWithCamIndex():cam %d 相机没有打开！", camIndex);
//            return false;
//        }
//        ImageItem& item = cimg->vImage[camIndex];
//        item.camera = vCameras[camIndex].get(); //标记camera来源
//
//        item.grabStartTime = clock();
//        if (vCameras[camIndex]->capture->read(item.image)) {
//
//            item.isSuccess = true;
//            item.grabEndTime = clock();
//            LogD("CameraGrab.grabWithCamIndex():cam %d 采图完成！fnumber=%d", vCameras[camIndex]->camIndex, fnumber);
//            return true;
//        }
//        else {
//            item.isSuccess = false;
//            item.grabEndTime = clock();
//            LogE("CameraGrab.grabWithCamIndex():cam %d 采图read失败！", camIndex);
//            return false;
//        }
//    }
//    catch (const std::exception& e) {
//        LogE("CameraGrab.grabWithCamIndex():异常 %s", e.what());
//    }
//    return false;
//}

//bool CameraGrab::endGrabImage(pCameraImage& result)
//{
//    result->grabEndTime = clock();
//    updateFPS();
//    return true;
//}

bool CameraGrab::open()
{
    if (vCameras.size() == 0) {
        LogW("CameraGrab.open():事先没有录入有效的相机（vCameras.size()=0），不做操作直接返回！");
        return false;
    }

    bool isSuccess = true;
    for (size_t camIndex = 0; camIndex < vCameras.size(); camIndex++) {
        auto& camera = vCameras[camIndex];
        if (camera == nullptr) {
            LogI("CameraGrab.open():相机index%d 为null", camIndex);
            continue;
        }
        if (camera->isVirtualCamera) {
            LogI("CameraGrab.open():相机 %s 是虚拟相机，不需要打开...", camera->devName.c_str());
            continue;
        }
        LogI("CameraGrab.open():尝试打开相机 %s ...", camera->devName.c_str());
        clock_t startTime = clock();
        //打开相机
        if (camera->open()) {
            double costTime = (clock() - startTime) / CLOCKS_PER_SEC * 1000;
            //先读一下看看,因为读第一帧的开销时间较长，可能影响dowork()函数中FPS的计算。
            cv::Mat img;
            camera->capture->read(img);
            LogI("CameraGrab.open():成功打开一个相机%s，耗时%.2f毫秒", camera->devName.c_str(), costTime); //打开相机大致耗时0.2s
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
            LogI("CameraGrab.close():释放相机%s ...", vCameras[i]->devName.c_str());
            vCameras[i]->release();
        }
    }

    clear();
    return true;
}

void CameraGrab::clear()
{
    vCameras.clear();
}
} // namespace dxlib