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
    vLock[index]->lock();
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
            doneCount++;
        }
        vLock[i]->unlock();
    }
    if (doneCount == vGrabCamera.size()) {
        pCameraImageGroup imgGroup = std::make_shared<CameraImageGroup>();
        imgGroup->fnum = doneCount++;

        //这表示采图完成了,所有的相机都已经采图OK
        for (size_t i = 0; i < vLock.size(); i++) {
            vLock[i]->lock();
            CameraImage& img = vTempImageQueue[i].front();
            imgGroup->addCameraImage(img);
            vTempImageQueue[i].pop_front();
            vLock[i]->unlock();
        }
        return imgGroup;
    }

    //这里采图没有完成
    return nullptr;
}

} // namespace dxlib