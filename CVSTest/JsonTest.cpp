#include "pch.h"
//#pragma execution_character_set("utf-8")

#include "../CVSystem/CVSystem.h"
#include "../CVSystem/DTO/JsonHelper.h"
#include "../CVSystem/DTO/Serialize.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/encodings.h"

using namespace dxlib;
using namespace dxlib::dxjson;
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

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, m);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d;
    d.ParseStream(s);
    ValueW v = d.GetObject();

    //反序列化到Mat
    cv::Mat m2 = Serialize::j2o(v);

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

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, m);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d;
    d.ParseStream(s);
    ValueW v = d.GetObject();

    //反序列化到Mat
    cv::Mat m2 = Serialize::j2o(v);

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

TEST(Json, cvMat_field)
{
    cv::Mat m, m2;
    m = (cv::Mat_<float>(4, 4) << 1, 0, 0, 0,
         0, -1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1);
    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, L"rt4x4", m);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);
    Serialize::j2o(d2, L"rt4x4", m2);
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

TEST(Json, arr)
{
    array<double, 4> arr = {1, 2, 3, 4};

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, L"rotate", arr);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);
    array<double, 4> arr2;

    Serialize::j2o(d2, L"rotate", arr2);

    for (size_t i = 0; i < arr.size(); i++) {
        EXPECT_TRUE(arr[i] == arr2[i]);
    }
}

TEST(Json, Vector3)
{
    Vector3 o1 = {1, 2, 3};
    Vector3 o2;

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, L"rotate", o1);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);

    Serialize::j2o(d2, L"rotate", o2);

    EXPECT_TRUE(o1.x == o2.x);
    EXPECT_TRUE(o1.y == o2.y);
    EXPECT_TRUE(o1.z == o2.z);
}

TEST(Json, Quaternion)
{
    Quaternion o1 = {1, 2, 3, 4};
    Quaternion o2;

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, L"rotate", o1);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);

    Serialize::j2o(d2, L"rotate", o2);

    EXPECT_TRUE(o1.x == o2.x);
    EXPECT_TRUE(o1.y == o2.y);
    EXPECT_TRUE(o1.z == o2.z);
    EXPECT_TRUE(o1.w == o2.w);
}

TEST(Json, v_wstring)
{
    vector<wstring> o1;
    o1.push_back(L"1");
    o1.push_back(L"2");
    o1.push_back(L"2");
    vector<wstring> o2;

    auto doc = JsonHelper::creatEmptyObjectDocW();
    //序列化到doc
    Serialize::o2j(doc, doc, L"rotate", o1);

    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);

    Serialize::j2o(d2, L"rotate", o2);

    for (size_t i = 0; i < o1.size(); i++) {
        EXPECT_TRUE(o1[i] == o2[i]);
    }
}

TEST(Json, creatarraydoc)
{
    vector<wstring> vStr;
    vStr.push_back(L"1");
    vStr.push_back(L"2");
    vStr.push_back(L"2");

    auto doc = JsonHelper::creatArrayDocW(vStr);
    //保存到string
    wstring text = JsonHelper::toStr(doc);

    //从string读取
    StringStreamW s(text.c_str());
    DocumentW d2;
    d2.ParseStream(s);

    vector<int> vi;
    vi.push_back(4);
    vi.push_back(5);
    vi.push_back(6);

    auto doc2 = JsonHelper::creatArrayDocW(vi);
    text = JsonHelper::toStr(doc2);

    //JsonHelper::save("read_write.json", doc); //这个文件是gbk的
    //Document doc2;
    //JsonHelper::read("read_write.json", doc2);
    //EXPECT_TRUE(doc2["a"] == 1);
    //EXPECT_TRUE(doc2["试试中文"] == 2);
}

#pragma region common

//这个读写测试生成的文件内容是GBK的.如果定义了#pragma execution_character_set("utf-8"),那么文件内容是UTF-8
// rapidjson是默认UTF8的
TEST(Json, read_write)
{
    auto doc = JsonHelper::creatEmptyObjectDoc();

    doc.AddMember("a", 1, doc.GetAllocator());
    doc.AddMember("试试中文", 2, doc.GetAllocator());

    JsonHelper::save("read_write.json", doc); //这个文件是gbk的
    Document doc2;
    JsonHelper::read("read_write.json", doc2);
    EXPECT_TRUE(doc2["a"] == 1);
    EXPECT_TRUE(doc2["试试中文"] == 2);
    //remove("read_write.json");
}

