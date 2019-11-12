#pragma once

#define _USE_MATH_DEFINES //定义了之后才有M_PI的宏定义
#include <math.h>
#include <opencv2/opencv.hpp>
#include <Eigen/Geometry>
#include <unsupported/Eigen/EulerAngles>
#include <opencv2/core/eigen.hpp>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 3D投影到2D. </summary>
///
/// <remarks> Dx, 2019/7/11. </remarks>
///-------------------------------------------------------------------------------------------------
class Project3DTo2D
{
  public:
    Project3DTo2D();
    ~Project3DTo2D();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 控制参数. </summary>
    ///
    /// <remarks> Dx, 2019/7/11. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Param
    {
      public:
        //std::vector<Eigen::Vector2d>  ModelVertices;

        //映射关系
        std::vector<Eigen::Vector3d> p3d;

        //范围在(-1,1之间)
        std::vector<Eigen::Vector2d> p2d;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测结果. </summary>
    ///
    /// <remarks> Dx, 2019/7/11. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Result
    {
      public:
        //模型空间变换到相机空间的第三行
        Eigen::Matrix<double, 1, 4> L2VMat;

        //整个4x4变换矩阵(等于L2V整合Project矩阵)的第一行和第二行
        Eigen::Matrix<double, 2, 4> tranMat;
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 检测. </summary>
    ///
    /// <remarks> Dx, 2019/7/10. </remarks>
    ///-------------------------------------------------------------------------------------------------
    static void detection(const Param& param, Result& result)
    {
        auto& p3d = param.p3d;
        auto& p2d = param.p2d;

        //思路是利用w=-z.  tranMat的结果(x,y,z,w)中的x就等于kw,(k范围是-1,1),这个方程组就是要把L2VMat的第3行和tranMat的第一行一起求解.
        Eigen::Matrix<double, 8, 8> L0_A;
        for (size_t i = 0; i < 8; i++) {
            L0_A.row(i) << p3d[i].x(), p3d[i].y(), p3d[i].z(), 1, p2d[i].x() * p3d[i].x(), p2d[i].x() * p3d[i].y(), p2d[i].x() * p3d[i].z(), p2d[i].x();
        }

        Eigen::Matrix<double, 8, 1> L0_B;
        //L0_B << p2d[0].x(), p2d[1].x(), p2d[2].x(), p2d[3].x();
        for (size_t i = 0; i < 8; i++) {
            L0_B.row(i) << 0;
        }

        Eigen::Matrix<double, 8, 1> L0 = L0_A.fullPivLu().solve(L0_B);

        //下面的内容是先前的,目前还没有实现完整的


        //方程组系数的第一行,只有平面上的y
        Eigen::Matrix<double, 4, 4> L1_A;
        L1_A << p3d[0].x(), p3d[0].y(), p3d[0].z(), 1,
            p3d[1].x(), p3d[1].y(), p3d[1].z(), 1,
            p3d[2].x(), p3d[2].y(), p3d[2].z(), 1,
            p3d[3].x(), p3d[3].y(), p3d[3].z(), 1;

        Eigen::Vector4d L1_B;
        L1_B << p2d[0].y(), p2d[1].y(), p2d[2].y(), p2d[3].y();
        Eigen::Vector4d L1 = L1_A.fullPivLu().solve(L1_B);

        result.tranMat << L0(0), L0(1), L0(2), L0(3),
            L1(0), L1(1), L1(2), L1(3);

        Eigen::Vector2d testP0 = result.tranMat * Eigen::Vector4d{param.p3d[0](0), param.p3d[0](1), param.p3d[0](2), 1};
        Eigen::Vector2d testP1 = result.tranMat * Eigen::Vector4d{param.p3d[1](0), param.p3d[1](1), param.p3d[1](2), 1};
        Eigen::Vector2d testP2 = result.tranMat * Eigen::Vector4d{param.p3d[2](0), param.p3d[2](1), param.p3d[2](2), 1};
        Eigen::Vector2d testP3 = result.tranMat * Eigen::Vector4d{param.p3d[3](0), param.p3d[3](1), param.p3d[3](2), 1};
    }

  private:
};

} // namespace dxlib
