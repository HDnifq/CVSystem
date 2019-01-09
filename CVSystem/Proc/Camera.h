#pragma once

#include <string>
#include <opencv2/opencv.hpp>

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

        /// <summary> 设备的id顺序. </summary>
        int devID = -1;

        /// <summary> 相机的分辨率size. </summary>
        cv::Size size;

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

        /// <summary> 相机的capture，这里基本应该是只读的. </summary>
        std::shared_ptr<cv::VideoCapture> capture = nullptr;

        /// <summary> 这是相机采图的帧率. </summary>
        float FPS = 0;

        /// <summary> 亮度(只读). </summary>
        int brightness = 0;

        #pragma endregion

        #pragma region 简单函数

        /// <summary> 设置亮度. </summary>
        void setBrightness(int aBrightness, bool isForce = false);

        /// <summary> 根据现有参数初始化rmap1和rmap2. </summary>
        void initUndistortRectifyMap();

        #pragma endregion

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

        /// <summary> 是否L相机的点的Y值在上面（更小）用于匹配. </summary>
        bool LYisAbove = true;
    };
    typedef std::shared_ptr<StereoCamera> pStereoCamera;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 所有相机的参数，单例. </summary>
    ///
    /// <remarks> Dx, 2018/11/8. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class CameraManger
    {
    public:
        CameraManger();
        ~CameraManger();

        static CameraManger* GetInst();

        /// <summary> 所有相机的map，以camIndex为key. </summary>
        std::map<int, pCamera> camMap;

        /// <summary> 立体相机对. </summary>
        std::vector<pStereoCamera> vStereo;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 清空记录. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///-------------------------------------------------------------------------------------------------
        void clear();

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 添加一个相机，添加注意camIndex搞对. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///
        /// <param name="cp"> 要添加的相机. </param>
        ///-------------------------------------------------------------------------------------------------
        void add(pCamera cp);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 从文件里载入参数，这个函数将来应该做更灵活的操作. </summary>
        ///
        /// <remarks> Dx, 2018/11/12. </remarks>
        ///-------------------------------------------------------------------------------------------------
        //void load(std::string filePath);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 得到一个立体相机. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///
        /// <returns> The stereo. </returns>
        ///-------------------------------------------------------------------------------------------------
        pStereoCamera getStereo(pCamera camera);

    private:

        /// <summary> 单例. </summary>
        static CameraManger* m_pInstance;

        /// <summary> 初始化一个相机的Map. </summary>
        void initUndistortRectifyMap(pCamera& camera);
    };

}