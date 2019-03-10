#include "pch.h"
#include "../CVSystem/CVSystem.h"
#include "../CVSystem/DTO/JsonHelper.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

using namespace dxlib;
using namespace std;
using namespace rapidjson;

TEST(Json, rapidjson0)
{
    cv::Mat m;
    m = (cv::Mat_<double>(4, 4) << 1, 0, 0, 0,
         0, -1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1);

    auto doc = JsonHelper::creatEmptyObjectDoc();
    JsonHelper::o2j(doc, doc, m);
    string text = JsonHelper::toStr(doc);

    StringStream s(text.c_str());
    Document d;
    d.ParseStream(s);
    Value v = d.GetObject();

    int type = v["type"].GetInt();
    int channels = v["channels"].GetInt();
    int cols = v["cols"].GetInt();
    int rows = v["rows"].GetInt();

    EXPECT_TRUE(cols == 4);
    EXPECT_TRUE(rows == 4);
    EXPECT_TRUE(type == m.type());
    JsonHelper::j2o(v);
}