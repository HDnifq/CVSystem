#include "pch.h"

#include "dlog/dlog.h"
#include "CVSystem/CVSystem.h"
#include "time.h"

#include <opencv2/freetype.hpp>

//用来测试s2ws_ws2s
#pragma execution_character_set("utf-8")
#include "xuexuejson/Serialize.hpp"

#include "CVSystem/Common/base64.h"
#include "Poco/File.h"
#include "Poco/Path.h"

using namespace dxlib;
using namespace std;
using namespace dxlib::cvsystem;

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
#if defined(_WIN32) || defined(_WIN64)
TEST(Common, s2ws_ws2s)
{
    //注意：有的字符是会转换失败的，如"· "
    std::string s = "试试能不能123（）╮(╯▽╰)╭ c++ float截取位数 - 修罗的博客 - CSDN博客 2018 - 7 - 23对于一个double a = 1.234567; 如果我只取小数点后3位，那么我可以这样做： a = floor(a * 1000) / 1000; floor函数的作用是返回一个小于传入参数的最大整数，所以...";
    std::wstring ws = L"试试能不能123（）╮(╯▽╰)╭ c++ float截取位数 - 修罗的博客 - CSDN博客 2018 - 7 - 23对于一个double a = 1.234567; 如果我只取小数点后3位，那么我可以这样做： a = floor(a * 1000) / 1000; floor函数的作用是返回一个小于传入参数的最大整数，所以...";

    //这两个函数目前的实现它不是UTF8对UTF16的,它只是转到本地字符集.
    //std::string s1 = StringHelper::ws2s(ws);
    //std::wstring ws1 = StringHelper::s2ws(s);

    //这两个是utf16和utf8的安全转换
    std::string s1 = xuexue::json::JsonHelper::utf16To8(ws);
    std::wstring ws1 = xuexue::json::JsonHelper::utf8To16(s);

    EXPECT_TRUE(s1 == s);
    EXPECT_TRUE(ws1 == ws);
}
#endif

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

TEST(Common, string_at)
{
    std::string str = "12345678";
    EXPECT_TRUE(str[0] == '1');
    EXPECT_TRUE(str[1] == '2');
}

TEST(Common, base64)
{
    std::vector<uchar> data;
    for (size_t i = 0; i < 1024; i++) {
        data.push_back(i);
    }

    std::string str = base64::encode(data.data(), data.size());
    std::vector<uchar> data2 = base64::decode(str);

    for (size_t i = 0; i < data.size(); i++) {
        ASSERT_TRUE(data[i] == data2[i]);
    }
}

TEST(Common, Font)
{
    Poco::File file("Meiryo-01.ttf");
    if (!file.exists()) {
        //如果字体文件不存在那么就不跑测试了
        return;
    }

    using namespace cv;
    String text = "Funny 中文 ❀ (╯‵□′)╯︵┻━┻";
    int fontHeight = 32;
    int thickness = -1;
    int linestyle = 8;
    Mat img(600, 800, CV_8UC3, Scalar::all(0));
    int baseline = 0;
    cv::Ptr<cv::freetype::FreeType2> ft2;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("Meiryo-01.ttf", 0);
    Size textSize = ft2->getTextSize(text,
                                     fontHeight,
                                     thickness,
                                     &baseline);
    if (thickness > 0) {
        baseline += thickness;
    }
    // center the text
    Point textOrg((img.cols - textSize.width) / 2,
                  (img.rows + textSize.height) / 2);
    // draw the box
    rectangle(img, textOrg + Point(0, baseline),
              textOrg + Point(textSize.width, -textSize.height),
              Scalar(0, 255, 0), 1, 8);
    // ... and the baseline first
    line(img, textOrg + Point(0, thickness),
         textOrg + Point(textSize.width, thickness),
         Scalar(0, 0, 255), 1, 8);
    // then put the text itself
    ft2->putText(img, text, textOrg, fontHeight,
                 Scalar::all(255), thickness, linestyle, true);
}

TEST(Common, Path)
{
    using namespace Poco;
    Path dirRoot = Path("D:\\Work\\ImageCapture\\build\\bin").makeDirectory();
    Path p2 = Path("/index.html");
    Path pfullPath = Poco::Path(dirRoot, p2).makeFile();
    Path pfullPath2 = dirRoot.append(p2).makeFile();
    string ext = p2.getExtension();
}
