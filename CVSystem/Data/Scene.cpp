﻿#include "Scene.h"

#ifndef _NO_ASYNCRTIMP
#define _NO_ASYNCRTIMP
#endif // !_NO_ASYNCRTIMP
#ifndef _TURN_OFF_PLATFORM_STRING
#define _TURN_OFF_PLATFORM_STRING
#endif // !_TURN_OFF_PLATFORM_STRING

#include "cpprest/json.h"

#include <codecvt>
#include <locale>

namespace dxlib {
namespace u3d {

void GameObj::toJson(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue);
    jv[L"name"] = web::json::value::string(name);
    jv[L"type"] = web::json::value::number(type);
    jv[L"position"] = web::json::value::array({position[0], position[1], position[2]});
    jv[L"rotation"] = web::json::value::array({rotation[0], rotation[1], rotation[2], rotation[3]});
    jv[L"localScale"] = web::json::value::array({localScale[0], localScale[1], localScale[2]});

    if (children.size() > 0) {
        //对children中的每一项进行序列化
        std::vector<web::json::value> vObj;
        for (size_t i = 0; i < children.size(); i++) {
            web::json::value obj;
            children[i].toJson(&obj);
            vObj.push_back(obj);
        }
        jv[L"children"] = web::json::value::array(vObj);
    }
    else {
        jv[L"children"] = web::json::value::array(); //否则就是一个空数组
    }
}

GameObj GameObj::toObj(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue); //这应该是已经parse过的
    GameObj obj;
    obj.name = jv[L"name"].as_string();
    obj.type = jv[L"type"].as_integer();
    web::json::array arr = jv[L"position"].as_array();
    obj.position = {arr[0].as_double(), arr[1].as_double(), arr[2].as_double()};
    arr = jv[L"rotation"].as_array();
    obj.rotation = {arr[0].as_double(), arr[1].as_double(), arr[2].as_double(), arr[3].as_double()};
    arr = jv[L"localScale"].as_array();
    obj.localScale = {arr[0].as_double(), arr[1].as_double(), arr[2].as_double()};
    arr = jv[L"children"].as_array();
    for (size_t i = 0; i < arr.size(); i++) {
        web::json::value child = arr[i];
        obj.children.push_back(GameObj::toObj(&child));
    }
    return obj;
}

void Line::toJson(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue);
    jv[L"name"] = web::json::value::string(name);
    jv[L"type"] = web::json::value::number(type);
    jv[L"pos0"] = web::json::value::array({pos0[0], pos0[1], pos0[2]});
    jv[L"pos1"] = web::json::value::array({pos1[0], pos1[1], pos1[2]});
}

Line Line::toObj(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue); //这应该是已经parse过的
    Line obj;
    obj.name = jv[L"name"].as_string();
    obj.type = jv[L"type"].as_integer();
    web::json::array arr = jv[L"pos0"].as_array();
    obj.pos0 = {arr[0].as_double(), arr[1].as_double(), arr[2].as_double()};
    arr = jv[L"pos1"].as_array();
    obj.pos1 = {arr[0].as_double(), arr[1].as_double(), arr[2].as_double()};
    return obj;
}

void Scene::save(std::string filePath)
{
    web::json::value jv;
    toJson(&jv);
    utility::ofstream_t out(filePath, std::ios::out | std::ios::binary);
    //这里只能new一个，不能完美释放（官方用法就是这样 http://www.cplusplus.com/reference/codecvt/codecvt_utf8/ ）
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    out.imbue(utf8_locale);
    jv.serialize(out);
    out.flush();
    out.close();
}

void Scene::toJson(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue);

    //对vGameObj中的每一项进行序列化
    std::vector<web::json::value> vObj;
    for (size_t i = 0; i < vGameObj.size(); i++) {
        web::json::value obj;
        vGameObj[i].toJson(&obj);
        vObj.push_back(obj);
    }
    jv[L"vGameObj"] = web::json::value::array(vObj);

    //对vLine中的每一项进行序列化
    std::vector<web::json::value> vJLine;
    for (size_t i = 0; i < vLine.size(); i++) {
        web::json::value obj;
        vLine[i].toJson(&obj);
        vJLine.push_back(obj);
    }
    jv[L"vLine"] = web::json::value::array(vJLine);
}

Scene Scene::toObj(void* jsonValue)
{
    web::json::value& jv = *((web::json::value*)jsonValue); //这应该是已经parse过的
    Scene obj;

    web::json::array arr = jv[L"vGameObj"].as_array();
    for (size_t i = 0; i < arr.size(); i++) {
        web::json::value jvGO = arr[i];
        obj.vGameObj.push_back(GameObj::toObj(&jvGO));
    }

    arr = jv[L"vLine"].as_array();
    for (size_t i = 0; i < arr.size(); i++) {
        web::json::value jvLine = arr[i];
        obj.vLine.push_back(Line::toObj(&jvLine));
    }

    return obj;
}
} // namespace u3d
} // namespace dxlib