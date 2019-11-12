#pragma once
#include <opencv2/opencv.hpp>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 用来在内存中保存一组图片,主要就是包含一个图片队列. </summary>
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
        return m_pInstance;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 创建新的一帧. </summary>
    ///
    /// <remarks> Dx, 2019/7/9. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void CreateNew()
    {
        if (!IsEnable) {
            return;
        }
        std::map<std::string, cv::Mat> m;
        dqImage.push_back(m);
        while (dqImage.size() > dequelimit) {
            dqImage.pop_front();
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加一个图片记录进来. </summary>
    ///
    /// <remarks> Dx, 2019/7/9. </remarks>
    ///
    /// <param name="name">  The name. </param>
    /// <param name="image"> The image. </param>
    ///-------------------------------------------------------------------------------------------------
    void AddImage(const std::string& name, const cv::Mat& image)
    {
        if (!IsEnable) {
            return;
        }
        auto& m = dqImage.back();
        m[name] = image;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 清空所有的记录.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void Clear()
    {
        dqImage.clear();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到队列前面的一组图片. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///
    /// <returns> The image. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::map<std::string, cv::Mat> GetFrontImage()
    {
        std::map<std::string, cv::Mat> res;
        if (!dqImageFile.empty())
            //从记录的文件名中得到最前的一项
            for (auto& kvp : dqImageFile.front()) {
                res[kvp.first] = cv::imread(kvp.second); //读取图片
            }
        dqImageFile.pop_front();
        return res;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到队列最后面的一组图片. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///
    /// <returns> The image. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::map<std::string, cv::Mat> GetBackImage()
    {
        std::map<std::string, cv::Mat> res;
        //从记录的文件名中得到最前的一项
        if (!dqImageFile.empty())
            for (auto& kvp : dqImageFile.back()) {
                res[kvp.first] = cv::imread(kvp.second); //读取图片
            }
        dqImageFile.pop_back();
        return res;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 是否整个类使能. </summary>
    ///
    /// <value> The true. </value>
    ///-------------------------------------------------------------------------------------------------
    bool IsEnable = true;

    /// <summary> 一个图片队列,每一帧里的map的名字使用abcd等等. </summary>
    std::deque<std::map<std::string, cv::Mat>> dqImage;

    /// <summary> 内存保存图像上限. </summary>
    int dequelimit = 100;

    /// <summary> 一个图片文件队列,对应上面的图片队列,只是需要的时候再去读取这个文件,把这个cv::Mat变成路径. </summary>
    std::deque<std::map<std::string, std::string>> dqImageFile;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 保存内存图片到文件夹. </summary>
    ///
    /// <remarks> Dx, 2018/11/26. </remarks>
    ///
    /// <param name="dirPath"> 要保存的文件夹路径. </param>
    ///-------------------------------------------------------------------------------------------------
    void SaveToFile(const std::string& dirPath);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从一个文件夹载入图片. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///
    /// <param name="dirPath"> Pathname of the directory. </param>
    ///-------------------------------------------------------------------------------------------------
    void ReadFormFile(const std::string& dirPath);

  private:
    /// <summary> The instance. </summary>
    static ImageSave* m_pInstance;
};

} // namespace dxlib