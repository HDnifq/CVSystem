#pragma once
#include <opencv2/opencv.hpp>

//所有的CV_CAP_PROP的数组的长度
#define DXLIB_CAMERA_CAP_PROP_LEN 128

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 一个相机的参数，这个类应该记录一个相机所有的信息. </summary>
///
/// <remarks> Dx, 2018/11/6. </remarks>
///-------------------------------------------------------------------------------------------------
class Camera
{
  public:
    /// <summary> 构造. </summary>
    Camera(int aCamIndex, std::wstring aDevName, cv::Size aSize = cv::Size(640, 360), int aBrightness = -64);

    /// <summary> 相机的编号0-3. </summary>
    int camIndex = -2;

    /// <summary> 这个相机的设备名. </summary>
    std::wstring devName;

    /// <summary> 非宽字符的设备名. </summary>
    std::string devNameA;

    /// <summary>
    /// 是否是一个不存在的虚拟的相机，仅仅只是利用这个录入参数，然后载入图像计算.
    /// (即如果设置了它为true，那么MultiCamera在根据CameraManger里录入的内容来open相机的时候不会自动的打开它).
    /// </summary>
    bool isVirtualCamera = false;

    /// <summary> 设备的id顺序. </summary>
    int devID = -1;

    /// <summary> 相机的分辨率size. </summary>
    cv::Size size;

    /// <summary> 相机的参数对应的分辨率size,默认和相机size相同,读参数的时候注意修改设置. </summary>
    cv::Size paramSize;

    /// <summary> 相机的内参数矩阵. </summary>
    cv::Mat camMatrix;

    /// <summary> The distance coeffs. </summary>
    cv::Mat distCoeffs;

    /// <summary> 校正畸变的参数R. </summary>
    cv::Mat R;

    /// <summary> 校正畸变的参数P. </summary>
    cv::Mat P;

    /// <summary> remap函数使用的. </summary>
    cv::Mat rmap1;

    /// <summary> remap函数使用的. </summary>
    cv::Mat rmap2;

    /// <summary> 校正畸变之后的相机矩阵. </summary>
    //cv::Mat camMatrix_new;

    /// <summary> 相机3d空间到某世界空间的变换矩阵. </summary>
    cv::Mat camRT4x4;

    /// <summary> 双目相机里的另一对相机. </summary>
    std::shared_ptr<Camera> stereoOther = nullptr;

    /// <summary> 用户使用的附加对象. </summary>
    void* userData;

#pragma region 工作状态

    /// <summary>
    /// 相机的capture，这里基本应该是只读的。
    /// </summary>
    std::shared_ptr<cv::VideoCapture> capture = nullptr;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开这个相机. </summary>
    ///
    /// <remarks> Dx, 2019/1/12. </remarks>
    ///
    /// <returns> 打开成功返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool openCamera();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭相机设备. </summary>
    ///
    /// <remarks> Dx, 2019/1/12. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void releaseCamera();

    /// <summary> 相机是否打开了. </summary>
    bool isOpened()
    {
        if (capture == nullptr) {
            return false;
        }
        else {
            return capture->isOpened();
        }
    }

    /// <summary> 暂存的上次设置的结果属性值,可以供随便的查询一下,但是一般用不到. </summary>
    double lastCapProp[DXLIB_CAMERA_CAP_PROP_LEN];

    /// <summary> 设置当前的相机的属性. </summary>
    void setProp(int CV_CAP_PROP, double value)
    {
        _setCapProp[CV_CAP_PROP] = value;
    }

    /// <summary> 这是相机采图的帧率. </summary>
    float FPS = 0;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 由采图线程去应用相机的属性设置. </summary>
    ///
    /// <remarks> Dx, 2019/1/11. </remarks>
    ///
    /// <returns> 如果设置成功返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool applyCapProp();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 在控制台输出当前实际的相机状态. </summary>
    ///
    /// <remarks> Dx, 2019/1/11. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void outputProp();
    void outputAllProp();

#pragma endregion

#pragma region 简单函数

    /// <summary> 根据现有参数初始化rmap1和rmap2. </summary>
    void initUndistortRectifyMap();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从double值转换到4个字节的FOURCC. </summary>
    ///
    /// <remarks> Dx, 2019/1/11. </remarks>
    ///
    /// <param name="FOURCC"> The fourcc. </param>
    ///
    /// <returns> FOURCC as a std::string. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::string toFOURCC(double FOURCC);

#pragma endregion

  private:
    /// <summary> 用来做状态的记录，好触发设置动作. </summary>
    double _setCapProp[DXLIB_CAMERA_CAP_PROP_LEN];
};
/// <summary>定义这个智能指针类型. </summary>
typedef std::shared_ptr<Camera> pCamera;

///-------------------------------------------------------------------------------------------------
/// <summary> 立体相机. </summary>
///
/// <remarks> Dx, 2018/11/23. </remarks>
///-------------------------------------------------------------------------------------------------
class StereoCamera
{
  public:
    /// <summary> 立体相机对的序号(区分多组立体相机). </summary>
    int scID;

    /// <summary> The camera l. </summary>
    pCamera camL;

    /// <summary> The camera r. </summary>
    pCamera camR;

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

    /// <summary> 相机3d空间到某世界空间的变换矩阵,它应该等于camL里的的相机camRT4x4. </summary>
    cv::Mat camRT4x4;

    /// <summary> 相机在世界空间的坐标(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    std::array<double, 3> camPos;

    /// <summary> 相机在世界空间的旋转(可以从上面的camRT4x4求出,放在这里方便使用). </summary>
    std::array<double, 4> camRotate;

    /// <summary> 是否L相机的点的Y值在上面（更小）用于匹配. </summary>
    bool LYisAbove = true;
};
typedef std::shared_ptr<StereoCamera> pStereoCamera;

} // namespace dxlib