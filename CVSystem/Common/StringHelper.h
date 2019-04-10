#pragma once
#ifndef _DXLIB_COMMON_H_
#define _DXLIB_COMMON_H_

#include <string>

namespace dxlib {

class StringHelper
{
  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> wstring2string. </summary>
    ///
    /// <remarks> Dx, 2017/7/18. </remarks>
    ///
    /// <param name="ws"> The ws. </param>
    ///
    /// <returns> A string. </returns>
    ///-------------------------------------------------------------------------------------------------
    static std::string ws2s(const std::wstring& ws);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> string2wstring. </summary>
    ///
    /// <remarks> Dx, 2017/7/18. </remarks>
    ///
    /// <param name="s"> The string to process. </param>
    ///
    /// <returns> A wstring. </returns>
    ///-------------------------------------------------------------------------------------------------
    static std::wstring s2ws(const std::string& s);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Byte数据转成文本. </summary>
    ///
    /// <remarks> Dx, 2018/1/8. </remarks>
    ///
    /// <param name="data">   字节数组起始位置. </param>
    /// <param name="length"> 长度. </param>
    ///
    /// <returns> 十六进制字符串. </returns>
    ///-------------------------------------------------------------------------------------------------
    static std::string byte2str(const void* data, int length);
};

} // namespace dxlib

#endif // !_DXLIB_COMMON_H_