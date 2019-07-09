#pragma once
#include <opencv2/opencv.hpp>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> An image save. </summary>
///
/// <remarks> Dx, 2019/7/8. </remarks>
///-------------------------------------------------------------------------------------------------
class ImageSave
{
  public:
    ImageSave();
    ~ImageSave();

    /// <summary> 单例. </summary>
    static ImageSave* GetInst()
    {
        if (m_pInstance == nullptr) { //判断是否第一次调用
            //这里注意当前相机的分辨率
            m_pInstance = new ImageSave();
        }
        return m_pInstance;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 创建新的一帧. </summary>
    ///
    /// <remarks> Dx, 2019/7/9. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void CreateNew()
    {
        std::map<std::string, cv::Mat> m;
        dqImage.push_back(m);
        while (dqImage.size() > dequelimit) {
            dqImage.pop_front();
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Images this object. </summary>
    ///
    /// <remarks> Dx, 2019/7/9. </remarks>
    ///
    /// <param name="name">  The name. </param>
    /// <param name="image"> The image. </param>
    ///-------------------------------------------------------------------------------------------------
    void AddImage(const std::string& name, const cv::Mat& image)
    {
        auto& m = dqImage.back();
        m[name] = image;
    }

    /// <summary> 一个图片队列,每一帧里的map的名字使用abcd等等. </summary>
    std::deque<std::map<std::string, cv::Mat>> dqImage;

    /// <summary> 内存保存图像上限. </summary>
    int dequelimit = 50;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 保存内存图片到文件夹. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="dirPath"> 要保存的文件夹路径. </param>
    ///-------------------------------------------------------------------------------------------------
    void SaveToFile(const std::string& dirPath);

  private:
    /// <summary> The instance. </summary>
    static ImageSave* m_pInstance;
};

} // namespace dxlib