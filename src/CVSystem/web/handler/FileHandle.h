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

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

namespace dxlib {

/**
 * 一个小的支持Range的文件服务器.
 *
 * @author daixian
 * @date 2020/3/14
 */
class FileHandle : public HTTPRequestHandler
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
    FileHandle(const std::string& rootDir);

    /**
     * 析构
     *
     * @author daixian
     * @date 2020/4/30
     */
    ~FileHandle();

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
     * @param [out] rangeStart  range的起始.
     * @param [out] rangeLength range的长度.
     *
     * @returns 成功的解析出了range.
     */
    bool parseRange(const std::string& range, Poco::File::FileSize fileSize, Poco::File::FileSize& rangeStart, Poco::File::FileSize& rangeLength);

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

    class Impl;
    Impl* _impl;
};
} // namespace dxlib