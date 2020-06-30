#include "FileHelper.h"

#include <stdlib.h>
#include "iostream"
#if defined(_WIN32) || defined(_WIN64)
#    include <windows.h>
#    include <ShlObj.h> //SHGetSpecialFolderPath
#elif defined(__linux__)
#    include <unistd.h>
#    include <sys/types.h>
#    include <pwd.h>
#endif

#include <boost/filesystem.hpp>

#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Format.h>
#include "Poco/Glob.h"
#include "Poco/FileStream.h"
#include "Poco/BinaryReader.h"

#include <regex>

#include "dlog/dlog.h"
#include "StringHelper.h"

////这个脚本可能会报错 语言->符合模式 "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-
namespace fs = boost::filesystem;
using namespace std;
using namespace Poco;

namespace dxlib {
namespace cvsystem {
//只需要使用api获得一次就行了
std::string moduleDir;
std::string homeDir;

#if defined(_WIN32) || defined(_WIN64)

//得到模块目录末尾不带斜杠"D:\\Work\\F3DSys\\F3DSystem"
std::string FileHelper::getModuleDir()
{
    if (!moduleDir.empty()) {
        return moduleDir; //只要有记录了就直接使用
    }
    else {
        char exeFullPath[MAX_PATH]; // Full path
        std::string strPath = "";

        GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);

        strPath = std::string(exeFullPath); // Get full path of the file
        size_t pos = strPath.find_last_of('\\', strPath.length());
        moduleDir = strPath.substr(0, pos); // Return the directory without the file name
        return moduleDir;
    }
}

std::string FileHelper::getHomeDir()
{
    if (!homeDir.empty()) {
        return homeDir; //只要有记录了就直接使用
    }
    else {
        char szPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
            homeDir = std::string(szPath);
            return homeDir;
        }
        LogE("FileHelper.getHomeDir():未能找到home文件夹!!");
        return "";
    }
}

#elif defined(__linux__)

std::string FileHelper::getModuleDir()
{
    if (!moduleDir.empty()) {
        return moduleDir; //只要有记录了就直接使用
    }
    else {
        char arg1[32];
        char exepath[512 + 1] = {0};
        snprintf(arg1, sizeof(arg1), "/proc/%d/exe", getpid());
        readlink(arg1, exepath, sizeof(exepath));
        std::string exeStr = std::string(exepath);
        size_t pos = exeStr.find_last_of('/', exeStr.length());
        moduleDir = exeStr.substr(0, pos); // Return the directory without the file name
        return moduleDir;
    }
}

std::string FileHelper::getHomeDir()
{
    if (!homeDir.empty()) {
        return homeDir; //只要有记录了就直接使用
    }
    else {
        struct passwd* pw = getpwuid(getuid());
        homeDir = std::string(pw->pw_dir);
        return homeDir;
    }
}
#endif

bool FileHelper::dirExists(const std::string& dirName_in)
{
    Poco::File dir(dirName_in);
    //如果存在
    if (dir.exists()) {
        if (dir.isDirectory()) {
            return true;
        }
        //它还是存在一种是一个文件的可能
        LogE("FileHelper.dirExists():文件夹被文件占用！%s", dirName_in.c_str());
    }
    return false;
}

bool FileHelper::dirExists(const std::wstring& dirName_in)
{
    //如果存在
    Poco::File dir(S::ws2s(dirName_in));
    if (dir.exists()) {
        if (dir.isDirectory()) {
            return true;
        }
        //它还是存在一种是一个文件的可能
        LogE("FileHelper.dirExists():文件夹被文件占用！%s", S::ws2s(dirName_in).c_str());
    }
    return false;
}

void FileHelper::isExistsAndCreat(const std::wstring& dirPath)
{
    std::string sDir = S::ws2s(dirPath);
    isExistsAndCreat(sDir);
}

void FileHelper::isExistsAndCreat(const std::string& sDir)
{
    if (!dirExists(sDir)) { //如果文件夹路径不存在
        try {
            Poco::File dir(sDir);
            dir.createDirectories();
        }
        catch (const std::exception& ex) {
            std::cerr << "FileHelper.isExistsAndCreat():" << ex.what() << '\n';
            throw ex;
        }
    }
}

void FileHelper::isExistsAndCreat(const Poco::Path& path)
{
    if (path.isFile()) {
        Poco::File dir(path.parent());
        dir.createDirectories();
    }
    else {
        Poco::File dir(path);
        dir.createDirectories();
    }
}

void FileHelper::readFile(const std::string& path, std::vector<char>& data)
{
    Poco::File file(path);
    data.resize(file.getSize()); //得到文件大小
    std::ifstream istr(path, std::ios::binary);
    istr.read(data.data(), data.size());
    istr.close();
}

void FileHelper::readFile(const std::string& path, std::vector<unsigned char>& data)
{
    Poco::File file(path);
    data.resize(file.getSize()); //得到文件大小
    std::ifstream istr(path, std::ios::binary);
    istr.read((char*)data.data(), data.size());
    istr.close();
}

void FileHelper::makeAbsolute(const Poco::Path& base, Poco::Path& path)
{
#if WIN32
    //在windows下如果是相对地址,或者是没有盘符
    if (path.isRelative() || path.getDevice().empty()) {
        string strPath = path.toString();
        //如果是windows下,没有盘符,那么这个路径应该就是相对地址.
        //如果开头存在/,\\,那么移除掉这开头的
        if (strPath.front() == '/' ||
            strPath.front() == '\\') {
            strPath.erase(0, 1);
            path = Poco::Path(strPath);
        }
#else
    if (root.isRelative()) {
#endif
        //这个函数会忽略相对路径的
        path.makeAbsolute(base);
    }
}

#pragma region 搜索文件和图片

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

void FileHelper::search(std::vector<std::string>& result, const std::string& dirPath, const std::string& regexStr)
{
    using namespace Poco;
    File dir(dirPath);
    result.clear();

    vector<File> vFiles;
    dir.list(vFiles);
    for (size_t i = 0; i < vFiles.size(); i++) {
        File& file = vFiles[i];
        if (file.isFile() && file.exists()) {
            Path filePath(vFiles[i].path());
            string fileName = filePath.getFileName();
            if (std::regex_search(fileName, std::regex(regexStr))) {
                result.push_back(file.path());
            }
        }
    }
}
void FileHelper::search(std::vector<std::string>& result_path, std::vector<std::string>& result_filename, const std::string& dirPath, const std::string& regexStr)
{
    using namespace Poco;
    File dir(dirPath);
    result_path.clear();

    vector<File> vFiles;
    dir.list(vFiles);
    for (size_t i = 0; i < vFiles.size(); i++) {
        File& file = vFiles[i];
        if (file.isFile() && file.exists()) {
            Path filePath(vFiles[i].path());
            string fileName = filePath.getFileName();
            if (std::regex_search(fileName, std::regex(regexStr))) {
                result_path.push_back(file.path());
                result_filename.push_back(filePath.getFileName());
            }
        }
    }
}

#pragma endregion

} // namespace cvsystem
} // namespace dxlib