#include "pch.h"
#include "CVSystem/Data/DBCommon.h"
#include <stdio.h>
#include "dlog/dlog.h"

using namespace dxlib;
using namespace std;

TEST(DBCommon, creatNewDB)
{
    //dlog_set_console_thr(dlog_level::warn);
    //remove("param.db3");

    vector<char> data;
    for (size_t i = 0; i < 4 * 1024 * 1024; i++) {
        data.push_back((char)i);
    }
    //DBCommon::GetInst()->createNew("param.db3");
    DBCommon::GetInst()->createNew(":memory:", true);

    ASSERT_TRUE(DBCommon::GetInst()->isOpened());

    DBCommon::GetInst()->createTable();

    int res = DBCommon::GetInst()->write(0, "dev_id", "1234567", data.data(), data.size());
    ASSERT_TRUE(res == 1);
    res = DBCommon::GetInst()->write(0, "dev_id2", "1234567aaaa", data.data(), data.size());
    ASSERT_TRUE(res == 1);
    res = DBCommon::GetInst()->write(0, "dev_id3", "1234567", data.data(), data.size());
    ASSERT_TRUE(res == 1);
    //修改第三条记录
    res = DBCommon::GetInst()->write(0, "dev_id3", "456", data.data(), data.size());
    ASSERT_TRUE(res == 1);

    //读取第三条记录
    string txt;
    vector<char> data2;
    res = DBCommon::GetInst()->readOne(0, "dev_id3", txt, data2);
    ASSERT_TRUE(res == 1);
    ASSERT_TRUE(txt == "456");
    ASSERT_TRUE(data2.size() == data.size());
    for (size_t i = 0; i < data2.size(); i++) {
        ASSERT_TRUE(data2[i] == data[i]);
    }

    //所有item的计数
    int count = DBCommon::GetInst()->itemCount();
    ASSERT_TRUE(count == 3);

    //使用一个简单读法
    string txt2 = DBCommon::GetInst()->readOneText("dev_id2");
    ASSERT_TRUE(txt2 == "1234567aaaa");

    //读一个不存在的项
    ASSERT_TRUE(DBCommon::GetInst()->readOneText("dd") == "");

    DBCommon::GetInst()->close();
}