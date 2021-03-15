#pragma once
#include <opencv2/opencv.hpp>
#include <memory>

#include "CameraParam.h"
#include "CameraDevice.h"

namespace dxlib {

/**
 * 一个逻辑计算相机.
 *
 * @author daixian
 * @date 2018/11/6
 */
class Camera
{
  public:
    /**
     * 构造.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  aName 相机名.
     * @param  aSize (Optional) 相机大小.
     */
    Camera(const std::string& aName, const cv::Size& aSize = cv::Size(640, 360));

    /**
     * 析构.
     *
     * @author daixian
     * @date 2020/5/21
     */
    virtual ~Camera();

    // 相机的逻辑编号0-3，它代表一个编程逻辑上的编号，不是相机的设备index.
    int camIndex = -1;

    // 这个相机的名字.
    std::string name;

    // 相机的图像分辨率size.
    cv::Size size;

    // 相机的当前参数对应的分辨率size,默认和相机size相同,读参数的时候注意修改设置.
    cv::Size paramSize;

    // 相机的内参数矩阵.
    cv::Mat camMatrix;

    // 畸变多项式系数.
    cv::Mat distCoeffs;

    // (因为一个相机可以对应多组立体相机组,所以这个属性应该去掉)相机的投影矩阵,可由立体相机的RT得到.
    cv::Mat projection;

    // 校正畸变的参数R.
    cv::Mat R;

    // 校正畸变的参数P.
    cv::Mat P;

    // remap函数使用的.
    cv::Mat rmap1;

    // remap函数使用的.
    cv::Mat rmap2;

    // 相机3d空间到某世界空间的变换矩阵.
    cv::Mat camTR4x4;

    // 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec3d camPos;

    // 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用).
    cv::Vec4d camRotate;

    // 这是相机采图的帧率.
    float FPS = 0;

    // 用户使用的附加对象.
    void* userData;

    // 产生这个相机图像的设备
    CameraDevice* device = nullptr;

#pragma region 简单函数

    /**
     * 把一个点统一大小成参数分辨率大小
     *
     * @author daixian
     * @date 2021/3/15
     *
     * @param  imagePoint 图片上的一点坐标.
     * @param  imageSize  图片大小.
     *
     * @returns A cv::Point2f.
     */
    cv::Point2f uniformImageSize(const cv::Point2f imagePoint, const cv::Size& imageSize)
    {
        float kx = paramSize.width / (float)imageSize.width; //如果target比原图小一半那么就是1/2
        float ky = paramSize.height / (float)imageSize.height;

        return cv::Point2f(imagePoint.x * kx, imagePoint.y * kx);
    }

    /**
     * 根据现有参数初始化rmap1和rmap2.
     *
     * @author daixian
     * @date 2020/5/21
     */
    void initUndistortRectifyMap();

    /**
     * 将屏幕上的一点根据内参矩阵映射到世界空间中的一点,这个方法配合Charuco使用, 
     * 目前发现将M矩阵的焦距中心放到正中, 把所有的畸变参数都去掉了,它计算合并的比较准.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  screenPoint 屏幕上的一点.
     * @param  z           (Optional) 一般为1.
     *
     * @returns A cv::Point3d.
     */
    cv::Point3d screenToWorld(const cv::Point2f& screenPoint, float z = 1);

#pragma endregion

#pragma region 立体相机扩展

    // 立体相机对的序号(区分多组立体相机).
    int scID = -1;

    // 双目相机里的L相机的camIndex.
    int stereoCamIndexL = -1;

    // 双目相机里的R相机的camIndex.
    int stereoCamIndexR = -1;

    // 双目相机里的另一对相机.(这个应该作为一个函数在StereoCamera中调用)
    std::shared_ptr<Camera> stereoOther = nullptr;

    // 是否是立体相机里的一个
    bool isStereo() const
    {
        if (scID >= 0) {
            return true;
        }
        return false;
    }

    // 是否是立体相机的左相机
    bool isStereoL() const
    {
        if (stereoCamIndexL == camIndex) {
            return true;
        }
        return false;
    }

    // 是否是立体相机的右相机
    bool isStereoR() const
    {
        if (stereoCamIndexR == camIndex) {
            return true;
        }
        return false;
    }

#pragma endregion

    // TODO:后面准备废弃这些内容
    // 这是相机的备用参数，一个相机可以记录多组参数.
    std::vector<std::pair<cv::Size, pCameraParam>> vParams;

    /**
     * 切换应用这个分辨率的参数.
     *
     * @author daixian
     * @date 2020/5/6
     *
     * @param  size 分辨率.
     *
     * @returns 成功返回true.
     */
    bool SwitchParam(const cv::Size& size);

    /**
     * 得到一个分辨率的参数.
     *
     * @author daixian
     * @date 2020/5/6
     *
     * @param  size The size.
     *
     * @returns 相机参数.
     */
    pCameraParam getParam(const cv::Size& size);

    /**
     * 根据一个相机文件载入尝试相机参数信息,会记录到上面的vParams里.
     *
     * @author daixian
     * @date 2020/5/3
     *
     * @param  path           参数文件的完整路径.
     * @param  paramSize      (Optional) 参数目标分辨率的字段名.
     * @param  nameCamMatrix  (Optional) 相机内参数矩阵的字段名.
     * @param  nameDistCoeffs (Optional) 相机畸变参数的字段名.
     * @param  isApply        (Optional) 是否把这个参数信息应用到成员变量中.
     */
    void loadParam(const std::string& path, const std::string& paramSize = "paramSize",
                   const std::string& nameCamMatrix = "M",
                   const std::string& nameDistCoeffs = "D", bool isApply = true);

  private:
    class Impl;
    Impl* _impl = nullptr;
};
// 定义这个智能指针类型.
typedef std::shared_ptr<Camera> pCamera;

} // namespace dxlib