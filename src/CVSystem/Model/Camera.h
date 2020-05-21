#pragma once
#include <opencv2/opencv.hpp>
#include <memory>

#include "CameraParam.h"

namespace dxlib {

/**
 * 一个相机的参数，这个类应该记录一个相机所有的信息.
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
     * @param  aDevName    相机名.
     * @param  aSize       (Optional) 相机大小.
     * @param  aBrightness (Optional) 相机亮度.
     */
    Camera(std::string aDevName, cv::Size aSize = cv::Size(640, 360), int aBrightness = -64);

    /**
     * 析构.
     *
     * @author daixian
     * @date 2020/5/21
     */
    ~Camera();

    /** 相机的逻辑编号0-3，它代表一个编程逻辑上的编号，不是相机的设备index. */
    int camIndex = -1;

    /** 这个相机的设备名. */
    std::string devName;

    /** 是否是一个不存在的虚拟的相机（参数计算相机），仅仅只是利用这个录入参数，然后载入图像计算.
    (即如果设置了它为true，那么MultiCamera在根据CameraManger里录入的内容来open相机的时候不会自动的打开它). */
    bool isVirtualCamera = false;

    /** 如果它是一个虚拟相机,那么它可能是一个立体相机的一半,这里则记录它的实际物理相机的名字. */
    std::string physicalDevName;

    /** (这个是用户只读的)设备的id顺序，物理硬件上的index. */
    int devID = -1;

    /** 相机的分辨率size，打开相机之后会按照这个设置来尝试设置相机分辨率. */
    cv::Size size;

    /** 相机的参数对应的分辨率size,默认和相机size相同,读参数的时候注意修改设置. */
    cv::Size paramSize;

    /** 相机的内参数矩阵. */
    cv::Mat camMatrix;

    /** 畸变多项式系数. */
    cv::Mat distCoeffs;

    /** (因为一个相机可以对应多组立体相机组,所以这个属性应该去掉)相机的投影矩阵,可由立体相机的RT得到. */
    cv::Mat projection;

    /** 校正畸变的参数R. */
    cv::Mat R;

    /** 校正畸变的参数P. */
    cv::Mat P;

    /** remap函数使用的. */
    cv::Mat rmap1;

    /** remap函数使用的. */
    cv::Mat rmap2;

    /** 相机3d空间到某世界空间的变换矩阵. */
    cv::Mat camTR4x4;

    /** 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用). */
    cv::Vec3d camPos;

    /** 相机在世界空间的旋转(x,y,z,w)(可以从上面的camRT4x4求出,放在这里方便使用). */
    cv::Vec4d camRotate;

    /** 双目相机里的另一对相机.(这个应该作为一个函数在StereoCamera中调用) */
    std::shared_ptr<Camera> stereoOther = nullptr;

    /** 实际的物理相机. */
    std::shared_ptr<Camera> physicalCamera = nullptr;

    /** 这是相机采图的帧率. */
    float FPS = 0;

    /** 用户使用的附加对象. */
    void* userData;

    /** 相机的capture，这里基本应该是只读的。 */
    std::shared_ptr<cv::VideoCapture> capture = nullptr;

    /**
     * 打开这个相机.
     *
     * @author daixian
     * @date 2019/1/12
     *
     * @returns 打开成功返回true.
     */
    bool open();

    /**
     * 采一张图片.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param [in,out] image The image to read.
     *
     * @returns True if it succeeds, false if it fails.
     */
    bool read(cv::Mat& image);

    /**
     * 关闭相机设备.
     *
     * @author daixian
     * @date 2020/5/21
     */
    void release();

    /**
     * 相机是否打开了,虚拟相机这里会返回一个false.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns 打开了就返回true,没打开返回false.
     */
    bool isOpened()
    {
        if (capture == nullptr) {
            return false;
        }
        else {
            return capture->isOpened();
        }
    }

#pragma region 相机属性

    /**
     * 设置当前的相机的一项属性，注意第二个参数value是double类型.
     *
     * @author daixian
     * @date 2019/5/14
     *
     * @param  CV_CAP_PROP 要设置的属性.
     * @param  value       The value.
     */
    void setProp(cv::VideoCaptureProperties CV_CAP_PROP, double value);

    /**
     * 设置当前的相机的一项属性.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param  CV_CAP_PROP The cv capability property.
     * @param  value       一个字符串值，由这个函数进行处理.
     */
    void setPropWithString(cv::VideoCaptureProperties CV_CAP_PROP, std::string value);

