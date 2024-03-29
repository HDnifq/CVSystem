﻿#pragma once

#include "SQLiteCpp/SQLiteCpp.h"

namespace dxlib {

class DBHelper
{
  public:
    /**
     * 更新一行记录里的一个值.
     *
     * @author daixian
     * @date 2019/3/12
     *
     * @param [in] db              要操作的数据库.
     * @param      table_name      要修改的表名.
     * @param      where_col_name  选择判断的列名.
     * @param      where_row_value 选择判断的行的值.
     * @param      update_col_name 要更新的列名.
     * @param      update_value    要更新的值.
     *
     * @returns 生效的行数.
     */
    static int UPDATE(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& update_col_name, const std::string& update_value);

    /**
     * 查询一项信息.
     *
     * @author daixian
     * @date 2019/3/12
     *
     * @param [in]  db              要操作的数据库.
     * @param       table_name      要查询的表名.
     * @param       where_col_name  选择判断的列名.
     * @param       where_row_value 选择判断的行的值.
     * @param       select_col_name 要查询的列名.
     * @param [out] value           查询结果.
     *
     * @returns 生效的行数.
     */
    static int SELECT(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& select_col_name, std::string& value);

    /**
     * 查询一项信息.
     *
     * @author daixian
     * @date 2019/3/12
     *
     * @param [in]  db              要操作的数据库.
     * @param       table_name      要查询的表名.
     * @param       where_col_name  选择判断的列名.
     * @param       where_row_value 选择判断的行的值.
     * @param       select_col_name 要查询的列名.
     * @param [out] value           查询结果.
     *
     * @returns 生效的行数.
     */
    static int SELECT(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& select_col_name, unsigned int& value);

    /**
     * 得到当前时间戳字符串."%Y%m%d-%H%M%S"形如20191228-023549
     *
     * @author daixian
     * @date 2020/12/5
     *
     * @returns 时间字符串.
     */ 
    static std::string SecTimeStr();

    /**
     * 得到sqlite格式的时间戳字符串,"%Y-%m-%d %H:%M:%S"形如2020-05-23 09:37:26.
     *
     * @author daixian
     * @date 2020/5/23
     *
     * @returns 时间字符串.
     */
    static std::string SqliteDateTime();
};

} // namespace dxlib