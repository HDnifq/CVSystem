#include <string>
#include <locale.h>
#include "StringHelper.h"
#include <stdlib.h>

#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/Format.h"

#include <Poco/TextConverter.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/Windows936Encoding.h>

//利用rapidjson做utf8和utf16的转换
#include "rapidjson/encodings.h"
#include "rapidjson/stringbuffer.h"

typedef rapidjson::GenericStringStream<rapidjson::UTF16<>> StringStreamW;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<>> StringBufferW;

#if defined(_WIN32) || defined(_WIN64)
#    include <windows.h>

//一个windows下的实验的函数,设置控制台彩色的
//void setConsoleMode()
//{
//    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//    if (hOut != INVALID_HANDLE_VALUE) {
//        DWORD dwMode = 0;
//        GetConsoleMode(hOut, &dwMode);
//
//        dwMode |= 0x0004;
//        SetConsoleMode(hOut, dwMode);
//    }
//}

#elif defined(__linux__)
/* Linux. --------------------------------------------------- */

#endif

namespace dxlib {
namespace cvsystem {

using namespace std;

string StringHelper::ws2s(const wstring& ws)
{
    using namespace rapidjson;
    StringStreamW source(ws.c_str());
    StringBuffer target;
    bool hasError = false;
    while (source.Peek() != L'\0')
        if (!Transcoder<UTF16<>, UTF8<>>::Transcode(source, target)) {
            hasError = true;
            break;
        }
    if (!hasError) {
        return target.GetString();
    }
    return std::string();
}

wstring StringHelper::s2ws(const string& s)
{
    using namespace rapidjson;
    StringStream source(s.c_str());
    StringBufferW target;
    bool hasError = false;
    while (source.Peek() != '\0')
        if (!Transcoder<UTF8<>, UTF16<>>::Transcode(source, target)) {
            hasError = true;
            break;
        }
    if (!hasError) {
        return target.GetString();
    }
    return std::wstring();
}

/**
 * Byte数据转成16进制文本.
 *
 * @author daixian
 * @date 2018/1/8
 *
 * @param  data   字节数组起始位置.
 * @param  length 长度.
 *
 * @returns 十六进制字符串.
 */
std::string StringHelper::byte2str(const void* data, int length)
{
    unsigned char* pChar = (unsigned char*)data;
    std::string msg;
    for (int i = 0; i < length; i++) {
        char b[8];

#if defined(_WIN32) || defined(_WIN64)
        sprintf_s(b, 8, "%02X ", pChar[i]);
#else
        snprintf(b, 8, "%02X ", pChar[i]);
#endif
        msg.append(b);
    }
    return msg;
}

/**
 * 得到当前时间戳.
 *
 * @author daixian
 * @date 2018/12/14
 *
 * @returns The time.
 */
std::string StringHelper::secTimeStr()
{
    //最后文件的形式如init_close.20191228-023549.log
    using namespace Poco;
    LocalDateTime now;
    std::string str = DateTimeFormatter::format(now, "%Y%m%d-%H%M%S");
    return str;
}

std::string StringHelper::sqliteDateTime()
{
    //2020-05-23 09:37:26
    using namespace Poco;
    LocalDateTime now;
    std::string str = DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S");
    return str;
}

std::string StringHelper::utf8_to_gbk(const std::string& s)
{
    Poco::Windows936Encoding cp936;
    Poco::UTF8Encoding utf8;
    Poco::TextConverter converter(utf8, cp936);
    std::string dst;
    converter.convert(s, dst);
    return dst;
}

std::string StringHelper::gbk_to_utf8(const std::string& s)
{
    Poco::Windows936Encoding cp936;
    Poco::UTF8Encoding utf8;
    Poco::TextConverter converter(cp936, utf8);
    std::string dst;
    converter.convert(s, dst);
    return dst;
}

} // namespace cvsystem
} // namespace dxlib