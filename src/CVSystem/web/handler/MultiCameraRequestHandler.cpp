#include "MultiCameraRequestHandler.h"

#include "../dto/MCStatusDto.hpp"

#include <iostream>
#include "dlog/dlog.h"

#include "Poco/URI.h"
#include <algorithm>

#include "../../System/MultiCamera.h"

#include "../extension/ParserQueryParameters.hpp"

using namespace xuexue::json;
using namespace Poco::Net;
using namespace Poco;

namespace dxlib {

class MultiCameraRequestHandler::Impl
{
  public:
    Impl() {}
    ~Impl() {}

  private:
};

MultiCameraRequestHandler::MultiCameraRequestHandler(const Poco::URI& uri,
                                                     const MultiCameraRequestHandler::HandlerType& ev) : uri(uri), ev(ev)
{
    _impl = new MultiCameraRequestHandler::Impl();
}

MultiCameraRequestHandler::~MultiCameraRequestHandler()
{
    delete _impl;
}

void MultiCameraRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& response)
{
    try {
        switch (ev) {
        case HandlerType::none:
            break;
        case HandlerType::openCamera:
            handleRequestOpenCamera(req, response);
            break;
        case HandlerType::closeCamera:
            handleRequestCloseCamera(req, response);
            break;
        case HandlerType::start:
            handleRequestStart(req, response);
            break;
        case HandlerType::stop:
            handleRequestStop(req, response);
            break;
        case HandlerType::status:
            handleRequestStatus(req, response);
            break;
        default:
            break;
        }
    }
    catch (const Poco::Exception& pe) {
        LogE("MultiCameraRequestHandler.handleRequest():异常:%s,%s ", pe.what(), pe.message().c_str());
    }
    catch (const std::exception& e) {
        LogE("MultiCameraRequestHandler.handleRequest():异常:%s", e.what());
    }
}

void MultiCameraRequestHandler::handleRequestOpenCamera(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("MultiCameraRequestHandler.handleRequestOpenCamera():执行打开相机");
    MultiCamera::GetInst()->openCamera();
    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("text/plain");
    std::ostream& out = response.send();
    out << "完成";
}

void MultiCameraRequestHandler::handleRequestCloseCamera(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("MultiCameraRequestHandler.handleRequestCloseCamera():执行关闭相机");
    MultiCamera::GetInst()->closeCamera();
    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("text/plain");
    std::ostream& out = response.send();
    out << "完成";
}

void MultiCameraRequestHandler::handleRequestStart(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("MultiCameraRequestHandler.handleRequestStart():执行启动分析线程");
    MultiCamera::GetInst()->start();
    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("text/plain");
    std::ostream& out = response.send();
    out << "完成";
}

void MultiCameraRequestHandler::handleRequestStop(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("MultiCameraRequestHandler.handleRequestStop():执行停止分析线程");
    MultiCamera::GetInst()->stop();
    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("text/plain");
    std::ostream& out = response.send();
    out << "完成";
}

void MultiCameraRequestHandler::handleRequestStatus(HTTPServerRequest& request, HTTPServerResponse& response)
{
    LogI("MultiCameraRequestHandler.handleRequestStatus():执行查询当前运行状态");

    MCStatusDto dto(MultiCamera::GetInst(), CameraManger::GetInst());

    response.setStatus(HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream& out = response.send();
    out << JsonMapper::toJson(dto, true);
}

} // namespace dxlib