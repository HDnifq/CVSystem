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
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/URI.h"
#include "Poco/File.h"

#include <vector>
#include <array>

using Poco::Net::HTTPRequest;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

namespace dxlib {
namespace cvsystem {

/**
 * 一个小的支持Range的文件服务器.
 *
 * @author daixian
 * @date 2020/3/14
 */
class FileHandler : public HTTPRequestHandler
{
  public:
    /**
     * 构造.
     *
     * @author daixian
     * @date 2020/4/30
     *
     * @param  rootDir 文件资源的路径.
     */
    FileHandler(const std::string& rootDir);

    /**
     * 析构
     *
     * @author daixian
     * @date 2020/4/30
     */
    ~FileHandler();

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

  private:
    /**
     * 解析范围.
     *
     * @author daixian
     * @date 2020/4/30
     *
     * @param       range       range文本.
     * @param       fileSize    资源文件大小.
     * @param [out] result      [起始,结束]的数组.
     *
     * @returns 成功的解析出了range.
     */
    bool parseRange(const std::string& range, Poco::File::FileSize fileSize,
                    std::vector<std::array<Poco::File::FileSize, 2>>& result);

    /**
     * 按Range发送这个文件.
     *
     * @author daixian
     * @date 2020/4/30
     *
     * @param [in,out] response    The response.
     * @param          path        Full pathname of the file.
     * @param          length      The length.
     * @param          rangeStart  The range start.
     * @param          rangeLength Length of the range.
     */
    void sendFileRange(Poco::Net::HTTPServerResponse& response, const std::string& path,
                       Poco::File::FileSize length, Poco::File::FileSize rangeStart, Poco::File::FileSize rangeLength);

    void sendFileRangeMultipart(Poco::Net::HTTPServerResponse& response, const std::string& path, const std::string& con_type,
                                Poco::File::FileSize length, std::vector<std::array<Poco::File::FileSize, 2>>& result);

    class Impl;
    Impl* _impl;
};

} // namespace cvsystem
} // namespace dxlib