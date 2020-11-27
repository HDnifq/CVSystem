#pragma once
#include "CameraImage.h"
#include "CameraImageGroup.h"

#include "../Common/concurrentqueue.h"
#include "../Common/blockingconcurrentqueue.h"

#include <vector>
#include <deque>
#include <mutex>
#include <atomic>

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

#if STLQueue

    // 线程锁
    std::vector<std::mutex*> vLock;

    // 相机采的图无脑扔进来
    std::vector<std::deque<CameraImage>> vTempImageQueue;

#else

    // 相机采的图无脑扔进来
    std::vector<moodycamel::ConcurrentQueue<CameraImage>> vTempImageQueue;

#endif

    // 抓图的相机
    std::vector<Camera*> vGrabCamera;

    // 全局取图+处理锁(特别要注意这是保护proc帧处理的锁)
    std::mutex lockGetImage;

    // 全局提取帧计数(由于它的使用基本上是在上面的锁的控制范围内,所以它可能不需要原子)
    uint frameCount = 0;

    // 采图队列的最大长度,防止过多的内存占用
    int maxQueueLen = 4;

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
     * 提取一个图片组,如果采图还没有结束那么返回null.不使用STLQueue的话这个函数可能需要由某一个固定线程来调用.
     *
     * @author daixian
     * @date 2020/11/8
     *
     * @returns 图片组,如果失败返回null.
     */
    pCameraImageGroup GetImage();

  private:
};

} // namespace dxlib