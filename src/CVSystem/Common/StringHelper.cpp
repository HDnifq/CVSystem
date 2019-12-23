﻿#include "string.h"
#include <locale.h>
#include "StringHelper.h"
#include <stdlib.h>

//#include <boost/date_time/posix_time/posix_time.hpp>
//#define BOOST_DATE_TIME_SOURCE  //这个定义不知道有什么用

////这个脚本可能会报错 语言->符合模式 "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

//使用标准库的ws转s
//#include <codecvt>
//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
//devNameA = converter.to_bytes(aDevName);

using namespace std;

namespace dxlib {

std::string StringHelper::ws2s(const std::wstring& ws)
{
    string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";

    setlocale(LC_ALL, "chs");

    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char* _Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);

#if defined(_WIN32) || defined(_WIN64)
    size_t res = 0;
    wcstombs_s(&res, _Dest, _Dsize, _Source, _Dsize); //换了个安全函数，未测
#else
    wcstombs(_Dest, _Source, _Dsize);
#endif

    string result = _Dest;
    delete[] _Dest;

    setlocale(LC_ALL, curLocale.c_str());

    return result;
}

///-------------------------------------------------------------------------------------------------
/// <summary> string2wstring. </summary>
///
/// <remarks> Dx, 2017/7/18. </remarks>
///
/// <param name="s"> The string to process. </param>
///
/// <returns> A wstring. </returns>
///-------------------------------------------------------------------------------------------------
std::wstring StringHelper::s2ws(const std::string& s)
{
    setlocale(LC_ALL, "chs");

    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
#if defined(_WIN32) || defined(_WIN64)
    size_t res = 0;
    mbstowcs_s(&res, _Dest, _Dsize, _Source, _Dsize); //换了个安全函数，未测
#else
    mbstowcs(_Dest, _Source, _Dsize);
#endif
    wstring result = _Dest;
    delete[] _Dest;

    setlocale(LC_ALL, "C");
    return result;
}

///-------------------------------------------------------------------------------------------------
/// <summary> Byte数据转成十六进制文本. </summary>
///
/// <remarks> Dx, 2018/1/8. </remarks>
///
/// <param name="data">   字节数组起始位置. </param>
/// <param name="length"> 长度. </param>
///
/// <returns> 十六进制字符串. </returns>
///-------------------------------------------------------------------------------------------------
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

///-------------------------------------------------------------------------------------------------
/// <summary> 得到当前时间戳. </summary>
///
/// <remarks> 剔除了这个函数,因为这个函数会导致引用一个boost库的依赖,在静态链接里比较麻烦
///           Dx, 2018/12/14. </remarks>
///
/// <returns> The time. </returns>
///-------------------------------------------------------------------------------------------------
//std::string StringHelper::secTimeStr()
//{
//    std::string strTime = boost::posix_time::to_iso_string(
//        boost::posix_time::second_clock::local_time());
//
//    // 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了
//
//    size_t pos = strTime.find('T');
//    strTime.replace(pos, 1, std::string("-"));
//    //strTime.replace(pos + 3, 0, std::string(":")); //这个冒号加上不能用作文件名了
//    //strTime.replace(pos + 6, 0, std::string(":"));
//
//    return strTime;
//}

} // namespace dxlib