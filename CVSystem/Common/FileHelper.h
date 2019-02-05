#pragma once

#include <string>
#include <vector>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary>
/// getImagePairs函数的返回结果.
/// </summary>
///
/// <remarks> Dx, 2019/1/9. </remarks>
///-------------------------------------------------------------------------------------------------
struct ImagePairsResult
{
    /// <summary> 所有标定图片路径列表 L. </summary>
    std::vector<std::string> imgListL;

    /// <summary> 所有标定图片路径列表 R. </summary>
    std::vector<std::string> imgListR;

    /// <summary> 所有的图片按一个L一个R排列. </summary>
    std::vector<std::string> imagelist;
};

class FileHelper
{
  public:
    static bool dirExists(const std::string& dirName_in);
    static bool dirExists(const std::wstring& dirName_in);

    //得到模块目录末尾不带斜杠"D:\\Work\\F3DSys\\F3DSystem"
    static std::string getModuleDir();

    //如果文件夹不存在就直接创建文件夹（确保创建文件夹）
    static void isExistsAndCreat(std::string sDir);
    static void isExistsAndCreat(std::wstring dirPath);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从一个文件夹的第一层目录去搜索文件添加到result中 </summary>
    ///
    /// <remarks> Dx, 2019/1/9. </remarks>
    ///
    /// <param name="result">  [in,out] The result. </param>
    /// <param name="dirPath"> Pathname of the directory. </param>
    /// <param name="pattern"> Specifies the pattern. </param>
    ///-------------------------------------------------------------------------------------------------
    static void getFiles(std::vector<std::string>& result, const std::string& dirPath, const std::string& pattern = ".png");

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 设定两个文件夹来选择文件夹里的所有图片. </summary>
    ///
    /// <remarks> Dx, 2018/11/22. </remarks>
    ///
    /// <param name="dirPathL">  The dir path l. </param>
    /// <param name="dirPathR">  The dir path r. </param>
    /// <param name="extension"> (Optional) The extension. </param>
    ///
    /// <returns> The image pairs. </returns>
    ///-------------------------------------------------------------------------------------------------
    static ImagePairsResult getImagePairs(const std::string& dirPathL, const std::string& dirPathR, const std::string& extension = ".png");
};
} // namespace dxlib