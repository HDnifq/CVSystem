#include "FileHelper.h"

#include <stdlib.h>
#include <io.h>
#include <windows.h>
#include <ShlObj.h> //SHGetSpecialFolderPath
#include "Common.h"

#include <boost/filesystem.hpp>

#include "../dlog/dlog.h"

////这个脚本可能会报错 语言->符合模式 "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

namespace fs = boost::filesystem;
using namespace std;

namespace dxlib {

//配置输出日志的目录：
#define LOGDIR "log"
#define MKDIR "mkdir -p " LOGDIR
#define MKDIR_WIN "mkdir " LOGDIR //有查资料windows的cmd不支持-p命令

#define CSIDL_APPDATA 0x001a // <user name>\Application Data

bool FileHelper::dirExists(const std::string& dirName_in)
{
    int ftyp = _access(dirName_in.c_str(), 0);

    if (0 == ftyp)
        return true; // this is a directory!
    else
        return false; // this is not a directory!
}

bool FileHelper::dirExists(const std::wstring& dirName_in)
{
    int ftyp = _access(ws2s(dirName_in).c_str(), 0);

    if (0 == ftyp)
        return true; // this is a directory!
    else
        return false; // this is not a directory!
}

//只需要使用api获得一次就行了
std::string moduleDir;

//得到模块目录末尾不带斜杠"D:\\Work\\F3DSys\\F3DSystem"
std::string FileHelper::getModuleDir()
{
    if (!moduleDir.empty()) {
        return moduleDir; //只要有记录了就直接使用
    }
    else {
        wchar_t exeFullPath[MAX_PATH]; // Full path
        std::string strPath = "";

        GetModuleFileName(NULL, exeFullPath, MAX_PATH);

        char CharString[MAX_PATH];
        size_t convertedChars = 0;
        wcstombs_s(&convertedChars, CharString, MAX_PATH, exeFullPath, _TRUNCATE);
        strPath = (std::string) CharString; // Get full path of the file
        size_t pos = strPath.find_last_of('\\', strPath.length());
        moduleDir = strPath.substr(0, pos); // Return the directory without the file name
        return moduleDir;
    }
}

void FileHelper::isExistsAndCreat(std::wstring dirPath)
{
    std::string sDir = ws2s(dirPath);
    if (!dirExists(sDir)) { //如果文件夹路径不存在
        std::string cmd = std::string("mkdir \"") + sDir + std::string("\"");
        system(cmd.c_str()); //创建文件夹
    }
}

void FileHelper::isExistsAndCreat(std::string sDir)
{
    if (!dirExists(sDir)) { //如果文件夹路径不存在
        std::string cmd = std::string("mkdir \"") + sDir + std::string("\"");
        system(cmd.c_str()); //创建文件夹
    }
}

void FileHelper::getFiles(std::vector<std::string>& result, const std::string& dirPath, const std::string& pattern)
{
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        LogE("FileHelper.getFiles():文件夹不存在！ %s", dirPath.c_str());
        return;
    }
    fs::directory_iterator di(dirPath);
    for (auto& de : di) {
        if (fs::is_regular_file(de.status())) { //如果这个是文件
            string fp = de.path().string();     //文件路径
            string fname = de.path().filename().string();
            string extension = de.path().extension().string();
            if (!pattern.empty()) {
                if (fname.find(pattern) != string::npos || //如果文件路径包含这个内容
                    extension == pattern) {
                    result.push_back(fp);
                }
            }
        }
    }
}

ImagePairsResult FileHelper::getImagePairs(const std::string& dirPathL, const std::string& dirPathR, const std::string& extension)
{
    ImagePairsResult result;
    if (!fs::is_directory(dirPathL) || !fs::is_directory(dirPathR)) {
        LogE("FileHelper.getImagePairs():文件夹路径不存在!");
        return result;
    }

    fs::directory_iterator diL(dirPathL);
    for (auto& de : diL) {
        if (fs::is_regular_file(de.status())) { //如果这个是文件
            string fp = de.path().string();     //文件路径
            string fname = de.path().filename().string();
            string ext = de.path().extension().string();
            if (ext == extension) { //如果文件路径包含这个内容
                result.imgListL.push_back(fp);
                LogI("FileHelper.getImagePairs():找到一个L图片 %s", fp.c_str());
            }
        }
    }
    fs::directory_iterator diR(dirPathR);
    for (auto& de : diR) {
        if (fs::is_regular_file(de.status())) { //如果这个是文件
            string fp = de.path().string();     //文件路径
            string fname = de.path().filename().string();
            string ext = de.path().extension().string();
            if (ext == extension) { //如果文件路径包含这个内容
                result.imgListR.push_back(fp);
                LogI("FileHelper.getImagePairs():找到一个R图片 %s", fp.c_str());
            }
        }
    }
    if (result.imgListL.size() != result.imgListR.size()) {
        LogE("FileHelper.getImagePairs():找到的L和R的图片数量不一致！");

        result.imgListL.clear();
        result.imgListR.clear();
        result.imagelist.clear();
        return result;
    }
    for (size_t i = 0; i < result.imgListL.size(); i++) {
        result.imagelist.push_back(result.imgListL[i]);
        result.imagelist.push_back(result.imgListR[i]);
    }

    return result;
}
} // namespace dxlib