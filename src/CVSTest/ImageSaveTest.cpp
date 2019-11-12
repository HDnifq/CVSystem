#include "pch.h"
#include "../CVSystem/CVSystem.h"
#include <regex>
#include <string>

using namespace dxlib;
using namespace std;

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
    ImageSave::GetInst()->ReadFormFile("D:\\Work\\MRSystem\\x64\\Release\\images\\debug\\camera");
    size_t size = ImageSave::GetInst()->dqImageFile.size();
    EXPECT_TRUE(size > 0);
    auto ig = ImageSave::GetInst()->GetFrontImage();
    EXPECT_TRUE(ig.size() > 0);
}