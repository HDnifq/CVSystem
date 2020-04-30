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

///-------------------------------------------------------------------------------------------------
/// <summary> 一个小的支持Range的文件服务器.. </summary>
///
/// <remarks> Dx, 2020/3/14. </remarks>
///-------------------------------------------------------------------------------------------------
class FileHandle : public HTTPRequestHandler
{
  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> Constructor. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="rootDir"> 文件资源的路径. </param>
    ///-------------------------------------------------------------------------------------------------
    FileHandle(const std::string& rootDir);
    ~FileHandle();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> override处理. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="request">  [in,out] The request. </param>
    /// <param name="response"> [in,out] The response. </param>
    ///-------------------------------------------------------------------------------------------------
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override;

  private:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> 解析范围. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="range">       range文本. </param>
    /// <param name="fileSize">    资源文件大小. </param>
    /// <param name="rangeStart">  [out] range的起始. </param>
    /// <param name="rangeLength"> [out] range的长度. </param>
    ///
    /// <returns> 成功的解析出了range. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool parseRange(const std::string& range, Poco::File::FileSize fileSize, Poco::File::FileSize& rangeStart, Poco::File::FileSize& rangeLength);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 按Range发送这个文件. </summary>
    ///
    /// <remarks> Dx, 2020/4/30. </remarks>
    ///
    /// <param name="response">    [in,out] The response. </param>
    /// <param name="path">        Full pathname of the
    ///                            file. </param>
    /// <param name="length">      The length. </param>
    /// <param name="rangeStart">  The range start. </param>
    /// <param name="rangeLength"> Length of the range. </param>
    ///-------------------------------------------------------------------------------------------------
    void sendFileRange(Poco::Net::HTTPServerResponse& response, const std::string& path,
                       Poco::File::FileSize length, Poco::File::FileSize rangeStart, Poco::File::FileSize rangeLength);

    class Impl;
    Impl* _impl;
};
} // namespace dxlib