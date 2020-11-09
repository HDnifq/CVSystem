#pragma once
#include "CameraImage.h"
#include "CameraImageGroup.h"

//#include "../Common/concurrentqueue.h"
//#include "../Common/blockingconcurrentqueue.h"

#include <vector>
#include <deque>
#include <mutex>

namespace dxlib {

/**
 * 用于多线程采图的时候无脑的采图的数据结构.
 *
 * @author daixian
 * @date 2020/11/8
 */
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

    /**
     * 清空数据记录
     *
     * @author daixian
     * @date 2020/11/9
     */
    void Clear();

    /**
     * 首先需要增加一个相机的记录,给对应的数据结构都增加.
     * vTempImageQueue和vGrabCamera这两个数组的index是保持一致的.
     *
     * @author daixian
     * @date 2020/11/8
     *
     * @param  camera 相机.
     */
    void AddCamera(Camera* camera);

    /**
     * 添加一个相机图片.
     *
     * @author daixian
     * @date 2020/11/8
     *
     * @param  image The image.
     */
    void PushImage(const CameraImage& image);

    /**
     * 提取一个图片组.
     *
     * @author daixian
     * @date 2020/11/8
     *
     * @returns The image.
     */
    pCameraImageGroup GetImage();

  private:
};

} // namespace dxlib