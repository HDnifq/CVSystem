#pragma once
#include "CameraImage.h"

//#include "../Common/concurrentqueue.h"
//#include "../Common/blockingconcurrentqueue.h"

#include <vector>
#include <deque>
#include <mutex>

namespace dxlib {

class CameraImageQueue
{
  public:
    CameraImageQueue();
    ~CameraImageQueue();

    //moodycamel::ConcurrentQueue<pCameraImage> vTempImageQueue;
    //moodycamel::ConcurrentQueue<pCameraImage> vDoneImageQueue;

    // 线程锁
    std::vector<std::mutex*> vLock;

    // 相机采的图无脑扔进来
    std::vector<std::deque<ImageItem>> vTempImageQueue;

    // 抓图的相机
    std::vector<Camera*> vGrabCamera;

    // 采图完成的帧数
    int doneCount = 0;

    void Clear();

    void AddCamera(Camera* camera);

    void PushImage(Camera* camera, ImageItem& image);

    void GetImage(pCameraImage& camImage);

  private:
};

} // namespace dxlib