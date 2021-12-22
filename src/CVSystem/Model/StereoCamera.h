#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "CameraDevice.h"
#include "StereoCameraParam.h"
#include "IStereo.h"

namespace dxlib {

/**
 * 立体相机(实际上它叫StereoCameraInfo更贴切).
 *
 * @author daixian
 * @date 2018/11/23
 */
class StereoCamera : public IStereo
{
  public:
    // 立体相机对的序号(区分多组立体相机).
    int scID = -1;

    // 这个立体相机组的名字.
    std::string name;

    // 实际的物理双目相机.
    pCameraDevice device;

    // 是否是垂直的立体相机,如果它为false则默认它是水平的立体相机.
    bool isVertical = false;

    // 是否L相机的点的Y值在上面（更小）用于匹配(这个属性实际不是绝对可靠).
    bool LYisAbove = true;

    // 是否L相机的点值X比R相机的X值小(这个属性实际不是绝对可靠).
    bool isLXLessThanR = true;

    /**
     * 使用一个实际的物理相机和LR相机一起设置个stereo信息.
     *
     * @author daixian
     * @date 2020/4/16
     *
     * @param      scID   Identifier for the screen.
     * @param [in] device The camera phy.
     * @param [in] camL   The camera l.
     * @param [in] camR   The camera r.
     */
    void setCamera(int scID, const pCameraDevice& device, const pCamera& camL, const pCamera& camR);

    /**
     * Sets a camera
     *
     * @author daixian
     * @date 2020/12/9
     *
     * @param          scID Identifier for the screen.
     * @param [in,out] camL The camera l.
     * @param [in,out] camR The camera r.
     */
    void setCamera(int scID, const pCamera& camL, const pCamera& camR);

    /**
     * 设置投影矩阵.
     *
     * @author daixian
     * @date 2020/4/14
     *
     * @param  LP L相机的投影矩阵.
     * @param  RP R相机的投影矩阵.
     */
    void setProjection(const cv::Mat& LP, const cv::Mat& RP)
    {
        this->LP = LP;
        this->RP = RP;
        this->camL->projection = LP;
        this->camR->projection = RP;
    }

    /**
     * 设置这个立体相机组中的L相机位置矩阵4x4,会对这个对象的成员camTR4x4和camL相机中的camTR4x4成员同时赋值.
     *
     * @author daixian
     * @date 2020/6/11
     *
     * @param  TRMat4x4 4x4的TR矩阵.
     * @param  isSetCamR 是否同时设置camR的值.
     */
    void setTRMat4x4(const cv::Mat& TRMat4x4, bool isSetCamR = true);

    /**
     * 根据当前的RT矩阵创建投影矩阵.
     *
     * @author daixian
     * @date 2020/5/8
     */
    void createProjectMat();
};
typedef std::shared_ptr<StereoCamera> pStereoCamera;

} // namespace dxlib
