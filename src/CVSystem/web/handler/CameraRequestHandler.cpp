#define XUEXUE_JSON_SUPPORT_OPENCV
#define XUEXUE_JSON_SUPPORT_EIGEN
#include "xuexuejson/JsonMapper.hpp"

#include "CameraRequestHandler.h"
#include <iostream>
#include "dlog/dlog.h"

#include "Poco/URI.h"
#include <algorithm>
#include "../extension/ParserQueryParameters.hpp"
#include "../../System/MultiCamera.h"
#include "../../System/CameraManger.h"
#include "../../System/DevicesHelper.h"

using namespace xuexue::json;
using namespace Poco::Net;
using namespace Poco;

namespace dxlib {
namespace cvsystem {

class CameraRequestHandler::Impl
{
  public:
    Impl() {}
    ~Impl() {}

  private:
};

CameraRequestHandler::CameraRequestHandler(const Poco::URI& uri, const CameraRequestHandler::HandlerType& ev) : uri(uri), ev(ev)
{
    _impl = new CameraRequestHandler::Impl();
}

CameraRequestHandler::~CameraRequestHandler()
{
    delete _impl;
}

void CameraRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& response)
{
    try {
        switch (ev) {
        case HandlerType::none:
            break;
        case HandlerType::listCamera:
            handleRequestListCamera(req, response);
            break;
        case HandlerType::getCapProp:
            handleRequestGetCapProp(req, response);
            break;
        case HandlerType::setCapProp:
            handleRequestSetCapProp(req, response);
            break;
        default:
            break;
        }
    }
    catch (const Poco::Exception& pe) {
        LogE("CameraRequestHandler.handleRequest():异常:%s,%s ", pe.what(), pe.message().c_str());
    }
    catch (const std::exception& e) {
        LogE("CameraRequestHandler.handleRequest():异常:%s", e.what());
    }
}

void CameraRequestHandler::handleRequestListCamera(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("CameraRequestHandler.handleRequestListCamera():执行列出camera设备");
    ParserQueryParameters querys(uri);

    DevicesHelper::GetInst()->listDevices();
    std::vector<std::string> vCamName;
    for (auto& kvp : DevicesHelper::GetInst()->devList) {
        vCamName.push_back(kvp.second);
    }

    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream& out = response.send();
    out << JsonMapper::toJson(vCamName);
}

void CameraRequestHandler::handleRequestGetCapProp(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("CameraRequestHandler.handleRequestGetCapProp():执行查询相机状态");
    ParserQueryParameters querys(uri);

    // 检察querys.
    if (!querys.checkKey("name", &response)) {
        LogI("CameraRequestHandler.handleRequestGetCapProp():querys缺少必须参数!");
        return;
    }

    std::string camName = querys.keyValue["name"];
    auto camera = CameraManger::GetInst()->getDevice(camName);
    if (camera == nullptr) {

        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("text/plain");
        std::ostream& out = response.send();
        out << "正在工作的相机中不存在名为" << camName;
    }
    else {
        response.setStatus(HTTPResponse::HTTP_OK);
        response.setContentType("application/json");
        std::ostream& out = response.send();
        out << JsonMapper::toJson(camera->outputAllProp());
    }
}

void CameraRequestHandler::handleRequestSetCapProp(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("CameraRequestHandler.handleRequestSetCapProp():设置相机属性");
    ParserQueryParameters querys(uri);

    // 检察querys.
    if (!querys.checkKey({"name", "prop", "value"}, &response)) {
        LogI("CameraRequestHandler.handleRequestSetCapProp():querys缺少必须参数!");
        return;
    }

    std::string camName = querys.keyValue["name"];
    std::string prop = querys.keyValue["prop"];
    std::string value = querys.keyValue["value"];
    auto camera = CameraManger::GetInst()->getDevice(camName);
    if (camera == nullptr) {

        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentType("text/plain");
        std::ostream& out = response.send();
        out << "正在工作的相机中不存在名为" << camName;
    }
    else {
        cv::VideoCaptureProperties p = CameraDevice::propStr2Enum(prop);
        if (p == cv::CV__CAP_PROP_LATEST) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
            response.setContentType("text/plain");
            std::ostream& out = response.send();
            out << "相机属性名" << prop << "错误";
        }
        else {
            camera->setPropWithString(p, value); //设置相机的这一项属性

            response.setStatus(HTTPResponse::HTTP_OK);
            response.setContentType("text/plain");
            std::ostream& out = response.send();
            out << "完成";
        }
    }
}

} // namespace cvsystem
} // namespace dxlib