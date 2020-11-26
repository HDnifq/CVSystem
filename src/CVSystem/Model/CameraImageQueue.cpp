#include "CameraImageQueue.h"
#include "dlog/dlog.h"

namespace dxlib {

CameraImageQueue::CameraImageQueue()
{
}

CameraImageQueue::~CameraImageQueue()
{
}

void CameraImageQueue::Clear()
{
    for (size_t i = 0; i < vLock.size(); i++) {
        delete vLock[i];
    }
    vLock.clear();
    vGrabCamera.clear();
    vTempImageQueue.clear();
}

void CameraImageQueue::AddCamera(Camera* camera)
{
    vGrabCamera.push_back(camera);
    vTempImageQueue.push_back(std::deque<CameraImage>());
    vLock.push_back(new std::mutex());
}

void CameraImageQueue::PushImage(const CameraImage& image)
{
    if (image.camera == nullptr) {
        LogE("CameraImageQueue.PushImage():加入的数据项的camera为null");
        return;
    }

    int index = 0;
    for (size_t i = 0; i < vGrabCamera.size(); i++) {
        if (vGrabCamera[i] == image.camera) {
            index = i;
            break;
        }
    }
    LogD("CameraImageQueue.PushImage():加入图片,等待锁...");
    vLock[index]->lock();
    LogD("CameraImageQueue.PushImage():拿到锁,加入图片!");
    //加入这一张图片
    vTempImageQueue[index].push_back(image);
    vLock[index]->unlock();
}

pCameraImageGroup CameraImageQueue::GetImage()
{
    int doneCount = 0;
    for (size_t i = 0; i < vLock.size(); i++) {
        vLock[i]->lock();
        if (!vTempImageQueue[i].empty()) {
            if (vTempImageQueue[i].size() > 3) {
                LogW("CameraImageQueue.GetImage():当前%zu号图片队列长度过长...长度为%zu", i, vTempImageQueue[i].size());
            }
            else {
                LogD("CameraImageQueue.GetImage():当前%zu号图片队列长度%zu", i, vTempImageQueue[i].size());
            }
            doneCount++;
        }
        vLock[i]->unlock();
    }
    if (doneCount == vGrabCamera.size()) {
        LogD("CameraImageQueue.GetImage():采图完成!");
        pCameraImageGroup imgGroup = std::make_shared<CameraImageGroup>();
        imgGroup->fnum = doneCount++;

        //这表示采图完成了,所有的相机都已经采图OK
        for (size_t i = 0; i < vLock.size(); i++) {
            vLock[i]->lock();
            CameraImage& img = vTempImageQueue[i].front();
            imgGroup->addCameraImage(img);

            //记录一下采图时间
            if (imgGroup->grabStartTime == 0 || imgGroup->grabStartTime > img.grabStartTime) {
                imgGroup->grabStartTime = img.grabStartTime;
            }
            if (imgGroup->grabEndTime == 0 || imgGroup->grabEndTime < img.grabEndTime) {
                imgGroup->grabEndTime = img.grabEndTime;
            }
            LogD("CameraImageQueue.GetImage():队列%zu移出最前一项!", i);
            vTempImageQueue[i].pop_front();
            LogD("CameraImageQueue.GetImage():当前%zu号图片队列长度%zu", i, vTempImageQueue[i].size());
            vLock[i]->unlock();
        }
        return imgGroup;
    }
    else {
        if (doneCount > 0)
            LogD("CameraImageQueue.GetImage():采图未全部完成doneCount=%d , vGrabCamera.size()=%zu", doneCount, vGrabCamera.size());
    }

    //这里采图没有完成
    return nullptr;
}

} // namespace dxlib