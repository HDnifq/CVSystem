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
    ImageSave::GetInst()->ReadFormFile("D:\\Work\\CVSystem\\TestImage\\ALGO\\GlassPoint");

    auto ig = ImageSave::GetInst()->GetBackImage();

    LightPoint::Param param;
    for (auto& kvp : ig) {
        LightPoint::Result result;
        LightPoint::detection(kvp.second, param, result);

        cv::Mat img = kvp.second;
        int w = img.cols;
        int h = img.rows;
        cv::Mat imgL = cv::Mat(img, cv::Rect(0, 0, w / 2, h));     //等于图的左半边
        cv::Mat imgR = cv::Mat(img, cv::Rect(w / 2, 0, w / 2, h)); //等于图的左半边
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
    param.p2d.push_back(PixToOne({588.9, 464.8}));

    param.p3d.push_back({1, 2, 0});
    param.p2d.push_back(PixToOne({581.5, 614.6}));

    param.p3d.push_back({2, 2, 0});
    param.p2d.push_back(PixToOne({667.0, 661.0}));

    param.p3d.push_back({3, 1, 0});
    param.p2d.push_back(PixToOne({791.5, 535.5}));

    param.p3d.push_back({4, 1, 0});
    param.p2d.push_back(PixToOne({938.1, 586.5}));

    param.p3d.push_back({5, 1, 0});
    param.p2d.push_back(PixToOne({1136.1, 656.5}));

    param.p3d.push_back({6, 0, 0});
    param.p2d.push_back(PixToOne({1497.7, 433.3}));

    param.p3d.push_back({7, 1, 0});
    param.p2d.push_back(PixToOne({1850.2, 905.2}));

    Project3DTo2D::Result result;

    Project3DTo2D::detection(param, result);

    Eigen::Vector2d testP0 = OneToPix(result.tranMat * Eigen::Vector4d{1, 1, 0, 1});
    Eigen::Vector2d testP1 = OneToPix(result.tranMat * Eigen::Vector4d{1, 2, 0, 1});
    Eigen::Vector2d testP2 = OneToPix(result.tranMat * Eigen::Vector4d{2, 2, 0, 1});
    Eigen::Vector2d testP3 = OneToPix(result.tranMat * Eigen::Vector4d{3, 1, 0, 1});

    Eigen::Vector2d testP4 = OneToPix(result.tranMat * Eigen::Vector4d{5, 2, 0, 1});
}

//实验矩阵的秩
TEST(Matrix, Rank)
{
    //将增广矩阵变为阶梯型后，我们就可以通过观察这个阶梯型矩阵判断方程组有无解。
    //具体的做法是看增广矩阵左侧的系数矩阵，如果他的秩和增广矩阵的秩是相等的，则该方程组有解，否则无解。

    //试一个3元方程组:
    // x+y+z=10;
    // x+y-z=10;
    // x+y-2z=10;

    Eigen::Matrix<double, 3, 3> a;
    a << 1, 1, 1,
        1, 1, -1,
        1, 1, -2;
    Eigen::Matrix<double, 3, 1> b;
    b << 10, 10, 10;

    //增广矩阵
    Eigen::Matrix<double, 3, 4> ab;
    ab << 1, 1, 1, 10,
        1, 1, -1, 10,
        1, 1, -2, 10;

    int ra = a.fullPivLu().rank();
    int rab = ab.fullPivLu().rank();
    //上面两个秩都等于2,所以是有解. 但是这个方程是有无穷解的

    Eigen::Matrix<double, 3, 1> s = a.fullPivLu().solve(b); //解出了其中一个符合的

    //-------------------------------------------------------------------------------
    //调整方程组,这个方程组应该是无解
    a << 1, 1, 1,
        1, 1, -1,
        -1, -1, -2;
    b << 10, 10, 10;
    ab << 1, 1, 1, 10,
        1, 1, -1, 10,
        -1, -1, -2, 10;

    ra = a.fullPivLu().rank();   //=2
    rab = ab.fullPivLu().rank(); //=3
    //调整了之后,现在秩不相等了,从上面两个方程可以推出z=0,x+y=10和-x-y=10,这个没办法求解
    s = a.fullPivLu().solve(b); //会解出一个无效的结果

    //-------------------------------------------------------------------------------
    //调整方程组,这个方程组应该是有解
    a << 1, 1, 1,
        1, 1, -1,
        1, -1, -2;
    b << 10, 10, 20;
    ab << 1, 1, 1, 10,
        1, 1, -1, 10,
        1, -1, -2, 20;
    ra = a.fullPivLu().rank();   //=3
    rab = ab.fullPivLu().rank(); //=3
    s = a.fullPivLu().solve(b);

    //-------------------------------------------------------------------------------
    //如果系数矩阵的秩r(A)小于增广矩阵的秩r(A,b)，r(A)<r(A,b)，那么方程组无解r(A)<r(A,b)，那么方程组无解；
    //如果系统矩阵的秩小于方程组未知数个数，r(A)=r(A,b)<n，那么方程组有多个解r(A)=r(A,b)<n，那么方程组有多个解。
    //如果系统矩阵的秩等于方程组未知数个数，r(A)=r(A,b)=n，那么方程组有唯一解r(A)=r(A,b)=n，那么方程组有唯一解。
    //
    //齐次线性方程组，就是方程组的等式右边全部是0的方程组，只有系数矩阵，不需要增广矩阵，所以不会出现{0=d}形式的不相容方程。所以不会出现无解的情况，只需要考虑是多个解，还是唯一解。
    //对于齐次线性方程组，有多个解叫做有非零解。唯一解叫做零解。多个解叫做有非零解。唯一解叫做零解。
    //对于Ax=0的齐次线性方程组，列出其系数矩阵（不需要增广矩阵），使用高斯消元法化简，化为阶梯形矩阵，化简后，判断有效方程组个数是否小于未知数个数，
    //如果有效方程组个数小于未知数个数，叫做有非零解（多个解）如果有效方程组个数小于未知数个数，叫做有非零解（多个解）
    //如果等于，叫做只有零解（唯一解）
}

TEST(Math, cross)
{
    Eigen::Vector3d d1(1, 0, 0);
    double c = d1.dot(Eigen::Vector3d{-1, 0, 0}); //-1
    double c2 = d1.dot(Eigen::Vector3d{1, 0, 0}); //1
}