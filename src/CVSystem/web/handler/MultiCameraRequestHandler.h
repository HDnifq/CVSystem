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

///-------------------------------------------------------------------------------------------------
/// <summary> 处理相机的http消息. </summary>
///
/// <remarks> Dx, 2020/3/13. </remarks>
///-------------------------------------------------------------------------------------------------
class MultiCameraRequestHandler : public HTTPRequestHandler
{
  public:
    enum class HandlerType
    {
        /// <summary>
        /// 没有支持的处理.
        /// </summary>
        none,

        /// <summary>
        /// 打开相机
        /// </summary>
        openCamera,

        /// <summary>
        /// 关闭相机.
        /// </summary>
        closeCamera,

        /// <summary>
        /// 启动分析线程
        /// </summary>
        start,

        /// <summary>
        /// 关闭分析线程.
        /// </summary>
        stop,

        /// <summary>
        /// 当前运行状态.
        /// </summary>
        status,
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 判断是否有支持的Path处理. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///
    /// <param name="path"> 解码之后的path. </param>
    ///
    /// <returns> 解析出来的处理类型. </returns>
    ///-------------------------------------------------------------------------------------------------
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

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 工厂方法，如果能够包含支持的path就成功创建对象,否则返回null. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///
    /// <param name="uri">  完整uri. </param>
    /// <param name="path"> 解码之后的path. </param>
    ///
    /// <returns> 返回null表示不支持的path. </returns>
    ///-------------------------------------------------------------------------------------------------
    static HTTPRequestHandler* creat(const URI& uri, const std::string& path)
    {
        //调用上面的解析
        auto ev = ParsePath(path);
        if (ev != HandlerType::none) {
            return new MultiCameraRequestHandler(uri, ev);
        }
        return nullptr;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Constructor. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///
    /// <param name="uri"> URI of the resource. </param>
    /// <param name="ev">  The ev. </param>
    ///-------------------------------------------------------------------------------------------------
    MultiCameraRequestHandler(const Poco::URI& uri, const MultiCameraRequestHandler::HandlerType& ev);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Destructor. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    virtual ~MultiCameraRequestHandler();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> override处理. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="request">  [in,out] The request. </param>
    /// <param name="response"> [in,out] The response. </param>
    ///-------------------------------------------------------------------------------------------------
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override;

    /// <summary> 传递过来的当前实际事件. </summary>
    HandlerType ev;

  private:
    /// <summary> 当前的uri. </summary>
    Poco::URI uri;

    void handleRequestOpenCamera(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestCloseCamera(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStart(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStop(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestStatus(HTTPServerRequest& request, HTTPServerResponse& response);

    //隐藏成员
    class Impl;
    Impl* _impl;
};
} // namespace dxlib