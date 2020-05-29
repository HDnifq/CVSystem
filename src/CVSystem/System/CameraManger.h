#pragma once

#include "../Model/Camera.h"
#include "../Model/StereoCamera.h"
#include "../Model/CameraPair.h"

namespace dxlib {

/**
 * 所有相机的参数，单例.
 *
 * @author daixian
 * @date 2018/11/8
 */
class CameraManger
{
  public:
    CameraManger();
    ~CameraManger();

    static CameraManger* GetInst()
    {
        if (m_pInstance == NULL)
            m_pInstance = new CameraManger();
        return m_pInstance;
    }

    // 所有相机的map，以camIndex为key.它实际已经等价于vCamera
    std::map<int, pCamera> camMap;

    // 立体相机对.
    std::vector<pStereoCamera> vStereo;

    // 所有相机对的数据.
    std::vector<pCameraPair> vCameraPair;

    /**
     * 清空记录.
     *
     * @author daixian
     * @date 2018/11/29
     */
    void clear();

    /**
     * 添加一个相机，会自动递增分配一个camIndex.
     *
     * @author daixian
     * @date 2018/11/29
     *
     * @param  cp 要添加的相机.
     *
     * @returns A pCamera.
     */
    pCamera add(pCamera cp);

    /**
     * 添加一个虚拟相机,虚拟相机参与计算逻辑.
     *
     * @author daixian
     * @date 2020/3/27
     *
     * @param  cp 要添加的相机.
     *
     * @returns A pCamera.
     */
    pCamera addVirtual(pCamera cp);

    /**
     * 添加立体相机对.
     *
     * @author daixian
     * @date 2019/2/8
     *
     * @param  sc 要添加的这个立体相机对.
     *
     * @returns A pStereoCamera.
     */
    pStereoCamera add(pStereoCamera sc);

    /**
     * 通过camIndex得到一个相机.
     *
     * @author daixian
     * @date 2020/3/17
     *
     * @param  camIndex 相机的index.
     *
     * @returns 如果相机不存在那么返回null.
     */
    pCamera getCamera(const int camIndex);

    /**
     * 通过相机名得到一个相机.
     *
     * @author daixian
     * @date 2020/3/17
     *
     * @param  devName 相机名.
     *
     * @returns 如果相机不存在那么返回null.
     */
    pCamera getCamera(const std::string& devName);

    /**
     * 得到当前所有相机的vector数组.
     *
     * @author daixian
     * @date 2020/4/18
     *
     * @returns 所有的相机组成的vector.
     */
    std::vector<pCamera>& getCameraVec();

    /**
     * 得到一个立体相机,找不到则返回null.
     *
     * @author daixian
     * @date 2018/11/29
     *
     * @param  name 立体相机的名字.
     *
     * @returns The stereo.
     */
    pStereoCamera getStereo(const std::string& name);

    /**
     * 使用子相机来得到一个立体相机,找不到则返回null.
     *
     * @author daixian
     * @date 2018/11/29
     *
     * @param  camera 属于这个立体相机的相机.
     *
     * @returns The stereo.
     */
    pStereoCamera getStereo(pCamera camera);

    /**
     * 使用子相机来得到一个立体相机,找不到则返回null.
     *
     * @author daixian
     * @date 2018/11/29
     *
     * @param  camIndex 属于这个立体相机的相机的index.
     *
     * @returns 找不到返回null.
     */
    pStereoCamera getStereo(int camIndex);

    /**
     * 得到一个相机对,找不到则返回null.
     *
     * @author daixian
     * @date 2018/11/29
     *
     * @param  name 相机对的名字.
     *
     * @returns 相机对.
     */
    pCameraPair getCameraPair(const std::string& name);

    /**
     * 得到一个相机对,因为这个相机对可能有随意组合的情况,所以需要两个参数,找不到则返回null.
     *
     * @author daixian
     * @date 2020/4/19
     *
     * @param  cameraL 相机对的L相机.
     * @param  cameraR 相机对的R相机.
     *
     * @returns 找不到返回null.
     */
    pCameraPair getCameraPair(pCamera cameraL, pCamera cameraR);

    /**
     * 得到一个相机对,因为这个相机对可能有随意组合的情况,所以需要两个参数,找不到则返回null.
     *
     * @author daixian
     * @date 2020/4/19
     *
     * @param  camIndexL 相机对的L相机的index.
     * @param  camIndexR 相机对的R相机的index.
     *
     * @returns 找不到返回null.
     */
    pCameraPair getCameraPair(int camIndexL, int camIndexR);

    /**
     * 设置某一个相机的属性.
     *
     * @author daixian
     * @date 2019/1/13
     *
     * @param  camIndex 相机的index.
     * @param  CAP_PROP The capability property.
     * @param  value    The value.
     *
     * @returns 如果存在这个相机就进行设置并且返回true.
     */
    bool setProp(int camIndex, cv::VideoCaptureProperties CAP_PROP, double value);

    /**
     * 是否所有的相机都已经打开.
     *
     * @author daixian
     * @date 2019/10/25
     *
     * @returns 如果相机全都打开了返回true.
     */
    bool isAllCameraIsOpen();

    /**
     * 检测相机的录入.
     *
     * @author daixian
     * @date 2020/3/30
     *
     * @returns 检察相机的录入是否有问题.
     */
    bool checkInputData();

  private:
    // 单例.
    static CameraManger* m_pInstance;

    // 名字和相机指针的map.
    std::map<std::string, pCamera> mNamePCamera;

    // 相机index的数组.
    std::vector<pCamera> vCamera;
};

} // namespace dxlib