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

//测试mat的json<->obj
TEST(Json, cvMat_double)
{
    cv::Mat m;
    m = (cv::Mat_<double>(4, 4) << 1, 0, 0, 0,
         0, -1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1);

    auto doc = JsonHelper::creatEmptyObjectDoc();
    //序列化到doc
    JsonHelper::o2j(doc, doc, m);

    //保存到string
    string text = JsonHelper::toStr(doc);

    //从string读取
    StringStream s(text.c_str());
    Document d;
    d.ParseStream(s);
    Value v = d.GetObject();

    //反序列化到Mat
    cv::Mat m2 = JsonHelper::j2o(v);

    //比较两个Mat
    EXPECT_TRUE(m2.cols == m.cols);
    EXPECT_TRUE(m2.rows == m.rows);
    EXPECT_TRUE(m2.type() == m.type());
    double* m_p = m.ptr<double>();
    double* m2_p = m2.ptr<double>();

    for (size_t i = 0; i < m.cols * m.rows; i++) {
        EXPECT_TRUE(m_p[i] == m2_p[i]);
    }
}

//测试mat的json<->obj
TEST(Json, cvMat_float)
{
    cv::Mat m;
    m = (cv::Mat_<float>(4, 4) << 1, 0, 0, 0,
         0, -1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1);

    auto doc = JsonHelper::creatEmptyObjectDoc();
    //序列化到doc
    JsonHelper::o2j(doc, doc, m);

    //保存到string
    string text = JsonHelper::toStr(doc);

    //从string读取
    StringStream s(text.c_str());
    Document d;
    d.ParseStream(s);
    Value v = d.GetObject();

    //反序列化到Mat
    cv::Mat m2 = JsonHelper::j2o(v);

    //比较两个Mat
    EXPECT_TRUE(m2.cols == m.cols);
    EXPECT_TRUE(m2.rows == m.rows);
    EXPECT_TRUE(m2.type() == m.type());
    float* m_p = m.ptr<float>();
    float* m2_p = m2.ptr<float>();

    for (size_t i = 0; i < m.cols * m.rows; i++) {
        EXPECT_TRUE(m_p[i] == m2_p[i]);
    }
}