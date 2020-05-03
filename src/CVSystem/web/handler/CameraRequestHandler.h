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
class CameraRequestHandler : public HTTPRequestHandler
{
  public:
    enum class HandlerType
    {
        /// <summary>
        /// 没有支持的处理.
        /// </summary>
        none,

        /// <summary>
        /// 列出当前相机名
        /// </summary>
        listCamera,

        /// <summary>
        /// 得到当前相机的属性.
        /// </summary>
        getCapProp,

        /// <summary>
        /// 设置相机的属性.
        /// </summary>
        setCapProp,
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 判断是否有支持的Path处理. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///
    /// <param name="path"> Full pathname of the file. </param>
    ///
    /// <returns> 解析出来的处理类型. </returns>
    ///-------------------------------------------------------------------------------------------------
    static HandlerType ParsePath(const std::string& path)
    {
        if (path == "/camera/list") {
            return HandlerType::listCamera;
        }
        else if (path == "/camera/prop/get") {
            return HandlerType::getCapProp;
        }
        else if (path == "/camera/prop/set") {
            return HandlerType::setCapProp;
        }
        return HandlerType::none;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 如果能够包含支持的path就成功创建对象,否则返回null. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///
    /// <param name="uri">  URI of the resource. </param>
    /// <param name="path"> Full pathname of the file. </param>
    ///
    /// <returns>
    /// Null if it fails, else a pointer to a
    /// HTTPRequestHandler.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    static HTTPRequestHandler* creat(const URI& uri, const std::string& path)
    {
        auto ev = ParsePath(path);
        if (ev != HandlerType::none) {
            return new CameraRequestHandler(uri, ev);
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
    CameraRequestHandler(const Poco::URI& uri, const CameraRequestHandler::HandlerType& ev);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Destructor. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    virtual ~CameraRequestHandler();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Handles the request. </summary>
    ///
    /// <remarks> Dx, 2020/3/18. </remarks>
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

    void handleRequestListCamera(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestGetCapProp(HTTPServerRequest& request, HTTPServerResponse& response);
    void handleRequestSetCapProp(HTTPServerRequest& request, HTTPServerResponse& response);

    //隐藏成员
    class Impl;
    Impl* _impl;
};
} // namespace dxlib