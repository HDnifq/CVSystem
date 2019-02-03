#include "pch.h"

#include "../CVSystem/CVSystem.h"

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