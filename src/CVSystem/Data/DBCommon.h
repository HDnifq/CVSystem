#pragma once

#include <vector>
#include <string>

namespace dxlib {

/**
 * 公共的数据库.
 *
 * @author daixian
 * @date 2020/5/18
 */
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

    /** 数据库文件路径. */
    std::string dbPath;

    /** 数据库加密的key. */
    std::string key;

    /**
     * 是否数据库已经打开了.
     *
     * @author daixian
     * @date 2019/3/12
     *
     * @returns 是否数据库已经打开了.
     */
    bool isOpened();

    /**
     * 直接把一个已经打开的数据库设置给它.
     *
     * @author daixian
     * @date 2020/5/18
     *
     * @param [in,out] db SQLite::Database*类型的指针.
     */
    void setPtr(void* db);

    /**
     * 得到当前的DB指针(SQLite::Database*).
     *
     * @author daixian
     * @date 2020/5/18
     *
     * @returns 类型为SQLite::Database*.
     */
    void* getPtr();

    /**
     * 创建新的数据库,会同时创建表.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  path  数据库路径(内存中 :memory: ).
     * @param  isKey (Optional) 是否加密.
     *
     * @returns 成功返回0,不成功返回-1.
     */
    int createNew(const std::string& path, bool isKey = true);

    /**
     * 打开一个数据库(只读).
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  path  数据库路径.
     * @param  isKey (Optional) 是否加密.
     *
     * @returns 成功返回0,不成功返回-1.
     */
    bool open(const std::string& path, bool isKey = true);

    /**
     * 打开一个数据库(读写).
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  path  数据库路径.
     * @param  isKey (Optional) 是否加密.
     *
     * @returns 成功返回0,不成功返回-1.
     */
    bool openRW(const std::string& path, bool isKey = true);

    /**
     * 关闭数据库.
     *
     * @author daixian
     * @date 2020/5/21
     */
    void close();

    /**
     * 创建它需要的表.
     *
     * @author daixian
     * @date 2020/5/21
     */
    void createTable();

    /**
     * 记录一项通用数据到数据库里.
     *
     * @author daixian
     * @date 2020/5/21
     *
     * @param  user_id 一个自定一个userid.
     * @param  name    这一项记录的名字,最好应该不重复.
     * @param  txt     这项记录的文本内容.
     * @param  data    这项记录的二进制数据.
     * @param  len     这项记录的二进制数据长度.
     *
     * @returns 修改的项数.
     */
    int write(int user_id, const std::string& name, const std::string& txt, const void* data, int len);

    /**
     * Writes
     *
     * @author daixian
     * @date 2020/5/25
     *
     * @param  user_id 一个自定一个userid.
     * @param  name    这一项记录的名字,最好应该不重复.
     * @param  txt     这项记录的文本内容.
     *
     * @returns 修改的项数.
     */
    int write(int user_id, const std::string& name, const std::string& txt);

    /**
     * 从数据库里读取一项通用数据.
     *
     * @author daixian
     * @date 2020/5/18
     *
     * @param       user_id 用户自定的user_id.
     * @param       name    这项数据的名字.
     * @param [out] txt     这项记录的文本内容.
     * @param [out] data    这项记录的二进制数据.
     *
     * @returns 修改的项数.
     */
    int readOne(int user_id, const std::string& name, std::string& txt, std::vector<char>& data);

    /**
     * 从数据库里读取一项通用数据的text文本.
     *
     * @author daixian
     * @date 2020/5/18
     *
     * @param  name    这项数据的名字.
     * @param  user_id (Optional) user_id默认一般是0.
     *
     * @returns 结果文本,失败则返回空字符串"".
     */
    std::string readOneText(const std::string& name, int user_id = 0);

    /**
     * 当前common表中的项数.
     *
     * @author daixian
     * @date 2020/5/18
     *
     * @returns 项数,如果为-1则数据库没有打开.
     */
    int itemCount();

  private:
    static DBCommon* m_pInstance;

    class Impl;
    Impl* _impl;
};

} // namespace dxlib