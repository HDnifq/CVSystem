#pragma once

#include "SQLiteCpp/SQLiteCpp.h"

namespace dxlib {

class DBHelper
{
  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> 更新一行记录里的一个值. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="db">              [in]要操作的数据库. </param>
    /// <param name="table_name">      要修改的表名. </param>
    /// <param name="where_col_name">  选择判断的列名. </param>
    /// <param name="where_row_value"> 选择判断的行的值. </param>
    /// <param name="update_col_name"> 要更新的列名. </param>
    /// <param name="update_value">    要更新的值. </param>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    static int UPDATE(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& update_col_name, const std::string& update_value);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 查询一项信息. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="db">              [in]要操作的数据库.. </param>
    /// <param name="table_name">      要查询的表名. </param>
    /// <param name="where_col_name">  选择判断的列名. </param>
    /// <param name="where_row_value"> 选择判断的行的值. </param>
    /// <param name="select_col_name"> 要查询的列名. </param>
    /// <param name="value">           [out]查询结果. </param>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    static int SELECT(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& select_col_name, std::string& value);

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 查询一项信息. </summary>
    ///
    /// <remarks> Dx, 2019/3/12. </remarks>
    ///
    /// <param name="db">              [in]要操作的数据库.. </param>
    /// <param name="table_name">      要查询的表名. </param>
    /// <param name="where_col_name">  选择判断的列名. </param>
    /// <param name="where_row_value"> 选择判断的行的值. </param>
    /// <param name="select_col_name"> 要查询的列名. </param>
    /// <param name="value">           [out]查询结果. </param>
    ///
    /// <returns> An int. </returns>
    ///-------------------------------------------------------------------------------------------------
    static int SELECT(SQLite::Database* db, const std::string& table_name,
                      const std::string& where_col_name, const std::string& where_row_value,
                      const std::string& select_col_name, unsigned int& value);
};

} // namespace dxlib