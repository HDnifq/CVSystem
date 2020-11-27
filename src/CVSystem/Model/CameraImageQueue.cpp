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
#if STLQueue
    for (size_t i = 0; i < vLock.size(); i++) {
        delete vLock[i];
    }
    vLock.clear();
#else
#endif // STLQueue
    vGrabCamera.clear();
    vTempImageQueue.clear();
}

void CameraImageQueue::AddCamera(Camera* camera)
{
    vGrabCamera.push_back(camera);

#if STLQueue
    vTempImageQueue.push_back(std::deque<CameraImage>());
    vLock.push_back(new std::mutex());
#else
    vTempImageQueue.push_back(moodycamel::ConcurrentQueue<CameraImage>());
#endif // STLQueue
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
#if STLQueue
    LogD("CameraImageQueue.PushImage():加入图片,等待锁...");
    vLock[index]->lock();
    LogD("CameraImageQueue.PushImage():拿到锁,加入图片!");
    //加入这一张图片
    vTempImageQueue[index].push_back(image);
    vLock[index]->unlock();
#else
    //此时肯定是系统发生了问题,丢弃,防止队列过长
    CameraImage img;
    while (vTempImageQueue[index].size_approx() >= maxQueueLen) {
        if (vTempImageQueue[index].try_dequeue(img)) {
#    if defined(_WIN32) || defined(_WIN64)
            LogW("CameraImageQueue.PushImage():队列长度大于maxQueueLen,这可能在等待其他相机采图...");
#    endif
        }
    }
    //加入这一张图片,一定要加入
    while (!vTempImageQueue[index].enqueue(image)) {
    }
#endif // STLQueue
}

pCameraImageGroup CameraImageQueue::GetImage()
{
    int doneCount = 0;
    for (size_t i = 0; i < vTempImageQueue.size(); i++) {
#if STLQueue
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
#else
        if (vTempImageQueue[i].size_approx() > 0) {
            doneCount++;
        }
#endif // STLQueue
    }

    if (doneCount == vGrabCamera.size()) {
        LogD("CameraImageQueue.GetImage():采图完成!");
        pCameraImageGroup imgGroup = std::make_shared<CameraImageGroup>();
        imgGroup->fnum = doneCount++;

        //这表示采图完成了,所有的相机都已经采图OK
        for (size_t i = 0; i < vTempImageQueue.size(); i++) {
#if STLQueue
            vLock[i]->lock();
            //如果不同相机采图的快慢速度不同了,那么舍弃掉所有很老的图片,只留最后两张,把倒数第二张作为结果传出.
            while (vTempImageQueue[i].size() > 2) {
                LogW("CameraImageQueue.GetImage():当前%zu号图片队列丢弃一帧", i);
                vTempImageQueue[i].pop_front();
            }
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
#else
            CameraImage img;
            while (vTempImageQueue[i].size_approx() > 2) {
                if (vTempImageQueue[i].try_dequeue(img)) {
                    LogW("CameraImageQueue.GetImage():当前%zu号图片队列丢弃一帧", i);
                    LogD("CameraImageQueue.GetImage():当前%zu号图片队列长度%zu", i, vTempImageQueue[i].size_approx());
                }
            }
            //然后硬是要再取一张图出来
            while (!vTempImageQueue[i].try_dequeue(img)) {
            }
            imgGroup->addCameraImage(img);
            //记录一下采图时间
            if (imgGroup->grabStartTime == 0 || imgGroup->grabStartTime > img.grabStartTime) {
                imgGroup->grabStartTime = img.grabStartTime;
            }
            if (imgGroup->grabEndTime == 0 || imgGroup->grabEndTime < img.grabEndTime) {
                imgGroup->grabEndTime = img.grabEndTime;
            }

#endif // STLQueue
        }
        frameCount++;
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