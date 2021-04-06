#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/RegularExpression.h"
#include <iostream>

#include "./handler/MultiCameraRequestHandler.h"
#include "./handler/CameraRequestHandler.h"
#include "./handler/FileHandler.h"

#include "dlog/dlog.h"
#include "xuexue/csharp/csharp.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;
//using Poco::RegularExpression;

namespace dxlib {

class CVSystemRequestHandlerFactory : public HTTPRequestHandlerFactory
{
  public:
    CVSystemRequestHandlerFactory() {}

    // 是否使用文件服务器
    bool isUseFileServer = true;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// Handler, called when the create request.
    /// </summary>
    ///
    /// <remarks>
    /// 考虑嵌套Factory使用来实现多个模块的功能集成到一个Server
    ///  Dx, 2020/3/18.
    /// </remarks>
    ///
    /// <param name="request"> The request. </param>
    ///
    /// <returns> HTTPRequestHandler指针. </returns>
    ///-------------------------------------------------------------------------------------------------
    virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
    {
        std::string decodeText;
        Poco::URI::decode(request.getURI(), decodeText);
        Poco::URI uri(decodeText);
        std::string path(uri.getPath());

        auto handler = cvsystem::CameraRequestHandler::creat(uri, path);
        if (handler != nullptr) {
            return handler;
        }

        handler = cvsystem::MultiCameraRequestHandler::creat(uri, path);
        if (handler != nullptr) {
            return handler;
        }

        if (isUseFileServer) {
            using namespace xuexue::csharp;
            return new cvsystem::FileHandler(Path::Combine(Path::ModuleDir(), "static"));
        }
        else {
            // 按照Poco的API这里可能不能空
            return nullptr;
        }
    }

  private:
};

/**
 * ServerApplication.
 *
 * @author daixian
 * @date 2020/12/15
 */
class CVSystemHttpServer : public Poco::Util::ServerApplication
{
  public:
    CVSystemHttpServer() : _helpRequested(false)
    {
    }

    ~CVSystemHttpServer()
    {
    }

  protected:
    void initialize(Application& self) override
    {
        //目前没有配置文件
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize() override
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options) override
    {
        //最先会进这个函数
        ServerApplication::defineOptions(options);

        options.addOption(
            Option("help", "h", "display argument help information")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<CVSystemHttpServer>(this, &CVSystemHttpServer::handleHelp)));
    }

    void handleHelp(const std::string& name,
                    const std::string& value)
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "A web server that serves the current date and time.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    int main(const std::vector<std::string>& args) override
    {
        //在上面的内容加载完毕之后才会进入这个函数
        if (!_helpRequested) {

            if (config().getBool("application.runAsService", false)) {
                // do service specific things
                LogI("AppHttpServer.initialize():检测到作为服务运行的设置..");
            }

            unsigned short port = (unsigned short)config().getInt("HTTPTimeServer.port", 42015);
            std::string format(config().getString("HTTPTimeServer.format", DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket socket(port);
            HTTPServer srv(new CVSystemRequestHandlerFactory(), socket, new HTTPServerParams());
            LogI("服务器启动 port:%d...", port);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

  private:
    bool _helpRequested;
};

} // namespace dxlib
