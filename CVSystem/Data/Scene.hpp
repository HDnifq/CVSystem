#pragma once

#define _NO_ASYNCRTIMP
#include "cpprest/json.h"

#include <math.h>
#include <opencv2/opencv.hpp>
#include <Eigen/Geometry>
#include <unsupported/Eigen/EulerAngles>
#include <opencv2/core/eigen.hpp>
#include <codecvt>
#include <locale>

namespace dxlib {

    namespace u3d {

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 使用枚举定义物体的类型. </summary>
        ///
        /// <remarks> Dx, 2019/1/7. </remarks>
        ///-------------------------------------------------------------------------------------------------
        enum ObjType
        {
            Empty = -1,
            Cube = 0,
            Point = 100,
        };

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 一个要显示的物体. </summary>
        ///
        /// <remarks> Dx, 2018/12/29. </remarks>
        ///-------------------------------------------------------------------------------------------------
        class GameObj
        {
        public:
            GameObj() {}

            GameObj(std::wstring name, int type = ObjType::Cube):
                name(name),
                type(type) {}

            GameObj(std::wstring name, int type, Eigen::Vector3d position, Eigen::Quaterniond rotation):
                name(name),
                type(type),
                position(position),
                rotation(rotation) {}

            ~GameObj() {}

            /// <summary> 物体名字(不能使用汉字会引起json失败). </summary>
            std::wstring name;

            /// <summary> 物体类型. </summary>
            int type = ObjType::Empty;

            /// <summary> 世界坐标. </summary>
            Eigen::Vector3d position = Eigen::Vector3d::Zero();

            /// <summary> 旋转. </summary>
            Eigen::Quaterniond rotation = Eigen::Quaterniond(0, 0, 0, 1);

            /// <summary> 这个物体的子物体. </summary>
            std::vector<GameObj> children;

            #pragma region obj<->json

            void toJson(web::json::value& jv)
            {
                jv[L"name"] = web::json::value::string(name);
                jv[L"type"] = web::json::value::number(type);
                jv[L"position"] = web::json::value::array({position.x(), position.y(), position.z()});
                jv[L"rotation"] = web::json::value::array({rotation.x(), rotation.y(), rotation.z(), rotation.w()});

                if (children.size() > 0) {
                    //对children中的每一项进行序列化
                    std::vector<web::json::value> vObj;
                    for (size_t i = 0; i < children.size(); i++) {
                        web::json::value obj;
                        children[i].toJson(obj);
                        vObj.push_back(obj);
                    }
                    jv[L"children"] = web::json::value::array(vObj);
                } else {
                    jv[L"children"] = web::json::value::array();//否则就是一个空数组
                }
            }

            #pragma endregion
        };

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 一条线. </summary>
        ///
        /// <remarks> Dx, 2019/1/3. </remarks>
        ///-------------------------------------------------------------------------------------------------
        class Line
        {
        public:

            Line() {}

            Line(std::wstring name, int type = 0) :
                name(name),
                type(type) {}

            Line(std::wstring name, int type, Eigen::Vector3d position0, Eigen::Vector3d position1) :
                name(name),
                type(type),
                pos0(position0),
                pos1(position1) {}

            ~Line() { }

            /// <summary> 物体名字(不能使用汉字会引起json失败). </summary>
            std::wstring name;

            /// <summary> 物体类型. </summary>
            int type = 0;

            /// <summary> 世界坐标. </summary>
            Eigen::Vector3d pos0 = Eigen::Vector3d::Zero();

            /// <summary> 世界坐标. </summary>
            Eigen::Vector3d pos1 = Eigen::Vector3d::Zero();

            #pragma region obj<->json

            void toJson(web::json::value& jv)
            {
                jv[L"name"] = web::json::value::string(name);
                jv[L"type"] = web::json::value::number(type);
                jv[L"pos0"] = web::json::value::array({pos0.x(), pos0.y(), pos0.z()});
                jv[L"pos1"] = web::json::value::array({pos1.x(), pos1.y(), pos1.z()});
            }
            #pragma endregion
        };

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 一个场景. </summary>
        ///
        /// <remarks> Dx, 2018/12/29. </remarks>
        ///-------------------------------------------------------------------------------------------------
        class Scene
        {
        public:

            Scene()
            {

            }

            ~Scene()
            {

            }

            /// <summary> 所有物体的列表. </summary>
            std::vector<GameObj> vGameObj;


            /// <summary> 所有绘制线的列表. </summary>
            std::vector<Line> vLine;

            ///-------------------------------------------------------------------------------------------------
            /// <summary> 保存到文件. </summary>
            ///
            /// <remarks> Dx, 2018/12/29. </remarks>
            ///
            /// <param name="filePath"> 文件路径. </param>
            ///-------------------------------------------------------------------------------------------------
            void save(std::string filePath)
            {
                web::json::value jv;
                toJson(jv);
                utility::ofstream_t out(filePath, std::ios::out | std::ios::binary);
                //这里只能new一个，不能完美释放（官方用法就是这样 http://www.cplusplus.com/reference/codecvt/codecvt_utf8/ ）
                const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
                out.imbue(utf8_locale);
                jv.serialize(out);
                out.flush();
                out.close();
            }

            #pragma region obj<->json

            void toJson(web::json::value& jv)
            {
                //对vGameObj中的每一项进行序列化
                std::vector<web::json::value> vObj;
                for (size_t i = 0; i < vGameObj.size(); i++) {
                    web::json::value obj;
                    vGameObj[i].toJson(obj);
                    vObj.push_back(obj);
                }
                jv[L"vGameObj"] = web::json::value::array(vObj);

                //对vLine中的每一项进行序列化
                std::vector<web::json::value> vJLine;
                for (size_t i = 0; i < vLine.size(); i++) {
                    web::json::value obj;
                    vLine[i].toJson(obj);
                    vJLine.push_back(obj);
                }
                jv[L"vLine"] = web::json::value::array(vJLine);

            }
            #pragma endregion
        private:

        };

    }
}
