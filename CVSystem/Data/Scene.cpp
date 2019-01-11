#include "Scene.h"

#ifndef _NO_ASYNCRTIMP
#define _NO_ASYNCRTIMP
#endif // !_NO_ASYNCRTIMP
#include "cpprest/json.h"

#include <codecvt>
#include <locale>

namespace dxlib {
    namespace u3d {

        void GameObj::toJson(void* jsonObj)
        {
            web::json::value& jv = *((web::json::value*)jsonObj);
            jv[L"name"] = web::json::value::string(name);
            jv[L"type"] = web::json::value::number(type);
            jv[L"position"] = web::json::value::array({ position[0], position[1], position[2] });
            jv[L"rotation"] = web::json::value::array({ rotation[0], rotation[1], rotation[2], rotation[3] });

            if (children.size() > 0) {
                //对children中的每一项进行序列化
                std::vector<web::json::value> vObj;
                for (size_t i = 0; i < children.size(); i++) {
                    web::json::value obj;
                    children[i].toJson(&obj);
                    vObj.push_back(obj);
                }
                jv[L"children"] = web::json::value::array(vObj);
            } else {
                jv[L"children"] = web::json::value::array();//否则就是一个空数组
            }

        }

        void Line::toJson(void* jsonObj)
        {
            web::json::value& jv = *((web::json::value*)jsonObj);
            jv[L"name"] = web::json::value::string(name);
            jv[L"type"] = web::json::value::number(type);
            jv[L"pos0"] = web::json::value::array({ pos0[0], pos0[1], pos0[2] });
            jv[L"pos1"] = web::json::value::array({ pos1[0], pos1[1], pos1[2] });
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

        void Scene::toJson(void* jsonObj)
        {
            web::json::value& jv = *((web::json::value*)jsonObj);

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
    }
}