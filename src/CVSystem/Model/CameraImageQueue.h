#pragma once
#include "CameraImage.h"

//#include "../Common/concurrentqueue.h"
//#include "../Common/blockingconcurrentqueue.h"

#include <vector>
#include <deque>
#include <mutex>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 用于多线程采图的时候无脑的采图的数据结构. </summary>
///
/// <remarks> Surface, 2020/11/7. </remarks>
///-------------------------------------------------------------------------------------------------
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
    std::vector<std::deque<CameraImage>> vTempImageQueue;

    // 抓图的相机
    std::vector<Camera*> vGrabCamera;

    // 采图完成的帧数
    int doneCount = 0;

    void Clear();

    // 首先需要增加一个相机的记录.
    void AddCamera(Camera* camera);

    void PushImage(Camera* camera, CameraImage& image);

    void GetImage(pCameraImage& camImage);

  private:
};

} // namespace dxlib