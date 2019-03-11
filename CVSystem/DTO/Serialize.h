#pragma once
#include "JsonHelper.h"

#include <opencv2/opencv.hpp>

namespace dxlib {
namespace dxjson {

///-------------------------------------------------------------------------------------------------
/// <summary> A vector 3. </summary>
///
/// <remarks> Xian Dai, 2018/1/23. </remarks>
///-------------------------------------------------------------------------------------------------
struct Vector3
{
    float x;
    float y;
    float z;
};

///-------------------------------------------------------------------------------------------------
/// <summary> A quaternion. </summary>
///
/// <remarks> Xian Dai, 2018/1/23. </remarks>
///-------------------------------------------------------------------------------------------------
struct Quaternion
{
    float x;
    float y;
    float z;
    float w;
};

///-------------------------------------------------------------------------------------------------
/// <summary> A serialize,这里面的方法在使用上,通常是某个对象的一个成员字段. </summary>
///
/// <remarks> Dx, 2019/3/11. </remarks>
///-------------------------------------------------------------------------------------------------
class Serialize
{
  public:
#pragma region cvMat

    //obj -> json
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value, const cv::Mat& m)
    {
        auto& allocator = doc.GetAllocator();
        int channels = m.channels();
        int cols = m.cols;
        int rows = m.rows;

        value.AddMember(L"type", m.type(), allocator);
        value.AddMember(L"channels", m.channels(), allocator);
        value.AddMember(L"cols", cols, allocator);
        value.AddMember(L"rows", rows, allocator);

        rapidjson::ValueW data(rapidjson::kArrayType);
        if (m.type() == CV_32F) {
            const float* p = m.ptr<float>();
            for (size_t i = 0; i < cols * rows * channels; i++)
                data.PushBack(p[i], allocator);
        }
        else if (m.type() == CV_64F) {
            const double* p = m.ptr<double>();
            for (size_t i = 0; i < cols * rows * channels; i++)
                data.PushBack(p[i], allocator);
        }
        value.AddMember(L"data", data, allocator);
    }

    //json -> obj
    static inline cv::Mat j2o(const rapidjson::ValueW& value)
    {
        int type = value[L"type"].GetInt();
        int channels = value[L"channels"].GetInt();
        int cols = value[L"cols"].GetInt();
        int rows = value[L"rows"].GetInt();
        const auto& data = value[L"data"];

        cv::Mat m;
        if (type == CV_32F) {
            m = cv::Mat(rows, cols, CV_32F);
            float* _r = m.ptr<float>();
            for (rapidjson::SizeType i = 0; i < data.Size(); i++) // 使用 SizeType 而不是 size_t
                _r[i] = data[i].GetFloat();
        }
        else if (type == CV_64F) {
            m = cv::Mat(rows, cols, CV_64F);
            double* _r = m.ptr<double>();
            for (rapidjson::SizeType i = 0; i < data.Size(); i++) // 使用 SizeType 而不是 size_t
                _r[i] = data[i].GetDouble();
        }
        return m;
    }
    //obj -> json 成员字段(整个doc,整个对象,字段名,要设置值的字段)
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value,
                           const rapidjson::GenericStringRef<wchar_t>& fieldName,
                           const cv::Mat& m)
    {
        using namespace rapidjson;
        auto& allocator = doc.GetAllocator();
        ValueW jv(kObjectType);
        o2j(doc, jv, m);
        value.AddMember(fieldName, jv, allocator);
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, cv::Mat& m)
    {
        m = j2o(value[fieldName]);
    }

#pragma endregion

#pragma region std_array

    //obj -> json 成员字段(整个doc,整个对象,字段名,要设置值的字段)
    template <size_t _Size>
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value,
                           const rapidjson::GenericStringRef<wchar_t>& fieldName,
                           const std::array<double, _Size>& arr)
    {
        using namespace rapidjson;
        auto& allocator = doc.GetAllocator();
        ValueW data(kArrayType);
        for (size_t i = 0; i < arr.size(); i++)
            data.PushBack(arr[i], allocator);
        value.AddMember(fieldName, data, allocator);
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    template <size_t _Size>
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, std::array<double, _Size>& obj)
    {
        const auto& data = value[fieldName];
        for (unsigned int i = 0; i < data.Size(); i++) {
            obj[i] = data[i].GetDouble();
        }
    }

#pragma endregion

#pragma region Vector3

    //obj -> json 成员字段(整个doc,整个对象,字段名,要设置值的字段)
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value,
                           const rapidjson::GenericStringRef<wchar_t>& fieldName,
                           const Vector3& obj)
    {
        using namespace rapidjson;
        auto& allocator = doc.GetAllocator();
        ValueW jv(kObjectType);
        jv.AddMember(L"x", obj.x, allocator);
        jv.AddMember(L"y", obj.y, allocator);
        jv.AddMember(L"z", obj.z, allocator);
        value.AddMember(fieldName, jv, allocator);
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, Vector3& obj)
    {
        const auto& jv = value[fieldName];
        obj.x = jv[L"x"].GetFloat();
        obj.y = jv[L"y"].GetFloat();
        obj.z = jv[L"z"].GetFloat();
    }

