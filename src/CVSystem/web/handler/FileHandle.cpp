#include "FileHandle.h"
#include <iostream>
#include <fstream>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/FileStream.h>

#include "dlog/dlog.h"
#include <regex>
#include <map>
#include <vector>

#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Format.h>

#include <stdlib.h>

using namespace Poco::Net;
using namespace Poco;

namespace dxlib {

class FileHandle::Impl
{
  public:
    Impl(std::string dirPath)
    {
        //剔除最后一个
        if (dirPath.back() == '\\' || dirPath.back() == '/')
            dirPath.erase(dirPath.end() - 1);

        Path root = Poco::Path(dirPath).absolute().makeDirectory().toString(); //拼接完整路径

        this->dirRoot = root.toString();
        File dir(root);
        if (!dir.exists())
            LogE("FileHandle.Impl():设置文件夹路径%s不存在，web资源可能不存在!", this->dirRoot.c_str());
    }

    ~Impl() {}

    /** 文件资源根目录，构造的时候设置. */
    std::string dirRoot;

    /** 文件扩展名对应的关联类型（静态类型）. */
    static std::map<std::string, std::string> content_type;

    /** The HTML 404. */
    const char* html404 = "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>poco/1.10.1 - dx's server</center></body></html>";

    /**
     * 输入相对路径,判断文件是否存在。 输入relativePath(uri)，返回fullPath和content_type文本.
     *
     * @author daixian
     * @date 2019/2/19
     *
     * @param [in]  relativePath 相对路径(UTF8).
     * @param [out] fullPath     输入完整路径.
     * @param [out] content_type content_type文本.
     *
     * @returns 如果文件存在返回true.
     */
    bool isFileExist(const std::string& relativePath, std::string& fullPath, std::string& content_type)
    {
        Path pfullPath = Poco::Path(dirRoot, relativePath).makeFile();
        ////返回的文件路径是UTF8的
        //fullPath = JsonHelper::utf16To8(pfullPath.wstring());
        File file(pfullPath);

        //如果文件存在
        if (file.exists() && file.isFile()) {
            if (pfullPath.getBaseName() != pfullPath.getFileName()) { //如果文件有扩展名
                std::string ext = pfullPath.getExtension();
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

    /**
     * 输入相对路径,判断文件夹是否存在，如果存在会生成一段文件夹的HTML文本.
     *
     * @author daixian
     * @date 2019/2/19
     *
     * @param       relativePath 相对路径(UTF8).
     * @param [out] html         HTML文本.
     *
     * @returns 文件夹存在返回true.
     */
    bool isDirExist(const std::string& relativePath, std::string& html)
    {
        if (relativePath.back() != '/') {
            return false;
        }
        html.clear();

        Path pfullPath = Poco::Path(dirRoot, relativePath).makeDirectory();
        File dir(pfullPath);

        //如果文件夹存在
        if (dir.exists() && dir.isDirectory()) {
            html = "<html><head><title>file server:" + relativePath + "</title></head><body><H2>" + relativePath + "</H2><hr><pre>";

            //2018/5/3    14:43        &lt;dir&gt; <A HREF="/Debug/">Debug</A><br>
            //2019/2/18    17:05         2742 <A HREF="/FilesDownload.zip">FilesDownload.zip</A><br>
            std::vector<Poco::File> vFiles;
            dir.list(vFiles);

            for (size_t i = 0; i < vFiles.size(); i++) {
                Poco::File& file = vFiles[i];

                if (file.exists() && file.isFile()) { //如果这个是文件
                    std::string fname = Path(file.path()).getFileName();
                    size_t filesize = file.getSize();
                    std::string uri = relativePath + fname;
                    //html += (boost::wformat(L"%10i  <A HREF=\"%s\">%s</A><br>") % filesize % uri % fname).str();
                    html += Poco::format("%10i  <A HREF=\"%s\">%s</A><br>", filesize, uri, fname);
                }
                else if (file.exists() && file.isDirectory()) { //如果这个是文件夹
                    std::string fname = Path(file.path()).getFileName();
                    std::string uri = relativePath + fname + "/";
                    //whtml += (boost::wformat(L"     &lt;dir&gt;  <A HREF=\"%s\">%s</A><br>") % uri % fname).str();
                    html += format("     &lt;dir&gt;  <A HREF=\"%s\">%s</A><br>", uri, fname);
                }
            }

            html += "</pre><hr></body></html>";
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
    try {
        LogI("FileHandle::handleRequest():进入了处理%s", req.getURI().c_str());

        //这里这个东西解码出来是UTF8的(直接看上去会像是乱码)
        string decStr;

        URI::decode(req.getURI(), decStr);
        string path = URI(decStr).getPath();

        LogI("FileHandle::handleRequest():解码uri=%s", decStr.c_str());
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
        if (path.back() == '/') {
            //如果这个目录下存在index.html
            if (_impl->isFileExist(path + "index.html", fullPath, con_type))
                path += "index.html";
        }

        if (_impl->isFileExist(path, fullPath, con_type)) { //如果存在文件那么就返回文件流
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
        else if (_impl->isDirExist(path, html)) { //如果是文件夹
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
    catch (const Poco::Exception& pe) {
        LogE("FileHandle.handleRequest():异常:%s,%s ", pe.what(), pe.message().c_str());
    }
    catch (const std::exception& e) {
        LogE("FileHandle.handleRequest():异常:%s", e.what());
    }
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