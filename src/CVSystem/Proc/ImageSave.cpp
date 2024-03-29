﻿#include "ImageSave.h"
#include "Poco/Format.h"
#include "xuexue/csharp/csharp.h"
#include "dlog/dlog.h"
#include <regex>

namespace dxlib {

ImageSave::ImageSave()
{
}

ImageSave::~ImageSave()
{
}

ImageSave* ImageSave::m_pInstance = nullptr;

//保存图片的名字的计数
int nameCount = 0;

void ImageSave::SaveToFile(const std::string& dirPath)
{
    using namespace xuexue::csharp;

    if (File::Exists(dirPath)) {

        LogE("ImageSave.SaveToFile():保存图片路径存在,但不是文件夹! %s", dirPath.c_str());
        return;
    }
    if (!Directory::Exists(dirPath)) {
        LogI("ImageSave.SaveToFile():保存图片路径不存在,尝试创建文件夹! %s", dirPath.c_str());
        try {
            Directory::createDirectory(dirPath);
        }
        catch (const std::exception& ex) {
            LogE("ImageSave.SaveToFile():异常:%s ,创建保存图片文件夹失败,函数返回!", ex.what());
            return;
        }
    }

    while (dqImage.size() > 0) {
        auto mapImg = dqImage.front();

        char fileName[64];
        for (auto& kvp : mapImg) {
            snprintf(fileName, sizeof(fileName), "%04d_%s.png", nameCount, kvp.first.c_str());
            cv::imwrite(Path::Combine(dirPath, fileName), kvp.second);
        }

        nameCount++;

        dqImage.pop_front();
    }
}

void ImageSave::ReadFormFile(const std::string& dirPath)
{
    //using namespace std;

    //map<string, map<string, fs::path>> mapfiles;

    //fs::directory_iterator di(dirPath);
    //for (auto& de : di) {
    //    if (fs::is_regular_file(de.status())) { //如果这个是文件
    //        string fp = de.path().string();     //文件路径
    //        string fname = de.path().filename().string();

    //        string extension = de.path().extension().string();
    //        if (extension == ".png") { //如果是png图片，那么就全部读取进去吧
    //            std::smatch sm;
    //            const std::regex pattern("^\\d+");
    //            //通常使用search,这里的含义就和c#里的差不多了,可以匹配一段
    //            if (std::regex_search(fname, sm, pattern)) {
    //                string num = sm[0];
    //                string name = fname.substr(num.length() + 1, fname.length() - num.length() - 1 - 4);

    //                if (mapfiles.find(num) == mapfiles.end()) //创建一个map
    //                {
    //                    map<string, fs::path> mp;
    //                    mapfiles[num] = mp;
    //                }
    //                mapfiles[num][name] = de.path();
    //            }
    //        }
    //    }
    //}
    //for (auto& kvpig : mapfiles) {
    //    string num = kvpig.first;
    //    auto& igPath = kvpig.second;
    //    std::map<std::string, std::string> ig;
    //    for (auto& kvp : igPath) {

    //        //ig[kvp.first] = cv::imread(kvp.second.string()); //读取图片文件
    //        ig[kvp.first] = kvp.second.string();
    //    }
    //    this->dqImageFile.push_back(ig);
    //}
}

} // namespace dxlib
