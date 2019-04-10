#include "pch.h"

#include "../CVSystem/CVSystem.h"
#include "time.h"

using namespace dxlib;
using namespace std;

class TestClass
{
  public:
    TestClass(std::string* aStr)
    {
        str = aStr;
    }
    ~TestClass()
    {
        str->append("执行了析构");
    }
    int i;
    std::string* str;

  private:
};
typedef std::shared_ptr<TestClass> pTestClass;

//宽字符的转换测试
TEST(Common, s2ws_ws2s)
{
    //注意：有的字符是会转换失败的，如"· "
    std::string s = "试试能不能123（）╮(╯▽╰)╭ c++ float截取位数 - 修罗的博客 - CSDN博客 2018 - 7 - 23对于一个double a = 1.234567; 如果我只取小数点后3位，那么我可以这样做： a = floor(a * 1000) / 1000; floor函数的作用是返回一个小于传入参数的最大整数，所以...";
    std::wstring ws = L"试试能不能123（）╮(╯▽╰)╭ c++ float截取位数 - 修罗的博客 - CSDN博客 2018 - 7 - 23对于一个double a = 1.234567; 如果我只取小数点后3位，那么我可以这样做： a = floor(a * 1000) / 1000; floor函数的作用是返回一个小于传入参数的最大整数，所以...";

    std::string s1 = StringHelper::ws2s(ws);
    std::wstring ws1 = StringHelper::s2ws(s);

    EXPECT_TRUE(s1 == s);
    EXPECT_TRUE(ws1 == ws);
}

//指针使用测试
TEST(Common, shared_ptr)
{
    EXPECT_TRUE(pTestClass(NULL) == NULL);
    EXPECT_TRUE(pTestClass(NULL) == nullptr);

    //std::string msg;
    //std::vector<pTestClass> vec;
    //vec.push_back(pTestClass(new TestClass(&msg)));
    //vec.clear(); //这里testClass就执行了析构
    //EXPECT_TRUE(msg.length() > 0);
}

TestClass func(std::string* aStr)
{
    TestClass tc(aStr);
    return tc;
}

//测试对象作为参数传递是否会拷贝一次
TEST(Common, returnTest)
{
    //std::string msg;
    //TestClass tc = func(&msg); //这里赋值执行了拷贝构造函数
    //EXPECT_TRUE(msg.length() > 0);
}