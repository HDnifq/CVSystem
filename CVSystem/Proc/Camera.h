#pragma once

#include <opencv2/opencv.hpp>
#include "../dlog/dlog.h"

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
        /// 构造capture的时候就要重置resetProp().
        /// </summary>
        std::shared_ptr<cv::VideoCapture> capture = nullptr;

        /// <summary> 相机是否打开了. </summary>
        bool isOpened()
        {
            if (capture == nullptr) {
                return false;
            } else {
                return capture->isOpened();
            }

        }

        /// <summary> 相机的一些属性. </summary>
        struct CAP_PROP
        {
            double BRIGHTNESS;
            double FOURCC;
            double FPS;
            double FRAME_HEIGHT;
            double FRAME_WIDTH;
            double AUTO_EXPOSURE;
            double FOCUS;
            double EXPOSURE;
        };

        /// <summary> 当前的相机属性(外面用来只读). </summary>
        CAP_PROP curProp;

        /// <summary> 设置当前的相机的属性. </summary>
        CAP_PROP& setProp()
        {
            return prepareProp;
        }

        /// <summary> 重置当前的相机属性. </summary>
        void resetProp();

        /// <summary> 这是相机采图的帧率. </summary>
        float FPS = 0;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 由采图线程去应用相机的属性设置. </summary>
        ///
        /// <remarks> Dx, 2019/1/11. </remarks>
        ///
        /// <returns> 如果设置成功返回true. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool applyCapProp()
        {
            if (capture == nullptr || !capture->isOpened()) {
                return false;
            }
            //******************** 这里考虑还是把curProp作为当前值，搞几个数组免得这么多代码
            //*****************一启动就读一遍相机属性
            //响应属性变化
            if (curProp.FOURCC != prepareProp.FOURCC) {
                curProp.FOURCC = prepareProp.FOURCC;
                std::string fourcc = toFOURCC(curProp.FOURCC);
                LogI("Camera.applyCapProp():设置相机%d -> FOURCC = %s", camIndex, fourcc.c_str());
                if (!capture->set(CV_CAP_PROP_FOURCC, curProp.FOURCC)) {
                    LogW("Camera.applyCapProp():设置FOURCC失败.");
                }
            }
            if (curProp.BRIGHTNESS != prepareProp.BRIGHTNESS) {
                curProp.BRIGHTNESS = prepareProp.BRIGHTNESS;
                LogI("Camera.applyCapProp():设置相机%d -> BRIGHTNESS = %d", camIndex, (int)curProp.BRIGHTNESS);
                if (!capture->set(CV_CAP_PROP_BRIGHTNESS, curProp.BRIGHTNESS)) {
                    LogW("Camera.applyCapProp():设置BRIGHTNESS失败.");
                }
            }
            if (curProp.FPS != prepareProp.FPS) {
                curProp.FPS = prepareProp.FPS;
                LogI("Camera.applyCapProp():设置相机%d -> FPS = %d", camIndex, (int)curProp.FPS);
                if (!capture->set(CV_CAP_PROP_FPS, curProp.FPS)) {
                    LogW("Camera.applyCapProp():设置FPS失败.");
                }
            }
            if (curProp.FRAME_HEIGHT != prepareProp.FRAME_HEIGHT) {
                curProp.FRAME_HEIGHT = prepareProp.FRAME_HEIGHT;
                LogI("Camera.applyCapProp():设置相机%d -> FRAME_HEIGHT = %d", camIndex, (int)curProp.FRAME_HEIGHT);
                if (!capture->set(CV_CAP_PROP_FRAME_HEIGHT, curProp.FRAME_HEIGHT)) {
                    LogW("Camera.applyCapProp():设置FRAME_HEIGHT失败.");
                }
            }
            if (curProp.FRAME_WIDTH != prepareProp.FRAME_WIDTH) {
                curProp.FRAME_WIDTH = prepareProp.FRAME_WIDTH;
                LogI("Camera.applyCapProp():设置相机%d -> RAME_WIDTH = %d", camIndex, (int)curProp.FRAME_WIDTH);
                if (!capture->set(CV_CAP_PROP_FRAME_WIDTH, curProp.FRAME_WIDTH)) {
                    LogW("Camera.applyCapProp():设置FRAME_WIDTH失败.");
                }
            }
            if (curProp.FOCUS != prepareProp.FOCUS) {
                curProp.FOCUS = prepareProp.FOCUS;
                LogI("Camera.applyCapProp():设置相机%d -> FOCUS = %f", camIndex, curProp.FOCUS);
                if (!capture->set(CV_CAP_PROP_FOCUS, curProp.FOCUS)) {
                    LogW("Camera.applyCapProp():设置FOCUS失败.");
                }

                int FOCUS = static_cast<int>(capture->get(CV_CAP_PROP_FOCUS));
                LogI("Camera.applyCapProp():当前相机%d实际值 -> FOCUS = %d", camIndex, FOCUS);
            }
            if (curProp.AUTO_EXPOSURE != prepareProp.AUTO_EXPOSURE) {
                curProp.AUTO_EXPOSURE = prepareProp.AUTO_EXPOSURE;
                //outputAllProp();
                LogI("Camera.applyCapProp():设置相机%d -> AUTO_EXPOSURE = %f", camIndex, curProp.AUTO_EXPOSURE);
                if (!capture->set(CV_CAP_PROP_AUTO_EXPOSURE, curProp.AUTO_EXPOSURE)) {
                    LogW("Camera.applyCapProp():设置AUTO_EXPOSURE失败.");
                }

                int AUTO_EXPOSURE = static_cast<int>(capture->get(CV_CAP_PROP_AUTO_EXPOSURE));
                LogI("Camera.applyCapProp():当前相机%d实际值 -> AUTO_EXPOSURE = %d", camIndex, AUTO_EXPOSURE);
                //outputAllProp();
            }
            if (curProp.EXPOSURE != prepareProp.EXPOSURE) {
                curProp.EXPOSURE = prepareProp.EXPOSURE;
                //outputAllProp();
                LogI("Camera.applyCapProp():设置相机%d -> EXPOSURE = %f", camIndex, curProp.EXPOSURE);
                if (!capture->set(CV_CAP_PROP_EXPOSURE, curProp.EXPOSURE)) {
                    LogW("Camera.applyCapProp():设置EXPOSURE失败.");
                }

                int EXPOSURE = static_cast<int>(capture->get(CV_CAP_PROP_EXPOSURE));
                LogI("Camera.applyCapProp():当前相机%d实际值 -> EXPOSURE = %d", camIndex, EXPOSURE);
                //outputAllProp();
            }
            return true;
        }

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
        /// <summary> 准备要设置的属性(外面用来写入). </summary>
        CAP_PROP prepareProp;
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

}