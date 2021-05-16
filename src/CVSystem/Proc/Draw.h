#pragma once

#include <opencv2/opencv.hpp>

namespace dxlib {

/**
 * 用来在调试的时候画图表 .
 *
 * @author daixian
 * @date  2017/7/26
 */
class Draw
{
  private:
    // 单例.
    static Draw* m_pInstance;

    // 机画面是640x360的时候,合适的k值是2.95.
    float k;

    // 画面宽度.
    int width;

    // 画面高度.
    int height;

    // 保存图片到硬盘时的名字计数.
    int nameCount;

    // 隐藏成员字段.
    class Impl;

    // 数据成员.
    Impl* _impl = nullptr;

  public:
    /**
     * 构造函数.
     *
     * @author daixian
     * @date 2017/7/26
     *
     * @param  width  画布宽.
     * @param  height 画布高.
     * @param  k      比例k值.
     */
    Draw(int width, int height, float k);

    /**
     * Destructor
     *
     * @author daixian
     * @date 2019/1/10
     */
    ~Draw();

    /**
     * 单例.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns 指针.
     */

    static Draw* GetInst()
    {
        if (m_pInstance == nullptr) { //判断是否第一次调用
            //这里注意当前相机的分辨率
            m_pInstance = new Draw(1920, 1080, 2.95f); //当摄像机画面是640x360的时候,合适的k值是2.95
        }
        return m_pInstance;
    }

    /**
     * 当前图片.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns 当前图片的Mat.
     */
    cv::Mat& diagram();

    /** 内存保存图像上限. */
    int memSavelimit = 4;

    /**
     * 预设的用来画相机的图像的roi.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @returns std::vector<cv::Rect>
     */
    std::vector<cv::Rect>& vImageROI();

    /**
     * 获得roi.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  index 一个相机的index.
     *
     * @returns A cv::Rect.
     */
    cv::Rect imageROI(int index);

    /** 是否使能画图. */
    bool isEnableDraw = true;

    /**
     * 得到k的值.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @returns The k.
     */
    float get_k();

    /**
     * 根据一个相机的图像size来设置合适的k.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  cameraSize 相机的图像size.
     */
    void set_k(const cv::Size& cameraSize);

    /**
     * 设置画图片用的roi.
     *
     * @author daixian
     * @date 2018/11/27
     *
     * @param  size  画面大小.
     * @param  count (Optional) 预备的数量.
     */
    void setImageROI(const cv::Size& size, int count = 6);

    /**
     * 根据一个int去得到一个颜色.
     *
     * @author daixian
     * @date 2019/8/23
     *
     * @param  seed The seed.
     *
     * @returns 颜色.
     */
    cv::Scalar getColor(unsigned int seed);

    /**
     * 画一个矩形.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  rect  The rectangle.
     * @param  color (Optional) The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawRectangle(const cv::Rect2f& rect, const cv::Scalar& color = cv::Scalar(0, 255, 0));

    /**
     * 画一个旋转矩形.
     *
     * @author daixian
     * @date 2018/11/27
     *
     * @param  rect  The rectangle.
     * @param  color (Optional) The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawRectangle(const cv::RotatedRect& rect, const cv::Scalar& color = cv::Scalar(0, 255, 0));

    /**
     * 画线.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  point1 The first point.
     * @param  point2 The second point.
     * @param  color  (Optional) The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawLine(const cv::Point2f& point1, const cv::Point2f& point2, const cv::Scalar& color = cv::Scalar(0, 0, 255));

    /**
     * 画一个点.
     *
     * @author daixian
     * @date 2018/11/28
     *
     * @param  point The point.
     * @param  color (Optional) The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawPoint(const cv::Point2f& point, const cv::Scalar& color = cv::Scalar(0, 0, 255));

    /**
     * 画十字线.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  point 十字线中心.
     * @param  color (Optional) The color.
     * @param  size  (Optional) 十字线的长度(5像素*size长).
     *
     * @returns 当前图片.
     */
    cv::Mat& drawCross(const cv::Point2f& point, const cv::Scalar& color = cv::Scalar(0, 0, 255), float size = 1);

