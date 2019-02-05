#include <string>
#include <locale.h>
#include "Common.h"

#include <boost/date_time/posix_time/posix_time.hpp>
//#define BOOST_DATE_TIME_SOURCE  //这个定义不知道有什么用

////这个脚本可能会报错 语言->符合模式 "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

//使用标准库的ws转s
//#include <codecvt>
//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
//devNameA = converter.to_bytes(aDevName);

std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, "chs");

    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char* _Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);

    size_t i;
    wcstombs_s(&i, _Dest, _Dsize, _Source, _Dsize);
    std::string result = _Dest;
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
std::wstring s2ws(const std::string& s)
{
    std::string curLocale = setlocale(LC_ALL, "chs");

    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);

    size_t i;
    mbstowcs_s(&i, _Dest, _Dsize, _Source, _Dsize);
    std::wstring result = _Dest;
    delete[] _Dest;

    setlocale(LC_ALL, curLocale.c_str());
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
std::string byte2str(const void* data, int length)
{
    unsigned char* pChar = (unsigned char*)data;
    std::string msg;
    for (int i = 0; i < length; i++) {
        char b[8];
        sprintf_s(b, 8, "%02X ", pChar[i]);
        msg.append(b);
    }
    return msg;
}

///-------------------------------------------------------------------------------------------------
/// <summary> 得到当前时间戳. </summary>
///
/// <remarks> Dx, 2018/12/14. </remarks>
///
/// <returns> The time. </returns>
///-------------------------------------------------------------------------------------------------
std::string secTimeStr()
{
    std::string strTime = boost::posix_time::to_iso_string(
        boost::posix_time::second_clock::local_time());

    // 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了

    size_t pos = strTime.find('T');
    strTime.replace(pos, 1, std::string("-"));
    //strTime.replace(pos + 3, 0, std::string(":")); //这个冒号加上不能用作文件名了
    //strTime.replace(pos + 6, 0, std::string(":"));

    return strTime;
}
