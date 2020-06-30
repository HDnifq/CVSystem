#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "Poco/Path.h"

namespace dxlib {
namespace cvsystem {

/**
 * getImagePairs函数的返回结果.
 *
 * @author daixian
 * @date 2020/5/21
 */
struct ImagePairsResult
{
    /// <summary> 所有标定图片路径列表 L. </summary>
    std::vector<std::string> imgListL;

    /// <summary> 所有标定图片路径列表 R. </summary>
    std::vector<std::string> imgListR;

    /// <summary> 所有的图片按一个L一个R排列. </summary>
    std::vector<std::string> imagelist;
};

/**
 * 文件辅助类.
 *
 * @author daixian
 * @date 2019/4/10
 */
class FileHelper
{
  public:
    //得到模块目录末尾不带斜杠"D:\\Work\\F3DSys\\F3DSystem"
    static std::string getModuleDir();

    //得到AppData目录末尾不带斜杠"C:\\Users\\dx\\AppData\\Roaming" linux下是home的dir
    static std::string getHomeDir();

    static bool dirExists(const std::string& dirName_in);
    static bool dirExists(const std::wstring& dirName_in);
    //如果文件夹不存在就直接创建文件夹（确保创建文件夹）
    static void isExistsAndCreat(const std::string& sDir);
    static void isExistsAndCreat(const std::wstring& dirPath);
    static void isExistsAndCreat(const Poco::Path& path);

    /**
     * 判断一个目录是否是绝对路径,如果不是绝对路径就用base和它拼接.
     *
     * @author daixian
     * @date 2020/4/22
     *
     * @param          base base路径.
     * @param [in,out] path 要拼接的路径.
     */
    static void makeAbsolute(const Poco::Path& base, Poco::Path& path);

    /**
     * 读一个文件到vector数组.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param       path 文件路径.
     * @param [out] data 数据存储到一个vector里.
     */
    static void readFile(const std::string& path, std::vector<char>& data);

    /**
     * 读一个文件到vector数组.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param       path 文件路径.
     * @param [out] data 数据存储到一个vector里.
     */
    static void readFile(const std::string& path, std::vector<unsigned char>& data);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 分隔符. </summary>
    ///
    /// <remarks> Dx, 2020/4/2. </remarks>
    ///
    /// <returns> A std::string. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline std::string sep()
    {
#if defined(_WIN32) || defined(_WIN64)
        return "\\";
#else
        return "/";
#endif
    }

    /**
     * 从一个文件夹的第一层目录去搜索文件添加到result中.
     *
     * @author daixian
     * @date 2019/1/9
     *
     * @param [in,out] result  The result.
     * @param          dirPath Pathname of the directory.
     * @param          pattern (Optional) Specifies the pattern.
     */
    static void getFiles(std::vector<std::string>& result, const std::string& dirPath, const std::string& pattern = ".png");

    /**
     * 设定两个文件夹来选择文件夹里的所有图片.
     *
     * @author daixian
     * @date 2018/11/22
     *
     * @param  dirPathL  The dir path l.
     * @param  dirPathR  The dir path r.
     * @param  extension (Optional) The extension.
     *
     * @returns The image pairs.
     */
    static ImagePairsResult getImagePairs(const std::string& dirPathL, const std::string& dirPathR, const std::string& extension = ".png");

    //按正则表达式搜索完整文件名
    static void search(std::vector<std::string>& result, const std::string& dirPath, const std::string& regexStr = "[.]png$");
    static void search(std::vector<std::string>& result_path, std::vector<std::string>& result_filename, const std::string& dirPath, const std::string& regexStr = "[.]png$");
};

} // namespace cvsystem
} // namespace dxlib