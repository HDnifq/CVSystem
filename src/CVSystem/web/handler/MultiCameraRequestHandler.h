#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/URI.h"
#include "Poco/File.h"

using Poco::URI;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

namespace dxlib {
namespace cvsystem {

/**
 * 处理多相机的http消息.
 *
 * @author daixian
 * @date 2020/3/13
 */
class MultiCameraRequestHandler : public HTTPRequestHandler
{
  public:
    enum class HandlerType
    {
        // 没有支持的处理.
        none,

        // 打开相机.
        openCamera,

        // 关闭相机.
        closeCamera,

        // 启动分析线程
        start,

        // 关闭分析线程.
        stop,

        // 当前运行状态.
        status,
    };

    /**
     * 判断是否有支持的Path处理.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param  path 解码之后的path.
     *
     * @returns 解析出来的处理类型.
     */
    static HandlerType ParsePath(const std::string& path)
    {

        if (path == "/multicamera/open-camera" ||
            path == "/mc/open-camera") {
            return HandlerType::openCamera;
        }
        else if (path == "/multicamera/close-camera" ||
                 path == "/mc/close-camera") {
            return HandlerType::closeCamera;
        }
        else if (path == "/multicamera/start" ||
                 path == "/mc/start") {
            return HandlerType::start;
        }
        else if (path == "/multicamera/stop" ||
                 path == "/mc/stop") {
            return HandlerType::stop;
        }
        else if (path == "/multicamera/status" ||
                 path == "/mc/status") {
            return HandlerType::status;
        }
        return HandlerType::none;
    }

    /**
     * 工厂方法,如果能够包含支持的path就成功创建对象,否则返回null.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param  uri  完整uri.
     * @param  path 解码之后的path.
     *
     * @returns 返回null表示不支持的path.
     */
    static HTTPRequestHandler* creat(const URI& uri, const std::string& path)
    {
        //调用上面的解析
        auto ev = ParsePath(path);
        if (ev != HandlerType::none) {
            return new MultiCameraRequestHandler(uri, ev);
        }
        return nullptr;
    }

    /**
     * 构造.
     *
     * @author daixian
     * @date 2020/3/18
     *
     * @param  uri 请求的rui.
     * @param  ev  Handler处理类型.
     */
    MultiCameraRequestHandler(const Poco::URI& uri, const MultiCameraRequestHandler::HandlerType& ev);

    /**
     * 析构.
     *
     * @author daixian
     * @date 2020/3/18
     */
    virtual ~MultiCameraRequestHandler();

    /**
     * override处理.
     *
     * @author daixian
     * @date 2020/4/30
     *
     * @param [in,out] request  http请求.
     * @param [in,out] response http响应.
     */
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override;

    // 传递过来的当前实际Handler事件.
    HandlerType ev;

  private:
    // 当前的uri.
    Poco::URI uri;

    void handleRequestOpenCamera(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestCloseCamera(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStart(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStop(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStatus(HTTPServerRequest& request, HTTPServerResponse& response);

    // 隐藏成员.
    class Impl;
    Impl* _impl;
};

} // namespace cvsystem
} // namespace dxlib