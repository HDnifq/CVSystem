#include "pch.h"

#include "CVSystem/CVSystem.h"
#include "CVSystem/ALGO.hpp"
#include "dlog/dlog.h"

using namespace dxlib;
using namespace std;

//指针使用测试
TEST(ALGO, findIndex)
{
    dlog_set_is_chcp65001(true);

    vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_TRUE(ALGO::findIndex(vec, 3) == 2);

    vector<int> vToFind = {3, 4, 5};
    vector<int> res = ALGO::findIndex(vec, vToFind);
    EXPECT_TRUE(res[0] == 2);
    EXPECT_TRUE(res[1] == 3);
    EXPECT_TRUE(res[2] == 4);
    EXPECT_TRUE(res.size() == 3);
}

TEST(ALGO, selectVector)
{
    vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> indexs = {3, 4, 5};
    vector<int> res = ALGO::select(vec, indexs);

    EXPECT_TRUE(res[0] == 4);
    EXPECT_TRUE(res[1] == 5);
    EXPECT_TRUE(res[2] == 6);
}

TEST(ALGO, selectMap)
{
    map<int, string> m;
    m[0] = "123";
    m[1] = "123";
    m[2] = "123";

    vector<int> keys = {1, 2};
    vector<string> res = ALGO::select(m, keys);

    EXPECT_TRUE(res.size() == 2);
}

TEST(ALGO, mapKeysValues)
{
    map<int, string> m;
    m[0] = "123";
    m[1] = "123";
    m[2] = "123";

    vector<int> keys;
    ALGO::mapKeys(m, keys);
    EXPECT_TRUE(keys.size() == 3);

    vector<string> values;
    ALGO::mapValues(m, values);
    EXPECT_TRUE(values.size() == 3);
    //EXPECT_TRUE(res[2] == 6);
}

TEST(ALGO, findMap)
{
    map<int, string> m;
    m[0] = "123";
    m[1] = "123";
    m[2] = "123";

    map<int, int> m2;
    m2[0] = 123;
    m2[1] = 345;
    m2[2] = 456;
    auto itr = ALGO::find(m2, 345);

    EXPECT_TRUE(itr->first == 1);
}

TEST(ALGO, copy1)
{
    vector<int> a = {1, 2, 3, 4};
    vector<int> b;

    ALGO::clone(a, b);
    //EXPECT_TRUE(b.size() == 4);
}

TEST(ALGO, copy2)
{
    vector<vector<int>> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}};
    vector<vector<int>> b;

    ALGO::clone(a, b);
    EXPECT_TRUE(b.size() == 4);
}

class TestClass
{
  public:
    TestClass(string a, string b, string c) : a(a),
                                              b(b),
                                              c(c)
    {
    }
    string a;
    string b;
    string c;
};

//测试一下reserve来避免引用&的错误
TEST(ALGO, vectorReserveTest)
{
    vector<TestClass> v;
    v.reserve(2);
    v.push_back({"x", "x", "x"});
    TestClass& v0 = v[0]; //当使用这个引用的时候,如果数组发生了大小改变,那么这个引用就会错误
    v0.a = "1";
    EXPECT_TRUE(v0.a == "1") << "v0.a=" << v0.a.c_str(); //这里能通过
    for (size_t i = 0; i < 1; i++) {
        v.push_back({"x", "x", "x"});
    }
    EXPECT_TRUE(v[0].a == "1");
    EXPECT_TRUE(v0.a == "1") << "v0.a=" << v0.a.c_str();
}

TEST(ALGO, vecAppend)
{
    int buf[1024];
    for (size_t i = 0; i < 1024; i++) {
        buf[i] = i;
    }
    std::vector<int> vec;
    for (size_t i = 0; i < 512; i++) {
        ALGO::append(vec, buf, 1024);
    }
    EXPECT_TRUE(vec.size() == 1024 * 512);
    for (size_t i = 0; i < 512; i++) {
        for (size_t j = 0; j < 1024; j++) {
            ASSERT_TRUE(vec[i * 1024 + j] == j);
        }
    }
}
