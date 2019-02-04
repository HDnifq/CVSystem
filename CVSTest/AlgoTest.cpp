#include "pch.h"

#include "../CVSystem/CVSystem.h"
#include "../CVSystem/AlGO.hpp"

using namespace dxlib;
using namespace std;

//指针使用测试
TEST(ALGO, findIndex)
{
    vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    EXPECT_TRUE(ALGO::findIndex(vec, 3) == 2);

    vector<int> vToFind = { 3, 4, 5 };
    vector<int> res = ALGO::findIndex(vec, vToFind);
    EXPECT_TRUE(res[0] == 2);
    EXPECT_TRUE(res[1] == 3);
    EXPECT_TRUE(res[2] == 4);
    EXPECT_TRUE(res.size() == 3);
}

TEST(ALGO, selectVector)
{
    vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    vector<int> indexs = { 3, 4, 5 };
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

    vector<int> keys = { 1, 2 };
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