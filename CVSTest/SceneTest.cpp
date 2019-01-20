﻿#include "pch.h"

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
    obj[L"arr"] = web::json::value::array({ 1, 2, 3 });

    utility::string_t str = obj.serialize();

    std::string md = FileHelper::getModuleDir();
    std::string filePath = md + "\\test0.json";

    //只有这样才能把一个wchar_t(utf16)写入utf8文件
    utility::ofstream_t out(filePath, std::ios::out | std::ios::binary);

    //这里只能new一个，不能完美释放
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    out.imbue(utf8_locale);
    obj.serialize(out);
    out.flush();
    out.close();

    web::json::value obj2;
    obj2.parse(str);

    EXPECT_TRUE(obj[L"key1"].is_boolean());
    EXPECT_TRUE(obj[L"key1"].as_bool() == false);
    EXPECT_TRUE(obj[L"key2"].is_integer());
    EXPECT_TRUE(obj[L"key2"].as_integer() == 44);
    EXPECT_TRUE(abs(obj[L"key3"].as_number().to_double() - 43.6) < 1e-4);
}

TEST(Scene, GameObj)
{
    Scene scene;

    scene.vGameObj.push_back(GameObj(_XPLATSTR("a"), 1, Eigen::Vector3d(1, 2, 3).data(), Eigen::Vector4d(0, 0, 0, 1).data()));
    scene.vGameObj.push_back(GameObj(_XPLATSTR("b"), 2, Eigen::Vector3d(2, 2, 3).data(), Eigen::Vector4d(0, 0, 0, 1).data()));

    scene.vGameObj.push_back(GameObj(_XPLATSTR("中文1"), 2, Eigen::Vector3d(2, 2, 3).data(), Eigen::Vector4d(0, 0, 0, 1).data()));

    scene.save(FileHelper::getModuleDir() + "\\test1.json");
}