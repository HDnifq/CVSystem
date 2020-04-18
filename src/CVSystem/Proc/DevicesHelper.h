#pragma once

#include <map>
#include <string>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 用来列出摄像头设备列表. </summary>
///
/// <remarks> Dx, 2017/7/16. </remarks>
///-------------------------------------------------------------------------------------------------
class DevicesHelper
{
  private:
    /// <summary> 单例. </summary>
    static DevicesHelper* m_pInstance;

  public:
    /// <summary> 构造. </summary>
    DevicesHelper();

    /// <summary> 析构. </summary>
    ~DevicesHelper();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 获得单例. </summary>
    ///
    /// <remarks> Dx, 2017/3/11. </remarks>
    ///
    /// <returns> 返回单例对象，第一次调用会创建. </returns>
    ///-------------------------------------------------------------------------------------------------
    static DevicesHelper* GetInst()
    {
        if (m_pInstance == NULL) //判断是否第一次调用
            m_pInstance = new DevicesHelper();
        return m_pInstance;
    }

    /// <summary> 一个设备名的列表. </summary>
    std::map<int, std::string> devList;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 通过相机的名字得到相机的index(注意调用前需要先调用一次listDevices),
    /// 如果不存在相机那么返回-1.
    /// </summary>
    ///
    /// <remarks> Dx, 2017/7/17. </remarks>
    ///
    /// <param name="name">    The name. </param>
    /// <param name="isRegex"> (Optional) 是否使用正则表达式匹配. </param>
    ///
    /// <returns> 如果不存在相机那么返回-1. </returns>
    ///-------------------------------------------------------------------------------------------------
    int getIndexWithName(std::string name, bool isRegex = false);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 输入一组需要的摄像机名字的得到一个设备列表(注意调用前需要先调用一次listDevices). </summary>
    ///
    /// <remarks> Dx, 2017/7/18. </remarks>
    ///
    /// <param name="name">   The name. </param>
    /// <param name="length"> The length. </param>
    ///
    /// <returns> The development list with names. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::map<int, std::string> getDevListWithNames(const std::string name[], int length);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 列出设备列表,将结果写到对象成员. </summary>
    ///
    /// <remarks> Dx, 2017/7/16. </remarks>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    int listDevices();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> (静态方法)列出设备列表. </summary>
    ///
    /// <remarks> Dx, 2017/7/16. </remarks>
    ///
    /// <param name="devList"> [out] 输出的设备列表. </param>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    static int listDevices(std::map<int, std::wstring>& devList);
};
} // namespace dxlib