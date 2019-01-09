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

        /// <summary> 用于com初始化的计数. </summary>
        int comInitCount = 0;

        /// <summary> 设备名. </summary>
        char deviceNames[20][255] = { { 0 } };

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
        static DevicesHelper* GetInst();

        /// <summary> 一个设备名的列表. </summary>
        std::map<int, std::wstring>* devList;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 初始化com，如果调用过了摄像头的open函数，那么就不需要了. </summary>
        ///
        /// <remarks> Dx, 2017/7/16. </remarks>
        ///
        /// <returns> 成功返回True,失败返回false. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool comInit();

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 卸载com. </summary>
        ///
        /// <remarks> Dx, 2017/7/16. </remarks>
        ///
        /// <returns> 成功返回True,失败返回false. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool comUnInit();

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 列出设备列表. </summary>
        ///
        /// <remarks> Dx, 2017/7/16. </remarks>
        ///
        /// <param name="silent"> True to silent. </param>
        ///
        /// <returns> An int. </returns>
        ///-------------------------------------------------------------------------------------------------
        int listDevices(std::map<int, std::wstring>& devList);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 通过相机的名字得到相机的index(注意调用前需要先调用一次listDevices). </summary>
        ///
        /// <remarks> Dx, 2017/7/17. </remarks>
        ///
        /// <param name="name"> The name. </param>
        ///
        /// <returns> The index with name. </returns>
        ///-------------------------------------------------------------------------------------------------
        int getIndexWithName(std::wstring name);

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
        std::map<int, std::wstring> getDevListWithNames(const std::wstring name[], int length);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 列出设备列表,将结果写到对象成员. </summary>
        ///
        /// <remarks> Dx, 2017/7/16. </remarks>
        ///
        /// <returns> An int. </returns>
        ///-------------------------------------------------------------------------------------------------
        int listDevices();
    };
}