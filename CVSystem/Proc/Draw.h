#pragma once

#include <opencv2/opencv.hpp>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 用来在调试的时候画图表 . </summary>
///
/// <remarks> Dx, 2017/7/26. </remarks>
///-------------------------------------------------------------------------------------------------
class Draw
{
  private:
    /// <summary> The instance. </summary>
    static Draw* m_pInstance;

    /// <summary> 机画面是640x360的时候,合适的k值是2.95. </summary>
    float k;

    /// <summary> 画面宽度. </summary>
    int width;

    /// <summary> 画面高度. </summary>
    int height;

    /// <summary>要开启的窗口名字. </summary>
    //std::string winName;

    /// <summary> 保存图片到硬盘时的名字计数. </summary>
    int nameCount;

    /// <summary> 按行写文本时起始位置记录. </summary>
    //cv::Point point_line = cv::Point(0, 10);

    /// <summary> 一个图片队列. </summary>
    //std::deque<cv::Mat> dqDiagram;

    //隐藏成员字段
    struct Fields;

    /// <summary> 数据成员. </summary>
    Fields* _m = nullptr;

  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> 构造函数. </summary>
    ///
    /// <remarks> Dx, 2017/7/26. </remarks>
    ///
    /// <param name="width">  画布宽. </param>
    /// <param name="height"> 画布高. </param>
    /// <param name="k">      比例k值. </param>
    ///-------------------------------------------------------------------------------------------------
    Draw(int width, int height, float k);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Destructor. </summary>
    ///
    /// <remarks> Dx, 2019/1/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    ~Draw();

    /// <summary> 单例. </summary>
    static Draw* GetInst();

    ///// <summary> 保存文件路径. </summary>
    std::string& filePath();

    ///// <summary> 当前图片. </summary>
    cv::Mat& diagram();

    /// <summary> 内存保存图像上限. </summary>
    int memSavelimit = 4;

    ///// <summary> 预设的用来画相机的图像的roi. </summary>
    std::vector<cv::Rect>& vImageROI();

    /// <summary> 是否使能画图. </summary>
    bool isEnableDraw = true;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到k的值. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <returns> The k. </returns>
    ///-------------------------------------------------------------------------------------------------
    float get_k();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 根据一个相机的图像size来设置合适的k. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="cameraSize"> Size of the camera. </param>
    ///-------------------------------------------------------------------------------------------------
    void set_k(cv::Size cameraSize);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设置画图片用的roi. </summary>
    ///
    /// <remarks> Dx, 2018/11/27. </remarks>
    ///
    /// <param name="size">  画面大小. </param>
    /// <param name="count"> 预备的数量. </param>
    ///-------------------------------------------------------------------------------------------------
    void setImageROI(cv::Size size, int count = 4);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个矩形. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="rect">  The rectangle. </param>
    /// <param name="color"> (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawRectangle(cv::Rect2f rect, cv::Scalar color = cv::Scalar(0, 255, 0));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个旋转矩形. </summary>
    ///
    /// <remarks> Dx, 2018/11/27. </remarks>
    ///
    /// <param name="rect">  The rectangle. </param>
    /// <param name="color"> (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawRectangle(cv::RotatedRect rect, cv::Scalar color = cv::Scalar(0, 255, 0));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画线. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="point1"> The first point. </param>
    /// <param name="point2"> The second point. </param>
    /// <param name="color">  (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawLine(cv::Point2f point1, cv::Point2f point2, cv::Scalar color = cv::Scalar(0, 0, 255));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个点. </summary>
    ///
    /// <remarks> Dx, 2018/11/28. </remarks>
    ///
    /// <param name="point1"> The first point. </param>
    /// <param name="color">  (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawPoint(cv::Point2f point, cv::Scalar color = cv::Scalar(0, 0, 255));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画十字线. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="point1"> The first point. </param>
    /// <param name="color">  (Optional) The color. </param>
    /// <param name="size">   (Optional) 十字线的长度(5像素*size长). </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawCross(cv::Point2f point, cv::Scalar color = cv::Scalar(0, 0, 255), float size = 1);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Draw text. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="text">  The text. </param>
    /// <param name="org">   [in] 起始点原点. </param>
    /// <param name="color"> (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawText(const std::string text, const cv::Point& org, cv::Scalar color = cv::Scalar(0, 0, 255));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 在图上另起一行画一行文本，通常使用这个函数. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="text">  The text. </param>
    /// <param name="color"> (Optional) The color. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawTextLine(const std::string text, cv::Scalar color = cv::Scalar(0, 0, 255));

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个图片. </summary>
    ///
    /// <remarks> Dx, 2017/8/17. </remarks>
    ///
    /// <param name="src"> [in] 源图片. </param>
    /// <param name="roi"> 目标的roi. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawMat(cv::Mat& src, cv::Rect roi);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个图片，输入两个roi. </summary>
    ///
    /// <remarks> Dx, 2017/8/17. </remarks>
    ///
    /// <param name="src">     [in] 源图片. </param>
    /// <param name="roi_src"> The roi. </param>
    /// <param name="roi_dst"> The roi. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawMat(cv::Mat& src, cv::Rect roi_src, cv::Rect roi_dst);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 画一个图片，输入一个目标区域的index就可以了. </summary>
    ///
    /// <remarks> Dx, 2018/11/27. </remarks>
    ///
    /// <param name="src">   [in] 源图片. </param>
    /// <param name="index"> 目标区域的index. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    cv::Mat drawMat(cv::Mat& src, int index);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 显示窗口. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="winName"> 窗口名字. </param>
    /// <param name="x">       窗口位置x. </param>
    /// <param name="y">       窗口位置y. </param>
    ///-------------------------------------------------------------------------------------------------
    void showWin(std::string winName, int x = 800, int y = 0);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭窗口. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void closeWin();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 保存当前帧在内存. </summary>
    ///
    /// <remarks> dx, 2017/10/2. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void saveToMemory();

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 保存内存图片到文件.
    /// </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void saveMemImgToFile();

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 写当前图片像素Raw到内存的某个位置.
    /// </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="buff"> [in] 内存的目标位置. </param>
    ///-------------------------------------------------------------------------------------------------
    void writeMemImgToMem(void* buff);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// Clears this object to its blank/initial state.
    /// </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void clear();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 清空内存里保存的图片. </summary>
    ///
    /// <remarks> Dx, 2018/12/4. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void clearMemory();
};
} // namespace dxlib