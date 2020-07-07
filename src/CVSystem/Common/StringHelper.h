#pragma once
#include <string>

namespace dxlib {
namespace cvsystem {

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
     * wstring转到utf8编码的字符串.
     *
     * @author daixian
     * @date 2017/7/18
     *
     * @param  ws utf16的字符串.
     *
     * @returns utf8的字符串.
     */
    static std::string ws2s(const std::wstring& ws);

    /**
     * utf8编码的字符串转到utf16.
     *
     * @author daixian
     * @date 2017/7/18
     *
     * @param  s utf8的字符串
     *
     * @returns utf16的宽字符串.
     */
    static std::wstring s2ws(const std::string& s);

    /**
     * utf8转到gbk.
     *
     * @author daixian
     * @date 2020/4/23
     *
     * @param  s_utf8 utf8编码的字符串.
     *
     * @returns gbk编码的字符串.
     */
    static std::string utf8_to_gbk(const std::string& s_utf8);

    /**
     * gbk转到utf8.
     *
     * @author daixian
     * @date 2020/4/23
     *
     * @param  s_gbk gbk编码的字符串.
     *
     * @returns utf8编码的字符串.
     */
    static std::string gbk_to_utf8(const std::string& s_gbk);

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
    static std::string byte2str(const void* data, int length);

    /**
     * 得到当前时间戳.
     * 最后文件的形式如20191228-023549.
     *
     * @author daixian
     * @date 2018/12/14
     *
     * @returns %Y%m%d-%H%M%S的字符串.
     */
    static std::string secTimeStr();

    /**
     * 得到sqlite格式的时间戳,形如2020-05-23 09:37:26.
     *
     * @author daixian
     * @date 2020/5/23
     *
     * @returns 时间字符串.
     */
    static std::string sqliteDateTime();
};
typedef StringHelper S;

} // namespace cvsystem
} // namespace dxlib