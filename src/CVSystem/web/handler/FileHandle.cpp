#include "FileHandle.h"
#include <iostream>
#include <fstream>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/FileStream.h>

#include "xuexuejson/Serialize.hpp"
#include "dlog/dlog.h"
#include <regex>

#include <boost/filesystem.hpp>
//#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <stdlib.h>

using namespace Poco::Net;
using namespace Poco;
namespace fs = boost::filesystem;
using namespace xuexue::json;

namespace dxlib {

class FileHandle::Impl
{
  public:
    Impl(std::string dirPath)
    {
        //剔除最后一个
        if (dirPath.back() == '\\' || dirPath.back() == '/')
            dirPath.erase(dirPath.end() - 1);

        fs::path root(dirPath); //拼接完整路径
        if (fs::is_directory(root))
            this->dirRoot = root.wstring();
        else
            LogE("FileHandle.Impl():设置路径不是文件夹，web资源可能不存在!");
    }
    ~Impl() {}

    /// <summary> 文件资源根目录，构造的时候设置. </summary>
    std::wstring dirRoot;

    /// <summary> 文件扩展名对应的关联类型（静态类型）. </summary>
    static std::map<std::string, std::string> content_type;

    /// <summary> The HTML 404. </summary>
    const char* html404 = "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>poco/1.10.1 - dx's server</center></body></html>";

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 输入相对路径,判断文件是否存在。
    /// 输入relativePath(uri)，返回fullPath和content_type文本.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/2/19. </remarks>
    ///
    /// <param name="relativePath"> [in] 相对路径(UTF8). </param>
    /// <param name="fullPath">     [out] 输入完整路径. </param>
    /// <param name="content_type"> [out] content_type文本. </param>
    ///
    /// <returns> 如果文件存在返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isFileExist(const std::string& relativePath, std::string& fullPath, std::string& content_type)
    {
        std::wstring wuri = JsonHelper::utf8To16(relativePath);

        fs::path pfullPath = fs::path(dirRoot) / fs::path(wuri);
        //返回的文件路径是UTF8的
        fullPath = JsonHelper::utf16To8(pfullPath.wstring());

        if (fs::is_regular_file(pfullPath)) { //如果文件存在
            if (pfullPath.has_extension()) {  //如果文件有扩展名
                std::string ext = pfullPath.extension().string();
                content_type = this->content_type[ext]; //尝试搜索对应的content-type
                if (content_type.empty()) {
                    content_type = "application/octet-stream";
                }
            }
            else {
                content_type = "application/octet-stream";
            }
            return true;
        }
        return false;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 输入相对路径,判断文件夹是否存在，如果存在会生成一段文件夹的HTML文本.
    /// </summary>
    ///
    /// <remarks> Dx, 2019/2/19. </remarks>
    ///
    /// <param name="relativePath"> 相对路径(UTF8). </param>
    /// <param name="html">         [out] HTML文本. </param>
    ///
    /// <returns> 文件夹存在返回true. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isDirExist(const std::string& relativePath, std::string& html)
    {
        std::wstring wRelativePath = JsonHelper::utf8To16(relativePath);
        if (relativePath.back() != L'/') {
            return false;
        }
        //这个函数里面都使用UTF16最后再转UTF8,免得文件系统编码有问题
        std::wstring whtml;
        fs::path fullPath = fs::path(dirRoot) / fs::path(wRelativePath);

        if (fs::is_directory(fullPath)) { //如果文件夹存在
            whtml = L"<html><head><title>file server:" + wRelativePath + L"</title></head><body><H2>" + wRelativePath + L"</H2><hr><pre>";

            //2018/5/3    14:43        &lt;dir&gt; <A HREF="/Debug/">Debug</A><br>
            //2019/2/18    17:05         2742 <A HREF="/FilesDownload.zip">FilesDownload.zip</A><br>

            fs::directory_iterator di(fullPath);
            for (auto& de : di) {
                if (fs::is_regular_file(de.status())) { //如果这个是文件
                    std::wstring fname = de.path().filename().wstring();
                    size_t filesize = fs::file_size(de.path());
                    std::wstring uri = wRelativePath + fname;
                    whtml += (boost::wformat(L"%10i  <A HREF=\"%s\">%s</A><br>") % filesize % uri % fname).str();
                }
                else if (fs::is_directory(de.status())) { //如果这个是文件夹
                    std::wstring fname = de.path().filename().wstring();
                    std::wstring uri = wRelativePath + fname + L"/";
                    whtml += (boost::wformat(L"     &lt;dir&gt;  <A HREF=\"%s\">%s</A><br>") % uri % fname).str();
                }
            }
            whtml += L"</pre><hr></body></html>";
            html = JsonHelper::utf16To8(whtml);
            return true;
        }
        return false;
    }
};

//静态的成员在类外初始化
std::map<std::string, std::string> FileHandle::Impl::content_type = {{".html", "text/html;charset=utf-8"},
                                                                     {".htm", "text/html;charset=utf-8"},
                                                                     {".stm", "text/html;charset=utf-8"},
                                                                     {".css", "text/css"},
                                                                     {".323", "text/h323"},
                                                                     {".txt", "text/plain"},
                                                                     {".rtx", "text/richtext"},
                                                                     {".json", "application/json;charset=utf-8"},
                                                                     {".js", "application/x-javascript"},
                                                                     {".mp3", "audio/mp3"},
                                                                     {".wav", "audio/x-wav"},
                                                                     {".m3u", "audio/x-mpegurl"},
                                                                     {".mp2", "video/mpeg"},
                                                                     {".mpa", "video/mpeg"},
                                                                     {".mpe", "video/mpeg"},
                                                                     {".mpeg", "video/mpeg"},
                                                                     {".mpg", "video/mpeg"},
                                                                     {".mpv2", "video/mpeg"},
                                                                     {".m4e", "video/mp4"},
                                                                     {".m4a", "video/mp4"},
                                                                     {".mp4", "video/mp4"},
                                                                     {".mkv", "video/x-matroska"},
                                                                     {".bmp", "image/bmp"},
                                                                     {".gif", "image/gif"},
                                                                     {".mid", "image/mid"},
                                                                     {".jpe", "image/jpeg"},
                                                                     {".jpeg", "image/jpeg"},
                                                                     {".jpg", "image/jpeg"},
                                                                     {".tif", "image/tiff"},
                                                                     {".tiff", "image/tiff"},
                                                                     {".ico", "image/x-icon"}};

//构造
FileHandle::FileHandle(const std::string& rootDir)
{
    _impl = new Impl(rootDir);
}
FileHandle::~FileHandle()
{
    delete _impl;
}

void FileHandle::handleRequest(HTTPServerRequest& req, HTTPServerResponse& response)
{
    using namespace std;

    LogI("FileHandle::handleRequest():进入了处理%s", req.getURI().c_str());

    //这里这个东西解码出来是UTF8的(直接看上去会像是乱码)
    string uri;
    URI::decode(req.getURI(), uri);
    LogI("FileHandle::handleRequest():解码uri=%s", uri.c_str());
    //wstring wuri = json::JsonHelper::utf8To16(uri);

    //输出一下http的key
    for (auto& kvp : req) {
        LogI("FileHandle::handleRequest():key=%s,value=%s", kvp.first.c_str(), kvp.second.c_str());
    }

    bool hasContentLength = req.hasContentLength();

    string fullPath;
    string con_type;
    string html;

    //如果目录下存在index.html那么就使用index.html
    if (uri.back() == '/') {
        //如果这个目录下存在index.html
        if (_impl->isFileExist(uri + "index.html", fullPath, con_type))
            uri += "index.html";
    }

    if (_impl->isFileExist(uri, fullPath, con_type)) { //如果存在文件那么就返回文件流
        //这是支持range的文件流
        Poco::File mediaFile(fullPath);
        Poco::Timestamp dateTime = mediaFile.getLastModified();
        Poco::File::FileSize length = mediaFile.getSize();

        if (req.getMethod() == Poco::Net::HTTPRequest::HTTP_HEAD ||
            req.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            response.set("Last-Modified", Poco::DateTimeFormatter::format(dateTime, Poco::DateTimeFormat::HTTP_FORMAT));
            response.set("Accept-Ranges", "bytes"); //表示服务器自己支持Range
            response.setContentType(con_type);
        }

        if (req.getMethod() == Poco::Net::HTTPRequest::HTTP_HEAD) {
            response.setContentLength64(length);
            response.send();
        }
        else if (req.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            if (req.has("Range")) {
                Poco::File::FileSize rangeStart = 0;
                Poco::File::FileSize rangeLength = 0;
                if (parseRange(req.get("Range"), length, rangeStart, rangeLength)) {
                    LogD("FileHandle::handleRequest():解析Range是 start=%llu , len=%llu", rangeStart, rangeLength);
                    //Range的上限是4M一次
                    if (rangeLength > 1024 * 1024 * 4) {
                        rangeLength = 1024 * 1024 * 4;
                    }
                    sendFileRange(response, fullPath, length, rangeStart, rangeLength);
                }
                else {
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_REQUESTED_RANGE_NOT_SATISFIABLE);
                    response.send();
                }
            }
            else {
                response.sendFile(fullPath, con_type);
            }
        }
        else {
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
            response.send();
        }
    }
    else if (_impl->isDirExist(uri, html)) { //如果是文件夹
        response.setStatus(HTTPResponse::HTTP_OK);
        response.setContentType("text/html;charset=utf-8");
        std::ostream& out = response.send();
        out << html;
        //out.flush();
    }
    else { //如果不存在文件那么就404
        response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        response.setContentType("text/html;charset=utf-8");
        std::ostream& out = response.send();
        out << _impl->html404;
        //out.flush();
    }
    LogI("FileHandle::handleRequest():handle处理结束!");
}

bool FileHandle::parseRange(const std::string& range, Poco::File::FileSize fileSize, Poco::File::FileSize& rangeStart, Poco::File::FileSize& rangeLength)
{
    using namespace std;
    std::smatch sm;
    //实际上也就只需要提取被分割的两个整数就完了
    if (regex_search(range, sm, std::regex("[0-9]+"))) {
        rangeStart = std::stoull(sm[0].str()); //第一个是起点
        if (sm.size() >= 2) {
            rangeLength = std::stoull(sm[1].str()); //第二个是获取长度
        }
        else {
            rangeLength = fileSize - rangeStart;
        }
    }
    else {
        return false;
    }
    //安全检查
    if (rangeStart >= fileSize) {
        return false;
    }
    if (rangeStart + rangeLength > fileSize) {
        rangeLength = fileSize - rangeStart;
    }
    return true;
}

void FileHandle::sendFileRange(Poco::Net::HTTPServerResponse& response, const std::string& path, Poco::File::FileSize length, Poco::File::FileSize rangeStart, Poco::File::FileSize rangeLength)
{
    using namespace std;
    int BUFFER_SIZE = 8192;
    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_PARTIAL_CONTENT);
    response.setContentLength64(rangeLength);
    string contentRange = Poco::format("bytes %?d-%?d/%?d", rangeStart, rangeStart + rangeLength - 1, length);
    LogD("FileHandle.sendFileRange():返回contentRange%s", contentRange.c_str());
    response.set("Content-Range", contentRange);
    std::ostream& ostr = response.send();

    Poco::FileInputStream istr(path);
    istr.seekg(static_cast<std::streampos>(rangeStart));
    Poco::Buffer<char> buffer(BUFFER_SIZE);
    while (rangeLength > 0) {
        std::streamsize chunk = BUFFER_SIZE;
        if (chunk > rangeLength) chunk = static_cast<std::streamsize>(rangeLength);
        istr.read(buffer.begin(), chunk);
        std::streamsize n = istr.gcount();
        if (n == 0) break;
        ostr.write(buffer.begin(), n);
        rangeLength -= n;
    }
}

} // namespace dxlib