#pragma endregion

#pragma region Quaternion

    //obj -> json 成员字段(整个doc,整个对象,字段名,要设置值的字段)
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value,
                           const rapidjson::GenericStringRef<wchar_t>& fieldName,
                           const Quaternion& obj)
    {
        using namespace rapidjson;
        auto& allocator = doc.GetAllocator();
        ValueW jv(kObjectType);
        jv.AddMember(L"x", obj.x, allocator);
        jv.AddMember(L"y", obj.y, allocator);
        jv.AddMember(L"z", obj.z, allocator);
        jv.AddMember(L"w", obj.w, allocator);
        value.AddMember(fieldName, jv, allocator);
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, Quaternion& obj)
    {
        const auto& jv = value[fieldName];
        obj.x = jv[L"x"].GetFloat();
        obj.y = jv[L"y"].GetFloat();
        obj.z = jv[L"z"].GetFloat();
        obj.w = jv[L"w"].GetFloat();
    }

#pragma endregion

#pragma region vector

    //obj -> json 成员字段(整个doc,整个对象,字段名,要设置值的字段)
    static inline void o2j(rapidjson::DocumentW& doc, rapidjson::ValueW& value,
                           const rapidjson::GenericStringRef<wchar_t>& fieldName,
                           const std::vector<std::wstring>& obj)
    {
        using namespace rapidjson;
        auto& allocator = doc.GetAllocator();
        ValueW jv(kArrayType);
        for (size_t i = 0; i < obj.size(); i++) {
            ValueW item; //string的话必须这样搞一下然后拷贝,它这个库分两种思路,ref的和拷贝的
            item.SetString(obj[i].c_str(), allocator);
            jv.PushBack(item, allocator);
        }
        value.AddMember(fieldName, jv, allocator);
    }

    //obj -> json 整个对象是纯array
    static inline rapidjson::DocumentW o2j(const std::vector<std::wstring>& obj)
    {
        rapidjson::DocumentW doc;
        doc.SetArray();
        auto& allocator = doc.GetAllocator();
        for (size_t i = 0; i < obj.size(); i++) {
            rapidjson::ValueW item;
            item.SetString(obj[i].c_str(), allocator);
            doc.PushBack(item, allocator);
        }
        return doc;
    }

    //obj -> json 整个对象是纯array
    template <typename T>
    static inline rapidjson::DocumentW o2j(const std::vector<T>& obj)
    {
        rapidjson::DocumentW doc;
        doc.SetArray();
        auto& allocator = doc.GetAllocator();
        for (size_t i = 0; i < obj.size(); i++) {
            rapidjson::ValueW item;
            doc.PushBack(obj[i], allocator);
        }
        return doc;
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, std::vector<std::wstring>& obj)
    {
        const auto& jv = value[fieldName];
        obj.clear();
        for (unsigned int i = 0; i < jv.Size(); i++) {
            obj.push_back(jv[i].GetString());
        }
    }

    //json -> obj
    static inline void j2o(const rapidjson::ValueW& value, std::vector<std::wstring>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetString());
        }
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, std::vector<double>& obj)
    {
        const auto& jv = value[fieldName];
        obj.clear();
        for (unsigned int i = 0; i < jv.Size(); i++) {
            obj.push_back(jv[i].GetDouble());
        }
    }

    //json -> obj
    static inline void j2o(const rapidjson::ValueW& value, std::vector<double>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetDouble());
        }
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, std::vector<float>& obj)
    {
        const auto& jv = value[fieldName];
        obj.clear();
        for (unsigned int i = 0; i < jv.Size(); i++) {
            obj.push_back(jv[i].GetFloat());
        }
    }

    //json -> obj
    static inline void j2o(const rapidjson::ValueW& value, std::vector<float>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetFloat());
        }
    }

    //json -> obj 成员字段(整个对象,字段名,要设置值的字段)
    static inline void j2o(const rapidjson::ValueW& value, const wchar_t* fieldName, std::vector<int>& obj)
    {
        const auto& jv = value[fieldName];
        obj.clear();
        for (unsigned int i = 0; i < jv.Size(); i++) {
            obj.push_back(jv[i].GetInt());
        }
    }

    //json -> obj
    static inline void j2o(const rapidjson::ValueW& value, std::vector<int>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetInt());
        }
    }

#pragma endregion
};
} // namespace dxjson
} // namespace dxlib