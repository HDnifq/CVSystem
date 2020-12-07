#pragma once
#include <opencv2/opencv.hpp>

namespace dxlib {

/**
 * 一个实际的相机硬件设备,它有采图功能.
 *
 * @author daixian
 * @date 2020/11/6
 */
class CameraDevice
{
  public:
    CameraDevice(const std::string& aDevName, cv::Size aSize, int aBrightness);

    ~CameraDevice();

    // 这个设备的id,也是ImageFactory的id.
    int id = -1;

    // 这个相机的设备名.
    std::string devName;

    // (这个是用户只读的)设备的id顺序，物理硬件上的index.
    int devID = -1;

    // 相机的分辨率size，打开相机之后会按照这个设置来尝试设置相机分辨率.
    cv::Size size;

    // 相机的capture，这里基本应该是只读的。
    std::shared_ptr<cv::VideoCapture> capture = nullptr;

    // 这是相机采图的帧率.
    float FPS = 0;

    /**
     * 打开这个相机.
     *
     * @author daixian
     * @date 2019/1/12
     *
     * @returns 打开成功返回true.
     */
    bool open(float& costTime);

    /**
     * 打开这个相机
     *
     * @author daixian
     * @date 2020/11/12
     *
     * @returns 打开成功返回true.
     */
    bool open();

    /**
     * 采一张图片,这个函数在采图的时候是阻塞的.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param [in,out] image 需要采集的图片.
     *
     * @returns 成功采集返回true.
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
    bool isOpened();

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

#pragma region 特殊相机扩展

    // 如果这个物理相机是立体相机的采图相机,那么这里需要记录一下scID;
    int scID = -1;

    // 相机的一些特殊安装形式,如安装时翻转了180度等等.
    enum class SpecialTpye
    {
        // 正常.
        None,
        // 旋转了180度.
        Rotate180,
    };

    // 相机的特殊类型.
    SpecialTpye specialTpye = SpecialTpye::None;

    // 是否在采图的时候自动处理相机的特殊类型，如采图的时候直接翻转180度，则后面处理逻辑不需要处理.
    bool isAutoProcSpecialTpye = true;

#pragma endregion

#pragma region 简单函数

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

#pragma endregion

  private:
    class Impl;
    Impl* _impl = nullptr;
};
// 定义这个智能指针类型.
typedef std::shared_ptr<CameraDevice> pCameraDevice;

} // namespace dxlib