TEST(Json, encodings)
{
    // 这个是官网文档说的转换例子,如果没有定义utf8的编译选项,那么这里是GBK的,然后会转换失败
    // http://rapidjson.org/zh-cn/md_doc_encoding_8zh-cn.html
    const char* s = "试试中文"; // UTF-8 string
    StringStream source(s);
    GenericStringBuffer<UTF16<>> target;
    bool hasError = false;
    while (source.Peek() != '\0')
        if (!Transcoder<UTF8<>, UTF16<>>::Transcode(source, target)) {
            hasError = true;
            break;
        }
    if (!hasError) {
        const wchar_t* t = target.GetString();
        std::wstring ws = std::wstring(t);
        // ...
    }
}

TEST(Json, utf8To16)
{
    //这个测试必须要定义下面的才能方便
    //#pragma execution_character_set("utf-8")
    unsigned char s[] = {0xE8, 0xAF, 0x95}; //这三个字节是"试"的UTF8
    std::wstring ws = JsonHelper::utf8To16((char*)s);
    EXPECT_TRUE(ws == L"试");
}

//它只能在UTF8和UTF16之间简单转换
TEST(Json, utf16To8)
{
    const wchar_t* s = L"试试中文"; // UTF-16 string
    string t = JsonHelper::utf16To8(s);
    EXPECT_TRUE(t[0] == (char)0xE8);
    EXPECT_TRUE(t[1] == (char)0xAF);
    EXPECT_TRUE(t[2] == (char)0x95);
    EXPECT_TRUE(t[3] == (char)0xE8);
    EXPECT_TRUE(t[4] == (char)0xAF);
    EXPECT_TRUE(t[5] == (char)0x95);
    EXPECT_TRUE(t[6] == (char)0xE4);
    EXPECT_TRUE(t[7] == (char)0xB8);
    EXPECT_TRUE(t[8] == (char)0xAD);
    EXPECT_TRUE(t[9] == (char)0xE6);
    EXPECT_TRUE(t[10] == (char)0x96);
    EXPECT_TRUE(t[11] == (char)0x87);
}

//一个utf16的doc保存成utf8的文件
TEST(Json, save_utf16_to_utf8)
{
    DocumentW doc;
    doc.SetObject();
    doc.AddMember(L"a", 1, doc.GetAllocator());
    doc.AddMember(L"试试中文", 2, doc.GetAllocator());
    doc.AddMember(L"再试试", 3, doc.GetAllocator());

    JsonHelper::save("tmp.txt", doc, false);
}

//这个库的api封装的很难用,作者没有做进一步封装,例子也很少

//TEST(Json, encodings2)
//{
//}
//
//typedef GenericDocument<UTF16<>> Document_UTF16;
//typedef GenericValue<UTF16<>> Value_UTF16;
//typedef GenericStringStream<UTF16<>> StringStream_UTF16;
//typedef GenericStringBuffer<UTF16<>> StringBuffer_UTF16;
//typedef EncodedInputStream<UTF16LE<>, FileReadStream> EncodedInputStream_UTF16;
//typedef EncodedOutputStream<UTF16LE<>, FileWriteStream> EncodedOutputStream_UTF16;

//TEST(Json, test_encoding_auto)
//{
//    GenericDocument<UTF16LE<>> d0;
//
//    d0.AddMember(L"a", 1, d0.GetAllocator());
//    d0.AddMember(L"试试中文", 2, d0.GetAllocator());
//    d0.AddMember(L"再试试", 3, d0.GetAllocator());
//
//    char buf[256];
//    FILE* fp;
//    remove("json-utf16le.json");
//    fopen_s(&fp, "json-utf16le.json", "wb"); // 非 Windows 平台使用 "w"
//    typedef AutoUTFOutputStream<unsigned, FileWriteStream> OutputStream;
//
//    FileWriteStream ws(fp, buf, sizeof(buf));
//    bool put_bom = true;
//    OutputStream os(ws, UTFType::kUTF16LE, put_bom);
//    Writer<OutputStream, Document::EncodingType, AutoUTF<unsigned>> writer(os);
//    d0.Accept(writer);
//    fclose(fp);
//
//    Document doc;
//
//    // read
//    fopen_s(&fp, "json-utf16le.json", "rb");
//    FileReadStream rs(fp, buf, sizeof(buf));
//    AutoUTFInputStream<unsigned, FileReadStream> is(rs);
//
//    doc.ParseStream<kParseDefaultFlags, AutoUTF<unsigned>>(is);
//
//    fclose(fp);
//
//    // do something...
//    UTFType utf_type = is.GetType();
//    EXPECT_TRUE(utf_type == kUTF16LE);
//    bool has_bom = is.HasBOM();
//    EXPECT_TRUE(has_bom == true);
//
//    doc["name"] = Value("hoge");
//}
#pragma endregion