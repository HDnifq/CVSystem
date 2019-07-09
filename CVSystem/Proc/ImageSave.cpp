#include "ImageSave.h"
#include "boost/filesystem.hpp"
#include "dlog/dlog.h"

namespace dxlib {

ImageSave::ImageSave()
{
}

ImageSave::~ImageSave()
{
}

ImageSave* ImageSave::m_pInstance = NULL;

//保存图片的名字的计数
int nameCount = 0;

void ImageSave::SaveToFile(const std::string& dirPath)
{
    namespace fs = boost::filesystem;
    if (fs::exists(dirPath)) {
        if (!fs::is_directory(dirPath)) {
            LogE("ImageSave.SaveToFile():保存图片路径存在,但不是文件夹! %s", dirPath.c_str());
            return;
        }
    }
    else {
        LogI("ImageSave.SaveToFile():保存图片路径不存在,尝试创建文件夹! %s", dirPath.c_str());
        try {
            fs::create_directories(dirPath);
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
            fs::path filePath = fs::path(dirPath) / fileName;
            cv::imwrite(filePath.string(), kvp.second);
        }

        nameCount++;

        dqImage.pop_front();
    }
}

} // namespace dxlib