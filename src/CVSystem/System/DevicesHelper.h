#pragma once

#include <map>
#include <string>

namespace dxlib {

/**
 * 用来列出摄像头设备列表.
 *
 * @author daixian
 * @date 2017/7/16
 */
class DevicesHelper
{
  private:
    // 单例.
    static DevicesHelper* m_pInstance;

  public:
    // 构造.
    DevicesHelper();

    // 析构.
    ~DevicesHelper();

    /**
     * 获得单例.
     *
     * @author daixian
     * @date 2017/3/11
     *
     * @returns 返回单例对象，第一次调用会创建.
     */
    static DevicesHelper* GetInst()
    {
        if (m_pInstance == NULL) //判断是否第一次调用
            m_pInstance = new DevicesHelper();
        return m_pInstance;
    }

    // 一个设备名的列表.
    std::map<int, std::string> devList;

    /**
     * 通过相机的名字得到相机的index(注意调用前需要先调用一次listDevices), 如果不存在相机那么返回-1.
     *
     * @author daixian
     * @date 2017/7/17
     *
     * @param  name      相机的名字.
     * @param  isRegex   (Optional) 是否使用正则表达式匹配.
     * @param  isWarning (Optional) 如果没有找到相机是否提示警告日志,用于悄悄的检索当前系统相机.
     *
     * @returns 如果不存在相机那么返回-1.
     */
    int getIndexWithName(std::string name, bool isRegex = false, bool isWarning = true);

    /**
     * 输入一组需要的摄像机名字的得到一个设备列表(注意调用前需要先调用一次listDevices).
     *
     * @author daixian
     * @date 2017/7/18
     *
     * @param  name   摄像机名字数组.
     * @param  length 数组长度.
     *
     * @returns 设备的index和名字对应的map.
     */
    std::map<int, std::string> getDevListWithNames(const std::string name[], int length);

    /**
     * 列出设备列表,将结果写到对象成员.
     *
     * @author daixian
     * @date 2017/7/16
     *
     * @returns 列出设备的个数.
     */
    int listDevices();

    /**
     * (静态方法)列出设备列表.
     *
     * @author daixian
     * @date 2017/7/16
     *
     * @param [out] devList 输出的设备列表.
     *
     * @returns 列出设备的个数.
     */
    static int listDevices(std::map<int, std::wstring>& devList);
};
} // namespace dxlib