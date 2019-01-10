#pragma once

#include <opencv2/opencv.hpp>

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

            GameObj(std::wstring name, int type, cv::Vec3d position, cv::Vec4d rotation):
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
            cv::Vec3d position = cv::Vec3d(0, 0, 0);

            /// <summary> 旋转. </summary>
            cv::Vec4d rotation = cv::Vec4d(0, 0, 0, 1);

            /// <summary> 这个物体的子物体. </summary>
            std::vector<GameObj> children;

            #pragma region obj<->json
            void toJson(void* jsonObj);
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

            Line(std::wstring name, int type, cv::Vec3d position0, cv::Vec3d position1) :
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
            cv::Vec3d pos0 = cv::Vec3d(0, 0, 0);

            /// <summary> 世界坐标. </summary>
            cv::Vec3d pos1 = cv::Vec3d(0, 0, 0);

            #pragma region obj<->json
            void toJson(void* jsonObj);
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
            void save(std::string filePath);

            #pragma region obj<->json
            void toJson(void* jsonObj);
            #pragma endregion
        private:

        };

    }
}