    /**
     * 设置相机的fourcc.
     *
     * @author daixian
     * @date 2020/3/30
     *
     * @param  fourcc The fourcc.
     */
    void setFourcc(const std::string& fourcc);

    /**
     * 由采图线程去应用相机的属性设置.
     *
     * @author daixian
     * @date 2019/1/11
     *
     * @returns 如果设置成功返回true.
     */
    bool applyCapProp();

    /**
     * 用比较紧凑的格式在控制台输出当前实际的相机状态, 在控制台两行信息只输出一些主要信息.同时为了方便输出json,将他们成对返回.
     *
     * @author daixian
     * @date 2019/1/11
     *
     * @returns 成对返回的属性文本;
     */
    std::map<std::string, std::string> outputProp();

    /**
     * 一行一个属性的输出所有的属性.
     *
     * @author daixian
     * @date 2019/5/14
     *
     * @returns 成对返回的属性文本;
     */
    std::map<std::string, std::string> outputAllProp();

    /**
     * 属性字符串转成枚举，如果返回cv::CV__CAP_PROP_LATEST则表示失败了.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param  propertiesStr 属性的字符串.
     *
     * @returns cv::VideoCaptureProperties枚举类型.
     */
    static cv::VideoCaptureProperties propStr2Enum(const std::string& propertiesStr);

    /**
     * 得到相机的属性的字符串文本map.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @returns 相机的属性的字符串文本map.
     */
    static std::map<cv::VideoCaptureProperties, const char*> propStringMap();

#pragma endregion

#pragma region 简单函数

    /**
     * 根据现有参数初始化rmap1和rmap2.
     *
     * @author daixian
     * @date 2020/5/21
     */
    void initUndistortRectifyMap();

    /**
     * 从double值转换到4个字节的FOURCC.
     *
     * @author daixian
     * @date 2019/1/11
     *
     * @param  FOURCC double类型的fourcc.
     *
     * @returns FOURCC的字符串.
     */
    std::string toFOURCC(double FOURCC);

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
    cv::Point3d screenToWorld(cv::Point2f screenPoint, float z = 1);

#pragma endregion

#pragma region 特殊相机扩展

    /**
     * 相机的一些特殊安装形式,如安装时翻转了180度等等.
     */
    enum class SpecialTpye
    {
        // 正常.
        None,
        // 旋转了180度.
        Rotate180,
    };

    /** 相机的特殊类型. */
    SpecialTpye specialTpye = SpecialTpye::None;

    /** 是否在采图的时候自动处理相机的特殊类型，如采图的时候直接翻转180度，则后面处理逻辑不需要处理. */
    bool isAutoProcSpecialTpye = true;

#pragma endregion

#pragma region 立体相机扩展
    //// 当这个相机是一个硬件上的双目相机的时候,那么先录入所有的逻辑相机,然后再创建这个立体相机

    /** 这个相机是否是一个立体相机，它标记着会去自动切割这个立体相机的图像，将它们转到对应的逻辑相机. */
    bool isStereoCamera = false;

    /** 是否不要传图到后面给Proc,如果设置它为true,那么需要把这个相机的index放到后面. */
    bool isNoSendToProc = false;

    /** 立体相机对的序号(区分多组立体相机). */
    int scID = -1;

    /** 双目相机里的L相机的camIndex. */
    int stereoCamIndexL = -1;

    /** 双目相机里的R相机的camIndex. */
    int stereoCamIndexR = -1;

#pragma endregion

    /** 这是相机的备用参数，一个相机可以记录多组参数. */
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
    bool SwitchParam(cv::Size size);

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
    pCameraParam getParam(cv::Size size);

    /**
     * 根据一个相机文件载入尝试相机参数信息,会记录到上面的vParams里.
     *
     * @author daixian
     * @date 2020/5/3
     *
     * @param  path           参数文件的完整路径.
     * @param  nameTargetSize (Optional) 参数目标分辨率的字段名.
     * @param  nameCamMatrix  (Optional) 相机内参数矩阵的字段名.
     * @param  nameDistCoeffs (Optional) 相机畸变参数的字段名.
     * @param  isApply        (Optional) 是否把这个参数信息应用到成员变量中.
     */
    void loadParam(const std::string& path, const std::string& nameTargetSize = "targetSize",
                   const std::string& nameCamMatrix = "M",
                   const std::string& nameDistCoeffs = "D", bool isApply = true);

  private:
    // 隐藏成员字段.
    class Impl;

    /** 数据成员. */
    Impl* _impl = nullptr;
};
/** 定义这个智能指针类型. */
typedef std::shared_ptr<Camera> pCamera;

} // namespace dxlib