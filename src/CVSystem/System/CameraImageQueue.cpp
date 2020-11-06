#include "CameraImageQueue.h"

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
    vTempImageQueue.push_back(std::deque<ImageItem>());
    vLock.push_back(new std::mutex());
}

void CameraImageQueue::PushImage(Camera* camera, ImageItem& image)
{
    int index = 0;
    for (size_t i = 0; i < vGrabCamera.size(); i++) {
        if (vGrabCamera[i] == camera) {
            index = i;
            break;
        }
    }
    vLock[index]->lock();
    //加入这一张图片
    vTempImageQueue[index].push_back(image);
    vLock[index]->unlock();
}

void CameraImageQueue::GetImage(pCameraImage& camImage)
{
    int doneCount = 0;
    for (size_t i = 0; i < vLock.size(); i++) {
        vLock[i]->lock();
        if (!vTempImageQueue[i].empty()) {
            doneCount++;
        }
        vLock[i]->unlock();
    }
    if (doneCount == vGrabCamera.size()) {
        //这表示采图完成了
        for (size_t i = 0; i < vLock.size(); i++) {
            vLock[i]->lock();
            if (!vTempImageQueue[i].empty()) {
                doneCount++;
            }
            vLock[i]->unlock();
        }
    }
}

} // namespace dxlib