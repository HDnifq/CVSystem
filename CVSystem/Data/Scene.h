#pragma once
//#include <Eigen/Geometry> //原本是使用它的，但是它和一些库有冲突
// eigen库里有一个代码会和U()宏冲突，微软注释说明可以使用_TURN_OFF_PLATFORM_STRING宏来去掉U(),
//然后使用_XPLATSTR来代替U()
//#ifndef _TURN_OFF_PLATFORM_STRING
//#define _TURN_OFF_PLATFORM_STRING
//#endif // !_TURN_OFF_PLATFORM_STRING
#include <vector>
#include <string>
#include <array>
#include <map>

//因为Eigen库里的v3的计算函数多一些，所以就使用它了
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
    Board = 200,
};

///-------------------------------------------------------------------------------------------------
/// <summary> 一个要显示的物体. </summary>
///
/// <remarks> Dx, 2018/12/29. </remarks>
///-------------------------------------------------------------------------------------------------
class GameObj
{
  public:
    GameObj(std::wstring name = L"unnamed",
            int type = ObjType::Empty,
            std::array<double, 3> position = {0, 0, 0},
            std::array<double, 4> rotation = {0, 0, 0, 1}) : name(name),
                                                             type(type),
                                                             position(position),
                                                             rotation(rotation) {}

    GameObj(std::wstring name, int type, double* position, double* rotation) : name(name),
                                                                               type(type)
    {
        this->position = {position[0], position[1], position[2]};
        this->rotation = {rotation[0], rotation[1], rotation[2], rotation[3]};
    }

    ~GameObj() {}

    /// <summary> 物体名字. </summary>
    std::wstring name;

    /// <summary> 物体类型. </summary>
    int type = ObjType::Empty;

    /// <summary> 世界坐标. </summary>
    std::array<double, 3> position = {0, 0, 0};

    /// <summary> 旋转. </summary>
    std::array<double, 4> rotation = {0, 0, 0, 1};

    /// <summary> 本地缩放(如果为0认为忽略缩放设置，使用u3d资源里的缩放). </summary>
    std::array<double, 3> localScale = {0, 0, 0};

    /// <summary> 上面提供的坐标系是否是本地坐标. </summary>
    bool isLocal = false;

    /// <summary> 这个物体的子物体. </summary>
    std::vector<GameObj> children;

#pragma region obj <->json
    void toJson(void* jsonValue);
    static GameObj toObj(void* jsonValue);
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

    Line(std::wstring name, int type = 0) : name(name),
                                            type(type) {}

    Line(std::wstring name, int type, std::array<double, 3> position0, std::array<double, 3> position1) : name(name),
                                                                                                          type(type),
                                                                                                          pos0(position0),
                                                                                                          pos1(position1) {}

    Line(std::wstring name, int type, double* position0, double* position1) : name(name),
                                                                              type(type)
    {
        this->pos0 = {position0[0], position0[1], position0[2]};
        this->pos1 = {position1[0], position1[1], position1[2]};
    }

    ~Line() {}

    /// <summary> 物体名字. </summary>
    std::wstring name;

    /// <summary> 物体类型. </summary>
    int type = 0;

    /// <summary> 世界坐标. </summary>
    std::array<double, 3> pos0 = {0, 0, 0};

    /// <summary> 世界坐标. </summary>
    std::array<double, 3> pos1 = {0, 0, 0};

#pragma region obj <->json
    void toJson(void* jsonValue);
    static Line toObj(void* jsonValue);
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

    /// <summary> 根目录下所有物体的map,key是这个物体的名字,对外保存json的时候是数组. </summary>
    std::map<std::wstring, GameObj> vGameObj;

    /// <summary> 所有绘制线的列表,对外保存json的时候是数组. </summary>
    std::map<std::wstring, Line> vLine;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 保存到文件. </summary>
    ///
    /// <remarks> Dx, 2018/12/29. </remarks>
    ///
    /// <param name="filePath"> 文件路径. </param>
    ///-------------------------------------------------------------------------------------------------
    void save(std::string filePath);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从文件载入. </summary>
    ///
    /// <remarks> Surface, 2019/2/7. </remarks>
    ///
    /// <param name="filePath"> The file path to load. </param>
    ///-------------------------------------------------------------------------------------------------
    void load(std::string filePath);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 添加一个GameObj物体,这里增加到map里面,
    /// 也就是所有这个map里面的物体都是根目录里面的,认为根目录下的物体不能重名,如果重名则会覆盖.
    /// </summary>
    ///
    /// <remarks> Surface, 2019/2/7. </remarks>
    ///
    /// <param name="go"> The go. </param>
    ///-------------------------------------------------------------------------------------------------
    void addGameObj(const GameObj& go);

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 添加一个Line物体,这里增加到map里面,
    /// 也就是所有这个map里面的物体都是根目录里面的,认为根目录下的物体不能重名,如果重名则会覆盖.
    /// </summary>
    ///
    /// <remarks> Surface, 2019/2/7. </remarks>
    ///
    /// <param name="line"> The go. </param>
    ///-------------------------------------------------------------------------------------------------
    void addLine(const Line& line);

#pragma region obj <->json
    void toJson(void* jsonValue);
    static Scene toObj(void* jsonValue);
    static void toObj(Scene& obj, void* jsonValue);
#pragma endregion
  private:
};

} // namespace u3d
} // namespace dxlib