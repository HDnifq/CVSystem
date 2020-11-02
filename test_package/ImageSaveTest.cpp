#include "pch.h"
#include "CVSystem/CVSystem.h"
#include <regex>
#include <string>
#include "Poco/File.h"

#include "CVSystem/CVSystem.h"

using namespace dxlib;
using namespace std;
using namespace Poco;

TEST(ImageSave, ReadFormFile)
{
    string str = "0000_F3D0001.png";

    std::smatch sm;
    const std::regex pattern("^\\d+");

    //通常使用search,这里的含义就和c#里的差不多了,可以匹配一段
    if (std::regex_search(str, sm, pattern)) {
        string num = sm[0];
        string name = str.substr(num.length() + 1, str.length() - num.length() - 1 - 4);
    }
    EXPECT_TRUE(sm.size() == 1);
}

TEST(ImageSave, ReadFormFile2)
{
    //File file("D:\\Work\\MRSystem\\x64\\Release\\images\\debug\\camera");

    //if (file.exists() && file.isDirectory()) {
    //    ImageSave::GetInst()->ReadFormFile(file.path());
    //    size_t size = ImageSave::GetInst()->dqImageFile.size();
    //    EXPECT_TRUE(size > 0);
    //    auto ig = ImageSave::GetInst()->GetFrontImage();
    //    EXPECT_TRUE(ig.size() > 0);
    //}
}

//getItem的方法,返回值改成了一个对象
TEST(CameraImage, getItem)
{
    CameraManger::GetInst()->clear();

    //加进去自动分配index
    CameraManger::GetInst()->add(pCamera(new Camera("cam1")));
    CameraManger::GetInst()->add(pCamera(new Camera("cam2")));
    CameraManger::GetInst()->add(pCamera(new Camera("cam3")));
    CameraManger::GetInst()->add(pCamera(new Camera("cam4")));

    CameraImage item(CameraManger::GetInst()->getCameraVec());

    auto it5 = item.getItem(5);
    auto it1 = item.getItem(1);
    auto it2 = item.getItem(2);

    EXPECT_TRUE(it5.camera == nullptr);
    EXPECT_TRUE(it1.camera->devName == "cam2");
    EXPECT_TRUE(it2.camera->devName == "cam3");
}