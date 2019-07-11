#include "pch.h"

#include "../CVSystem/CVSystem.h"
#include "../CVSystem/Algorithm/LightPoint.hpp"
#include "../CVSystem/Algorithm/Project3DTo2D.hpp"

using namespace dxlib;
//using namespace Eigen;
using namespace std;

//亮点测试
TEST(CVALGO, LightPoint)
{
    ImageSave::GetInst()->ReadFormFile("..\\..\\TestImage\\ALGO\\GlassPoint");

    auto ig = ImageSave::GetInst()->GetImage();

    LightPoint::Param param;
    for (auto& kvp : ig) {
        LightPoint::Result result;
        LightPoint::detection(kvp.second, param, result);
    }
}

Eigen::Vector2d PixToOne(Eigen::Vector2d pix)
{
    double x = (pix.x() - (1920.0 / 2)) / (1920.0 / 2);
    double y = (-pix.y() + (1080.0 / 2)) / (1080.0 / 2);
    return {x, y};
}

Eigen::Vector2d OneToPix(Eigen::Vector2d one)
{
    double x = one.x() * (1920.0 / 2) + (1920.0 / 2);
    double y = -(one.y() * (1080.0 / 2) - (1080.0 / 2));
    return {x, y};
}

TEST(CVALGO, Project3DTo2D)
{
    Project3DTo2D::Param param;
    param.p3d.push_back({1, 1, 0});
    param.p3d.push_back({1, 2, 0});
    //param.p3d.push_back({2, 2, 0});
    param.p3d.push_back({3, 1, 0});
    param.p3d.push_back({7, 1, 0});

    param.p2d.push_back(PixToOne({588.9, 464.8}));
    param.p2d.push_back(PixToOne({581.5, 614.6}));
    //param.p2d.push_back(PixToOne({667.0, 661.0}));
    param.p2d.push_back(PixToOne({791.5, 535.5}));
    param.p2d.push_back(PixToOne({1850.2, 905.2}));

    Project3DTo2D::Result result;

    Project3DTo2D::detection(param, result);

    Eigen::Vector2d testP0 = OneToPix(result.tranMat * Eigen::Vector4d{1, 1, 0, 1});
    Eigen::Vector2d testP1 = OneToPix(result.tranMat * Eigen::Vector4d{1, 2, 0, 1});
    Eigen::Vector2d testP2 = OneToPix(result.tranMat * Eigen::Vector4d{2, 2, 0, 1});
    Eigen::Vector2d testP3 = OneToPix(result.tranMat * Eigen::Vector4d{3, 1, 0, 1});

    Eigen::Vector2d testP4 = OneToPix(result.tranMat * Eigen::Vector4d{5, 2, 0, 1});
}