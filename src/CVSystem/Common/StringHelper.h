#pragma once
#include <string>

namespace dxlib {

/**
 * 字符串辅助类.
 *
 * @author daixian
 * @date 2019/4/10
 */
class StringHelper
{
  public:
    /**
     * wstring转成string.(未规定string的编码,这个函数不好)
     *
     * @author daixian
     * @date 2017/7/18
     *
     * @param  ws 要处理的宽字符串.
     *
     * @returns string结果.
     */
    static std::string ws2s(const std::wstring& ws);

    /**
     * string转成wstring.(未规定string的编码,这个函数不好)
     *
     * @author daixian
     * @date 2017/7/18
     *
     * @param  s 要处理的字符串.
     *
     * @returns wstring结果.
     */
    static std::wstring s2ws(const std::string& s);

    /**
     * Byte数据转成文本.
     *
     * @author daixian
     * @date 2018/1/8
     *
     * @param  data   字节数组起始位置.
     * @param  length 长度.
     *
     * @returns 十六进制字符串.
     */
    static std::string byte2str(const void* data, int length);
};
typedef StringHelper S;
} // namespace dxlib