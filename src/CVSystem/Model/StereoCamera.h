#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "CameraDevice.h"
#include "StereoCameraParam.h"

namespace dxlib {

/**
 * 立体相机(实际上它叫StereoCameraInfo更贴切).
 *
 * @author daixian
 * @date 2018/11/23
 */
class StereoCamera
{
  public:
    // 立体相机对的序号(区分多组立体相机).
    int scID = -1;

    // 这个立体相机组的名字.
    std::string name;

    // L相机.
    pCamera camL;

    // R相机.
    pCamera camR;

    // 实际的物理双目相机.
    pCameraDevice device;

    // 相机的参数对应的分辨率size.
    cv::Size paramSize;

    // 参数R.
    cv::Mat R;

    // 参数T.
    cv::Mat T;

    // 参数E.
    cv::Mat E;

    // 参数F.
    cv::Mat F;

    // 参数Q.
    cv::Mat Q;

    // 左相机的投影矩阵.
    cv::Mat LP;

    // 右相机的投影矩阵.
    cv::Mat RP;

    // 这组相机3d空间到某世界空间的变换矩阵,它应该等于camL里的的相机camTR4x4.
    cv::Mat camTR4x4;

    // 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec3d camPos;

    // 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec4d camRotate;

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
    void setCamera(int scID, pCameraDevice& device, pCamera& camL, pCamera& camR);

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
    void setCamera(int scID, pCamera& camL, pCamera& camR);

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
     * 设置相机位置矩阵4x4
     *
     * @author daixian
     * @date 2020/6/11
     *
     * @param  TRMat4x4 4x4的TR矩阵.
     */
    void setTRMat4x4(const cv::Mat& TRMat4x4);

    /**
     * 根据当前的RT矩阵创建投影矩阵.
     *
     * @author daixian
     * @date 2020/5/8
     */
    void createProjectMat();

    /**
     * 得到一个相机的数组，index=0表示L相机，index=1表示R相机.
     *
     * @author daixian
     * @date 2020/3/30
     *
     * @returns The cameras.
     */
    std::array<pCamera, 2> getCameras()
    {
        std::array<pCamera, 2> arr = {camL, camR};
        return arr;
    }

    /** 这是相机的备用参数，一个相机可以记录多组参数. */
    std::vector<std::pair<cv::Size, pStereoCameraParam>> vParams;

    /**
     * 切换应用这个分辨率的参数(待定).
     *
     * @author daixian
     * @date 2020/5/6
     *
     * @param  size 分辨率.
     *
     * @returns 成功返回true.
     */
    bool SwitchParam(cv::Size size);

    /**
     * 得到一个分辨率的参数.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  size The size.
     *
     * @returns The parameter.
     *
     * ### remarks Dx, 2020/5/6.
     */
    pStereoCameraParam getParam(cv::Size size);

    /**
     * 根据一个相机文件载入尝试相机参数信息.
     *
     * @author daixian
     * @date 2020/5/3
     *
     * @param  path            参数文件的完整路径.
     * @param  nameTargetSize  (Optional) Size of the name target.
     * @param  nameCamMatrixL  (Optional) The name camera matrix l.
     * @param  nameDistCoeffsL (Optional) The name distance coeffs l.
     * @param  nameCamMatrixR  (Optional) The name camera matrix r.
     * @param  nameDistCoeffsR (Optional) The name distance coeffs r.
     * @param  nameR           (Optional) The name r.
     * @param  nameT           (Optional) The name t.
     * @param  nameE           (Optional) The name.
     * @param  nameF           (Optional) The name f.
     * @param  nameQ           (Optional) The name q.
     * @param  nameR1          (Optional) The first name r.
     * @param  nameP1          (Optional) The first name p.
     * @param  nameR2          (Optional) The second name r.
     * @param  nameP2          (Optional) The second name p.
     */
    void loadParam(const std::string& path, const std::string& nameTargetSize = "targetSize",
                   const std::string& nameCamMatrixL = "M1",
                   const std::string& nameDistCoeffsL = "D1",
                   const std::string& nameCamMatrixR = "M2",
                   const std::string& nameDistCoeffsR = "D2",
                   const std::string& nameR = "R",
                   const std::string& nameT = "T",
                   const std::string& nameE = "E",
                   const std::string& nameF = "F",
                   const std::string& nameQ = "Q",
                   const std::string& nameR1 = "R1",
                   const std::string& nameP1 = "P1",
                   const std::string& nameR2 = "R2",
                   const std::string& nameP2 = "P2");
};
typedef std::shared_ptr<StereoCamera> pStereoCamera;

} // namespace dxlib