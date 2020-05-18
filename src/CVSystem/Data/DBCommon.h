#pragma once
#include "../Model/StereoCamera.h"
#include "../Model/StereoCameraParam.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 系统相机参数的数据库. </summary>
///
/// <remarks> Dx, 2020/5/18. </remarks>
///-------------------------------------------------------------------------------------------------
class DBCommon
{
  public:
    DBCommon();
    ~DBCommon();
    static DBCommon* GetInst()
    {
        if (m_pInstance == nullptr)
            m_pInstance = new DBCommon();
        return m_pInstance;
    }

    /// <summary> 数据库文件路径. </summary>
    std::string dbPath;

    /// <summary> 数据库加密的key. </summary>
    std::string key;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 是否数据库已经打开了. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <returns>
    /// True if database file exists, false if not.
    /// </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isOpened();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 直接把一个已经打开的数据库设置给它. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <param name="db"> SQLite::Database*类型的指针. </param>
    ///-------------------------------------------------------------------------------------------------
    void setPtr(void* db);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到当前的DB指针(SQLite::Database*). </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <returns> 类型为SQLite::Database*. </returns>
    ///-------------------------------------------------------------------------------------------------
    void* getPtr();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 创建新的数据库,会同时创建表. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="path">  数据库路径. </param>
    /// <param name="isKey"> (Optional) 是否加密. </param>
    ///
    /// <returns> 成功返回0,不成功返回-1. </returns>
    ///-------------------------------------------------------------------------------------------------
    int createNew(const std::string& path, bool isKey = true);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开一个数据库(只读). </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="path">  数据库路径. </param>
    /// <param name="isKey"> (Optional) 是否加密. </param>
    ///
    /// <returns> 成功返回0,不成功返回-1. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool open(const std::string& path, bool isKey = true);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 打开一个数据库(读写). </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="path">  数据库路径. </param>
    /// <param name="isKey"> (Optional) 是否加密. </param>
    ///
    /// <returns> 成功返回0,不成功返回-1. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool openRW(const std::string& path, bool isKey = true);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 关闭数据库. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void close();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 创建它需要的表. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void createTable();

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 记录一项通用数据到数据库里. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <param name="user_id"> 一个自定一个userid. </param>
    /// <param name="name">    这一项记录的名字,最好应该不重复. </param>
    /// <param name="txt">     这项记录的文本内容. </param>
    /// <param name="data">    这项记录的二进制数据. </param>
    /// <param name="len">     这项记录的二进制数据长度. </param>
    ///
    /// <returns> 修改的项数. </returns>
    ///-------------------------------------------------------------------------------------------------
    int write(int user_id, const std::string& name, const std::string& txt, const void* data, int len);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从数据库里读取一项通用数据. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <param name="user_id"> 用户自定的user_id. </param>
    /// <param name="name">    这项数据的名字. </param>
    /// <param name="txt">     [out] The text. </param>
    /// <param name="data">    [out] The data. </param>
    ///
    /// <returns> 修改的项数. </returns>
    ///-------------------------------------------------------------------------------------------------
    int readOne(int user_id, const std::string& name, std::string& txt, std::vector<char>& data);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从数据库里读取一项通用数据的text文本. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <param name="name">    The name. </param>
    /// <param name="user_id"> (Optional) user_id默认一般是0. </param>
    ///
    /// <returns> 结果文本,失败则返回"". </returns>
    ///-------------------------------------------------------------------------------------------------
    std::string readOneText(const std::string& name, int user_id = 0);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 当前common表中的项数. </summary>
    ///
    /// <remarks> Dx, 2020/5/18. </remarks>
    ///
    /// <returns> 项数,如果为-1则数据库没有打开. </returns>
    ///-------------------------------------------------------------------------------------------------
    int itemCount();

  private:
    static DBCommon* m_pInstance;

    class Impl;
    Impl* _impl;
};

} // namespace dxlib