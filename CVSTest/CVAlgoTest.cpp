#include "pch.h"

#include "../CVSystem/CVSystem.h"
#include "../CVSystem/Algorithm/LightPoint.hpp"

using namespace dxlib;
//using namespace Eigen;
using namespace std;

//是否是凸多边形
TEST(ALGO, LightPoint)
{
    ImageSave::GetInst()->ReadFormFile("D:\\Work\\MRSystem\\x64\\Release\\images\\debug\\camera");

    auto ig = ImageSave::GetInst()->GetImage();
    for (auto& kvp : ig)
    {
        LightPoint::detection(kvp.second);
    }
}