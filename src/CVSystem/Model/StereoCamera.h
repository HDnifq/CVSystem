#pragma once
#include <opencv2/opencv.hpp>
#include <memory>
#include "Camera.h"
#include "StereoCameraParam.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 立体相机(实际上它叫StereoCameraInfo更贴切). </summary>
///
/// <remarks> Dx, 2018/11/23. </remarks>
///-------------------------------------------------------------------------------------------------
class StereoCamera
{
  public:
    /// <summary> 立体相机对的序号(区分多组立体相机). </summary>
    int scID = -1;

    /// <summary> 这个立体相机组的名字. </summary>
    std::string name;

    /// <summary> The camera l. </summary>
    pCamera camL;

    /// <summary> The camera r. </summary>
    pCamera camR;

    /// <summary> 实际的物理双目相机. </summary>
    pCamera camPhy;

    /// <summary> 参数R. </summary>
    cv::Mat R;

    /// <summary> 参数T. </summary>
    cv::Mat T;

    /// <summary> 参数E. </summary>
    cv::Mat E;

    /// <summary> 参数F. </summary>
    cv::Mat F;

    /// <summary> 参数Q. </summary>
    cv::Mat Q;

    /// <summary> 左相机的投影矩阵. </summary>
    cv::Mat LP;

    /// <summary> 右相机的投影矩阵. </summary>
    cv::Mat RP;

    /// <summary> 这组相机3d空间到某世界空间的变换矩阵,它应该等于camL里的的相机camTR4x4. </summary>
    cv::Mat camTR4x4;

    /// <summary> 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec3d camPos;

    /// <summary> 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    cv::Vec4d camRotate;

    /// <summary> 是否是垂直的立体相机,如果它为false则默认它是水平的立体相机. </summary>
    bool isVertical = false;

    /// <summary> 是否L相机的点的Y值在上面（更小）用于匹配(这个属性实际不是绝对可靠). </summary>
    bool LYisAbove = true;

    /// <summary> 是否L相机的点值X比R相机的X值小(这个属性实际不是绝对可靠). </summary>
    bool isLXLessThanR = true;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 设置左右相机，当没有物理相机或者。两个全是真实相机的时候就可以用这个.
    /// </summary>
    ///
    /// <remarks> Dx, 2020/3/27. </remarks>
    ///
    /// <param name="camL"> [in] The camera l. </param>
    /// <param name="camR"> [in] The camera r. </param>
    ///-------------------------------------------------------------------------------------------------
    void setCameraLR(pCamera& camL, pCamera& camR)
    {
        this->camL = camL;
        this->camR = camR;
        this->camL->stereoOther = this->camR;
        this->camR->stereoOther = this->camL;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 使用一个实际的物理相机和LR相机一起设置个stereo信息. </summary>
    ///
    /// <remarks> Dx, 2020/4/16. </remarks>
    ///
    /// <param name="camPhy"> [in] The camera phy. </param>
    /// <param name="camL">   [in] The camera l. </param>
    /// <param name="camR">   [in] The camera r. </param>
    ///-------------------------------------------------------------------------------------------------
    void setCameraPhyLR(pCamera& camPhy, pCamera& camL, pCamera& camR);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置投影矩阵. </summary>
    ///
    /// <remarks> Dx, 2020/4/14. </remarks>
    ///
    /// <param name="LP"> L相机的投影矩阵. </param>
    /// <param name="RP"> R相机的投影矩阵. </param>
    ///-------------------------------------------------------------------------------------------------
    void setProjection(const cv::Mat& LP, const cv::Mat& RP)
    {
        this->LP = LP;
        this->RP = RP;
        this->camL->projection = LP;
        this->camR->projection = RP;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到一个相机的数组，index=0表示L相机，index=1表示R相机. </summary>
    ///
    /// <remarks> Dx, 2020/3/30. </remarks>
    ///
    /// <returns> The cameras. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::array<pCamera, 2> getCameras()
    {
        std::array<pCamera, 2> arr = {camL, camR};
        return arr;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 这是相机的备用参数，一个相机可以记录多组参数.
    /// </summary>
    ///-------------------------------------------------------------------------------------------------
    std::vector<std::pair<cv::Size, pStereoCameraParam>> vParams;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 切换应用这个分辨率的参数. </summary>
    ///
    /// <remarks> Dx, 2020/5/6. </remarks>
    ///
    /// <param name="size"> 分辨率. </param>
    ///
    /// <returns> 成功返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool SwitchParam(cv::Size size);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到一个分辨率的参数. </summary>
    ///
    /// <remarks> Dx, 2020/5/6. </remarks>
    ///
    /// <param name="size"> The size. </param>
    ///
    /// <returns> The parameter. </returns>
    ///-------------------------------------------------------------------------------------------------
    pStereoCameraParam getParam(cv::Size size);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 根据一个相机文件载入尝试相机参数信息. </summary>
    ///
    /// <remarks> Surface, 2020/5/3. </remarks>
    ///
    /// <param name="path"> 参数文件的完整路径. </param>
    ///-------------------------------------------------------------------------------------------------
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