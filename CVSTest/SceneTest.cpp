#include "pch.h"

#include "../CVSystem/CVSystem.h"
#ifndef _NO_ASYNCRTIMP
#define _NO_ASYNCRTIMP
#endif // !_NO_ASYNCRTIMP
#ifndef _TURN_OFF_PLATFORM_STRING
#define _TURN_OFF_PLATFORM_STRING
#endif // !_TURN_OFF_PLATFORM_STRING
#include "cpprest/json.h"
#include <codecvt>
#include <locale>
#include <Eigen/Geometry>

using namespace dxlib;
using namespace dxlib::u3d;

TEST(Scene, json)
{
    web::json::value obj;

    obj[L"key1"] = web::json::value::boolean(false);
    obj[L"key2"] = web::json::value::number(44);
    obj[L"key3"] = web::json::value::number(43.6);
    obj[L"key4"] = web::json::value::string(_XPLATSTR("str"));
    obj[L"key5"] = web::json::value::string(_XPLATSTR("中文"));
    obj[L"arr"] = web::json::value::array({1, 2, 3});

    utility::string_t str = obj.serialize();

    std::string filePath = "test0.json";

    //只有这样才能把一个wchar_t(utf16)写入utf8文件
    utility::ofstream_t out(filePath, std::ios::out | std::ios::binary);

    //这里只能new一个，不能完美释放
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    out.imbue(utf8_locale);
    obj.serialize(out);
    out.flush();
    out.close();

    web::json::value obj2 = web::json::value::parse(str);
    utility::string_t str2 = obj2.serialize();

    EXPECT_TRUE(obj[L"key1"].is_boolean());
    EXPECT_TRUE(obj[L"key1"].as_bool() == false);
    EXPECT_TRUE(obj[L"key2"].is_integer());
    EXPECT_TRUE(obj[L"key2"].as_integer() == 44);
    EXPECT_TRUE(abs(obj[L"key3"].as_number().to_double() - 43.6) < 1e-4);
}

TEST(Scene, json2)
{
    web::json::value obj = web::json::value::parse(_XPLATSTR("{ \"a\" : 10 }"));
    utility::string_t str = obj.serialize();

    obj[_XPLATSTR("a")] = web::json::value(12);
    obj[_XPLATSTR("b")] = web::json::value(13);
    auto nullValue = obj[_XPLATSTR("c")];
}

TEST(Scene, GameObj)
{
    GameObj obj(_XPLATSTR("a"), 1, {1, 2, 3}, {0, 0, 0, 1});
    obj.localScale = {3, 4, 5};
    obj.isLocal = true;
    obj.isActive = false;
    for (size_t i = 0; i < 3; i++) {
        obj.children.push_back(obj);
    }
    Line l0(L"abc", 100, {1, 2, 3}, {4, 5, 6});
    obj.lines.push_back(l0);

    //序列化
    web::json::value json;
    obj.toJson(&json);
    utility::string_t str = json.serialize();

    web::json::value json2 = web::json::value::parse(str);
    utility::string_t str2 = json2.serialize();
    //反序列化
    GameObj obj2 = GameObj::toObj(&json2);

    EXPECT_TRUE(obj.name == obj2.name);
    EXPECT_TRUE(obj.type == obj2.type);
    EXPECT_TRUE(obj.position == obj2.position);
    EXPECT_TRUE(obj.rotation == obj2.rotation);
    EXPECT_TRUE(obj.localScale == obj2.localScale);
    EXPECT_TRUE(obj.isLocal == obj2.isLocal);
    EXPECT_TRUE(obj.isActive == obj2.isActive);
    EXPECT_TRUE(obj.children.size() == obj2.children.size());
    EXPECT_TRUE(obj.lines.size() == obj2.lines.size());
}

TEST(Scene, Line)
{
    Line obj(L"abc", 100, {1, 2, 3}, {4, 5, 6});

    //序列化
    web::json::value json;
    obj.toJson(&json);
    //反序列化
    Line obj2 = Line::toObj(&json);

    EXPECT_TRUE(obj.name == obj2.name);
    EXPECT_TRUE(obj.type == obj2.type);
    EXPECT_TRUE(obj.pos0 == obj2.pos0);
    EXPECT_TRUE(obj.pos1 == obj2.pos1);
}

TEST(Scene, Scene)
{
    Scene obj;

    obj.addGameObj(GameObj(_XPLATSTR("a"), 1, {1, 2, 3}, {0, 0, 0, 1}));
    obj.addGameObj(GameObj(_XPLATSTR("b"), 2, {2, 2, 3}, {0, 0, 0, 1}));

    //序列化
    web::json::value json;
    obj.toJson(&json);

    utility::string_t str = json.serialize();

    web::json::value json2 = web::json::value::parse(str);
    utility::string_t str2 = json2.serialize();
    //反序列化
    Scene obj2 = Scene::toObj(&json2);

    EXPECT_TRUE(obj.vGameObj.size() == obj2.vGameObj.size());
    EXPECT_TRUE(obj.vGameObj.size() == obj2.vGameObj.size());
}

TEST(Scene, save_load)
{
    Scene scene;

    scene.addGameObj(GameObj(_XPLATSTR("a"), 1, {1, 2, 3}, {0, 0, 0, 1}));
    scene.addGameObj(GameObj(_XPLATSTR("b"), 2, {2, 2, 3}, {0, 0, 0, 1}));
    scene.addGameObj(GameObj(_XPLATSTR("中文"), 2, {2, 2, 3}, {0, 0, 0, 1}));

    //添加一个重复项
    scene.addGameObj(GameObj(_XPLATSTR("b"), 100, {1, 1, 1}, {0, 0, 0, 1}));
    EXPECT_TRUE(scene.vGameObj.size() == 3); //应该为3项(有替换b项)

    EXPECT_TRUE(scene.vGameObj[L"b"].type == 100); //应该为3项

    scene.save("test1.json");

    Scene scene2;
    scene2.load("test1.json");

    EXPECT_TRUE(scene.vGameObj.size() == scene2.vGameObj.size());
}