    /**
     * 画文本.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param      text      The text.
     * @param [in] org       起始点原点.
     * @param      color     (Optional) The color.
     * @param      fontScale (Optional) The font scale.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawText(const std::string& text, const cv::Point& org, const cv::Scalar& color = cv::Scalar(0, 0, 255), double fontScale = 0.4);

    /**
     * 在图上另起一行画一行文本，通常使用这个函数.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  text      The text.
     * @param  color     (Optional) The color.
     * @param  fontScale (Optional) The font scale.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawTextLine(const std::string& text, const cv::Scalar& color = cv::Scalar(0, 0, 255), double fontScale = 0.5);

    /**
     * 画一个图片.
     *
     * @author daixian
     * @date 2017/8/17
     *
     * @param [in] src 源图片.
     * @param      roi 目标的roi.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawMat(const cv::Mat& src, const cv::Rect& roi);

    /**
     * 画一个图片，输入两个roi.
     *
     * @author daixian
     * @date 2017/8/17
     *
     * @param [in] src     源图片(如果是单通道图像那么会自动转换成3通道).
     * @param      roi_src The roi.
     * @param      roi_dst The roi.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawMat(const cv::Mat& src, const cv::Rect& roi_src, const cv::Rect& roi_dst);

    /**
     * 画一个图片，输入一个目标区域的index就可以了.
     *
     * @author daixian
     * @date 2018/11/27
     *
     * @param [in] src   源图片(如果是单通道图像那么会自动转换成3通道).
     * @param      index 目标区域的index.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawMat(const cv::Mat& src, int index);

    /**
     * 对相机图像上画一个矩形.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  index ROI的index,通常应该等于camIndex.
     * @param  rect  要画的矩形,相对坐标就是小roi中的坐标.
     * @param  color (Optional) The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawRectangleROI(int index, const cv::Rect2f& rect, const cv::Scalar& color = cv::Scalar(0, 255, 0));

    /**
     * 对相机图像上画线.
     *
     * @author daixian
     * @date 2019/3/13
     *
     * @param  index  ROI的index,通常应该等于camIndex.
     * @param  point1 The first point.
     * @param  point2 The second point.
     * @param  color  The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawLineROI(int index, const cv::Point2f& point1, const cv::Point2f& point2, const cv::Scalar& color);

    /**
     * 对相机图像上画多边形.
     *
     * @author daixian
     * @date 2019/3/13
     *
     * @param  index   ROI的index,通常应该等于camIndex.
     * @param  polygon The polygon.
     * @param  color   The color.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawPolygonROI(int index, const std::vector<cv::Point>& polygon, const cv::Scalar& color);

    /**
     * 画文本.
     *
     * @author daixian
     * @date 2019/3/13
     *
     * @param  index     ROI的index,通常应该等于camIndex.
     * @param  text      文本.
     * @param  org       文本起始点.
     * @param  color     (Optional) The color.
     * @param  fontScale (Optional) The font scale.
     *
     * @returns 当前图片.
     */
    cv::Mat& drawTextROI(int index, const std::string& text, const cv::Point& org, const cv::Scalar& color = cv::Scalar(0, 0, 255), double fontScale = 0.3);

    /**
     * 显示窗口.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  winName 窗口名字.
     * @param  x       (Optional) 窗口位置x.
     * @param  y       (Optional) 窗口位置y.
     */
    void showWin(const std::string& winName, int x = 800, int y = 0);

    /**
     * 关闭窗口.
     *
     * @author daixian
     * @date 2018/11/26
     */
    void closeWin();

    /**
     * 保存当前帧在内存.
     *
     * @author daixian
     * @date 2017/10/2
     */
    void saveToMemory();

    /**
     * 保存内存图片到文件夹.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param  dirPath 要保存的文件夹路径.
     */
    void saveMemImgToFile(const std::string& dirPath);

    /**
     * 写当前图片像素Raw到内存的某个位置.
     *
     * @author daixian
     * @date 2018/11/26
     *
     * @param [in] buff 内存的目标位置.
     */
    void writeMemImgToMem(void* buff);

    /**
     * 清空成黑色背景.
     *
     * @author daixian
     * @date 2018/11/26
     */
    void clear();

    /**
     * 清空内存里保存的图片.
     *
     * @author daixian
     * @date 2018/12/4
     */
    void clearMemory();
};
} // namespace dxlib