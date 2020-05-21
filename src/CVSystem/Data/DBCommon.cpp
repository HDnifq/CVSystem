#include "DBCommon.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "SQLiteCpp/SQLiteCpp.h"

#include "DBHelper.h"
#include "dlog/dlog.h"

using namespace std;
namespace fs = boost::filesystem;

namespace dxlib {

class DBCommon::Impl
{
  public:
    Impl()
    {
    }
    ~Impl()
    {
        if (db != nullptr) {
            delete db;
        }
    }

    /** The database. */
    SQLite::Database* db = nullptr;

  private:
};

DBCommon* DBCommon::m_pInstance = nullptr;

DBCommon::DBCommon()
{
    _impl = new Impl();

    key = "xX--xuexue--Xx";
}

DBCommon::~DBCommon()
{
    delete _impl;
}

void DBCommon::setPtr(void* db)
{
    if (db != nullptr)
        _impl->db = (SQLite::Database*)db;
    else
        LogE("DBCommon.setPtr():传入指针不能为null!");
}

void* DBCommon::getPtr()
{
    return _impl->db;
}

bool DBCommon::open(const std::string& path, bool isKey)
{
    if (_impl->db != nullptr) {
        delete _impl->db;
    }
    //如果文件存在
    if (fs::is_regular_file(path)) {
        _impl->db = new SQLite::Database(path, SQLite::OPEN_READONLY);
        if (isKey) {
            _impl->db->key(this->key); //加密
        }

        if (_impl->db != nullptr) {
            LogI("DBCommon.open():打开数据库成功!");
            dbPath = path;

            SQLite::Database& db = *(_impl->db);
            db.exec("PRAGMA synchronous = OFF;");
            db.exec("PRAGMA locking_mode = EXCLUSIVE;");
            return true;
        }
    }
    else {
        LogE("DBCommon.open():数据库文件不存在path=%s !", path.c_str());
    }
    LogE("DBCommon.open():打开数据库失败!");
    return false;
}

bool DBCommon::openRW(const std::string& path, bool isKey)
{
    if (_impl->db != nullptr) {
        delete _impl->db;
    }
    //如果文件存在
    if (fs::is_regular_file(path)) {
        _impl->db = new SQLite::Database(path, SQLite::OPEN_READWRITE);
        if (isKey) {
            _impl->db->key(this->key); //加密
        }

        if (_impl->db != nullptr) {
            LogI("DBCommon.openRW():打开数据库成功!");
            dbPath = path;

            SQLite::Database& db = *(_impl->db);
            db.exec("PRAGMA synchronous = OFF;");
            db.exec("PRAGMA locking_mode = EXCLUSIVE;");
            return true;
        }
    }
    else {
        LogE("DBCommon.openRW():数据库文件不存在path=%s !", path.c_str());
    }
    LogE("DBCommon.openRW():打开数据库失败!");
    return false;
}

void DBCommon::close()
{
    if (_impl->db != nullptr) {
        delete _impl->db;
        _impl->db = nullptr;
        LogI("DBCommon.close():关闭了数据库!");
        return;
    }
    else {
        LogE("DBCommon.close():数据库已经关闭,还未打开!");
    }
}

bool DBCommon::isOpened()
{
    if (_impl->db != nullptr) {
        return true;
    }
    return false;
}

int DBCommon::createNew(const std::string& path, bool isKey)
{
    if (fs::is_regular_file(path)) { //如果文件存在
        LogE("DBCommon.createNew():数据库文件已经存在了,先确认手动删除之后再操作!");
        return -1;
    }
    _impl->db = new SQLite::Database(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    SQLite::Database& db = *(_impl->db);
    if (isKey) {
        db.key(this->key); //设置加密
    }

    db.exec("PRAGMA synchronous = OFF;");
    db.exec("PRAGMA locking_mode = EXCLUSIVE;");
    //db.exec("PRAGMA cache_size = 4000;");
    //db.exec("PRAGMA temp_store = FILE;");

    //创建一下表
    createTable();

    return 0;
}

void DBCommon::createTable()
{
    SQLite::Database& db = *(_impl->db);
    db.exec("DROP TABLE IF EXISTS common_tbl");
    //通用表,主要就是一个键值+数据的方式来存放各种数据
    db.exec("CREATE TABLE common_tbl (id INTEGER PRIMARY KEY, user_id INTEGER, name TEXT, txt TEXT, data BLOB)");
}

int DBCommon::write(int user_id, const std::string& name, const std::string& txt, const void* data, int len)
{
    if (_impl->db == nullptr) {
        LogE("DBCommon.write():未打开数据库!");
        return -1;
    }

    SQLite::Database& db = *_impl->db;
    //从数据库读信息
    SQLite::Statement queryS(db, (boost::format("SELECT * FROM common_tbl WHERE name='%s'") % name).str());
    int updateCount = 0;
    while (queryS.executeStep()) {
        int id = queryS.getColumn("id").getInt(); //选到主键
        LogD("DBCommon.write():选中了项id=%d,更新它的数据...", id);
        SQLite::Statement queryU(db, (boost::format("UPDATE common_tbl SET user_id=?,name=?,txt=?,data=? WHERE id='%d'") % id).str());
        queryU.bind(1, user_id);
        queryU.bind(2, name);
        queryU.bind(3, txt);
        queryU.bind(4, data, len);
        int nb = queryU.exec();
        if (nb != 1) {
            LogE("DBCommon.write():Update执行失败!");
        }
        updateCount++;
    }
    //如果成功的更新完成了那么直接返回
    if (updateCount > 0) {
        return updateCount;
    }
    //如果没有能够更新那么就插入
    LogD("插入一条新数据%s", name.c_str());
    SQLite::Statement queryI(db, "INSERT INTO common_tbl VALUES (NULL,?,?,?,?)");
    queryI.bind(1, user_id);
    queryI.bind(2, name);
    queryI.bind(3, txt);
    queryI.bind(4, data, len);
    int nb = queryI.exec();
    if (nb != 1) {
        LogE("DBCommon.write():INSERT执行失败!");
    }
    return nb;
}

int DBCommon::readOne(int user_id, const std::string& name, std::string& txt, std::vector<char>& data)
{
    if (_impl->db == nullptr) {
        LogE("DBCommon.readOne():未打开数据库!");
        return -1;
    }

    SQLite::Database& db = *_impl->db;
    SQLite::Statement queryS(db, (boost::format("SELECT * FROM common_tbl WHERE user_id='%d' AND name='%s'") % user_id % name).str());

    while (queryS.executeStep()) {
        int id = queryS.getColumn("id").getInt(); //选到主键
        LogD("DBCommon.readOne():选中了项id=%d", id);
        txt = queryS.getColumn("txt").getString();
        auto column = queryS.getColumn("data");
        data.resize(column.getBytes());
        memcpy(data.data(), column.getBlob(), data.size());
        return 1;
    }
    return 0;
}

std::string DBCommon::readOneText(const std::string& name, int user_id)
{
    if (_impl->db == nullptr) {
        LogE("DBCommon.readOneText():未打开数据库!");
        return "";
    }

    SQLite::Database& db = *_impl->db;
    SQLite::Statement queryS(db, (boost::format("SELECT txt FROM common_tbl WHERE user_id='%d' AND name='%s'") % user_id % name).str());
    while (queryS.executeStep()) {
        return queryS.getColumn(0).getString();
    }
    LogE("DBCommon.readOneText():未能读取到数据项user_id=%d , name=%s!", user_id, name);
    return "";
}

int DBCommon::itemCount()
{
    if (_impl->db == nullptr) {
        LogE("DBCommon.itemCount():未打开数据库!");
        return -1;
    }

    SQLite::Database& db = *_impl->db;
    SQLite::Statement query(db, "SELECT COUNT(*) FROM common_tbl");
    while (query.executeStep()) {
        return query.getColumn(0).getInt(); //得到计数
    }
    return 0;
}
} // namespace dxlib