#pragma once
#include "DTOType.h"
#include <opencv2/opencv.hpp>

#include "JsonHelper.h"

namespace dxlib {
namespace json {

///-------------------------------------------------------------------------------------------------
/// <summary> 使用rapidjson扩展的一些固定序列化/反序列化方法. </summary>
///
/// <remarks> 使用示例:
///           //1. 添加一个成员
///           Serialize::AddMember(value, L"name", object, doc.GetAllocator());
///
///           //2. 从一个value得到一个对象
///           GetObj(value, obj);
///
///           //3. 直接去得到一个对象作为返回值
///           cv::Mat m = GetCvMat(value);
///
///           //4. 直接序列化到doc
///           auto doc = ObjectDoc(m);
///
///           //5. 反序列化到Mat
///           cv::Mat m2 = Serialize::GetCvMat(v);
///
///           Dx, 2019/3/11. </remarks>
///-------------------------------------------------------------------------------------------------
class Serialize
{
  public:
#pragma region std_array

    template <size_t _Size>
    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const std::array<std::wstring, _Size>& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kArrayType);
        for (size_t i = 0; i < obj.size(); i++) {
            ValueW item; //string的话必须这样搞一下然后拷贝,它这个库分两种思路,ref的和拷贝的
            item.SetString(obj[i].c_str(), allocator);
            jv.PushBack(item, allocator);
        }
        value.AddMember(fieldName, jv, allocator);
    }

    template <typename T, size_t _Size>
    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const std::array<T, _Size>& arr, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW data(kArrayType);
        for (size_t i = 0; i < arr.size(); i++)
            data.PushBack(arr[i], allocator);
        value.AddMember(fieldName, data, allocator);
    }

    //->json 整个对象是纯array
    template <typename T, size_t _Size>
    static inline rapidjson::DocumentW ArrayDoc(const std::array<T, _Size>& arr)
    {
        rapidjson::DocumentW doc;
        doc.SetArray();
        auto& allocator = doc.GetAllocator();
        for (size_t i = 0; i < arr.size(); i++)
            doc.PushBack(arr[i], allocator);
        return doc;
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<double, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetDouble();
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<float, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetFloat();
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<int, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetInt();
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<unsigned int, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetUint();
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<bool, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetBool();
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    template <size_t _Size>
    static inline void GetObj(const rapidjson::ValueW& value, std::array<std::wstring, _Size>& obj)
    {
        if (!value.IsArray()) //它应该是一个array
            return;
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj[i] = value[i].GetString();
        }
    }

#pragma endregion

#pragma region vector

    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const std::vector<std::wstring>& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kArrayType);
        for (size_t i = 0; i < obj.size(); i++) {
            ValueW item; //string的话必须这样搞一下然后拷贝,它这个库分两种思路,ref的和拷贝的
            item.SetString(obj[i].c_str(), allocator);
            jv.PushBack(item, allocator);
        }
        value.AddMember(fieldName, jv, allocator);
    }

    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const std::vector<std::string>& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kArrayType);
        for (size_t i = 0; i < obj.size(); i++) {
            ValueW item; //string的话必须这样搞一下然后拷贝,它这个库分两种思路,ref的和拷贝的

            item.SetString(utf8To16(obj[i]).c_str(), allocator);
            jv.PushBack(item, allocator);
        }
        value.AddMember(fieldName, jv, allocator);
    }

    template <typename T>
    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const std::vector<T>& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kArrayType);
        for (size_t i = 0; i < obj.size(); i++) {
            jv.PushBack(obj[i], allocator);
        }
        value.AddMember(fieldName, jv, allocator);
    }

    //->json 整个对象是纯array
    static inline rapidjson::DocumentW ArrayDoc(const std::vector<std::wstring>& obj)
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
    static inline rapidjson::DocumentW ArrayDoc(const std::vector<T>& obj)
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

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, std::vector<std::wstring>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetString());
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, std::vector<double>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetDouble());
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, std::vector<float>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetFloat());
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, std::vector<int>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetInt());
        }
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, std::vector<bool>& obj)
    {
        obj.clear();
        for (unsigned int i = 0; i < value.Size(); i++) {
            obj.push_back(value[i].GetBool());
        }
    }

#pragma endregion

#pragma region string

    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& name, const std::wstring& str, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW valStr;
        valStr.SetString(str.c_str(), allocator); //有val的方式是拷贝一次string的方式 ,
        value.AddMember(name, valStr, allocator);

        //不拷贝的方式是使用StringRef
        // bid.AddMember("adm", rapidjson::StringRef(html_snippet.c_str(),html_snippet.size()), allocator);
    }

    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& name, const std::string& str, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        std::wstring wstr = JsonHelper::utf8To16(str);
        ValueW valStr;
        valStr.SetString(wstr.c_str(), allocator);
        value.AddMember(name, valStr, allocator);
    }

#pragma endregion

#pragma region Vector3

    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const dxlib::dto::Vector3& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kObjectType);
        jv.AddMember(L"x", obj.x, allocator);
        jv.AddMember(L"y", obj.y, allocator);
        jv.AddMember(L"z", obj.z, allocator);
        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, dxlib::dto::Vector3& obj)
    {
        obj.x = value[L"x"].GetFloat();
        obj.y = value[L"y"].GetFloat();
        obj.z = value[L"z"].GetFloat();
    }

    //->obj 带返回值的直接GetObj
    static inline dxlib::dto::Vector3 GetDtoVector3(const rapidjson::ValueW& value)
    {
        dxlib::dto::Vector3 obj;
        obj.x = value[L"x"].GetFloat();
        obj.y = value[L"y"].GetFloat();
        obj.z = value[L"z"].GetFloat();
        return obj;
    }

#pragma endregion

#pragma region Quaternion

    static inline void AddMember(rapidjson::ValueW& value,
                                 const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const dxlib::dto::Quaternion& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kObjectType);
        jv.AddMember(L"x", obj.x, allocator);
        jv.AddMember(L"y", obj.y, allocator);
        jv.AddMember(L"z", obj.z, allocator);
        jv.AddMember(L"w", obj.w, allocator);
        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, dxlib::dto::Quaternion& obj)
    {
        obj.x = value[L"x"].GetFloat();
        obj.y = value[L"y"].GetFloat();
        obj.z = value[L"z"].GetFloat();
        obj.w = value[L"w"].GetFloat();
    }

    //->obj 带返回值的直接GetObj
    static inline dxlib::dto::Quaternion GetDtoQuaternion(const rapidjson::ValueW& value)
    {
        dxlib::dto::Quaternion obj;
        obj.x = value[L"x"].GetFloat();
        obj.y = value[L"y"].GetFloat();
        obj.z = value[L"z"].GetFloat();
        obj.w = value[L"w"].GetFloat();
        return obj;
    }

#pragma endregion

#pragma region cvSize
    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const cv::Size& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        //一个对象带width和height字段
        using namespace rapidjson;
        ValueW jv(kObjectType);

        jv.AddMember(L"width", obj.width, allocator);
        jv.AddMember(L"height", obj.height, allocator);

        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, cv::Size& m)
    {
        m.width = value[L"width"].GetInt();
        m.height = value[L"height"].GetInt();
    }

    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const cv::Size2f& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        //一个对象带width和height字段
        using namespace rapidjson;
        ValueW jv(kObjectType);

        jv.AddMember(L"width", obj.width, allocator);
        jv.AddMember(L"height", obj.height, allocator);

        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, cv::Size2f& m)
    {
        m.width = value[L"width"].GetFloat();
        m.height = value[L"height"].GetFloat();
    }

    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const cv::Size2d& obj, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        //一个对象带width和height字段
        using namespace rapidjson;
        ValueW jv(kObjectType);

        jv.AddMember(L"width", obj.width, allocator);
        jv.AddMember(L"height", obj.height, allocator);

        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, cv::Size2d& m)
    {
        m.width = value[L"width"].GetDouble();
        m.height = value[L"height"].GetDouble();
    }

#pragma endregion

#pragma region cvMat

    //->json 整个对象是一个object
    static inline rapidjson::DocumentW ObjectDoc(const cv::Mat& m)
    {
        rapidjson::DocumentW doc;
        doc.SetObject();
        auto& allocator = doc.GetAllocator();

        int channels = m.channels();
        int cols = m.cols;
        int rows = m.rows;

        doc.AddMember(L"type", m.type(), allocator);
        doc.AddMember(L"channels", m.channels(), allocator);
        doc.AddMember(L"cols", cols, allocator);
        doc.AddMember(L"rows", rows, allocator);

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
        doc.AddMember(L"data", data, allocator);
        return doc;
    }

    static inline void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& fieldName,
                                 const cv::Mat& m, rapidjson::MemoryPoolAllocator<>& allocator)
    {
        using namespace rapidjson;
        ValueW jv(kObjectType);

        int channels = m.channels();
        int cols = m.cols;
        int rows = m.rows;

        jv.AddMember(L"type", m.type(), allocator);
        jv.AddMember(L"channels", m.channels(), allocator);
        jv.AddMember(L"cols", cols, allocator);
        jv.AddMember(L"rows", rows, allocator);

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
        jv.AddMember(L"data", data, allocator);
        value.AddMember(fieldName, jv, allocator);
    }

    //->obj 带返回值的直接GetObj
    static inline cv::Mat GetCvMat(const rapidjson::ValueW& value)
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

    //->obj 不带返回值的,参数作为结果的GetObj
    static inline void GetObj(const rapidjson::ValueW& value, cv::Mat& m)
    {
        int type = value[L"type"].GetInt();
        int channels = value[L"channels"].GetInt();
        int cols = value[L"cols"].GetInt();
        int rows = value[L"rows"].GetInt();
        const auto& data = value[L"data"];

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
    }

#pragma endregion

    ///-------------------------------------------------------------------------------------------------
    /// <summary> UTF8转换成UTF16. </summary>
    ///
    /// <remarks> Dx, 2019/3/11. </remarks>
    ///
    /// <param name="str"> The string. </param>
    ///
    /// <returns> A std::wstring. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline std::wstring utf8To16(const std::string& str)
    {
        using namespace rapidjson;
        StringStream source(str.c_str());
        StringBufferW target;
        bool hasError = false;
        while (source.Peek() != '\0')
            if (!Transcoder<UTF8<>, UTF16<>>::Transcode(source, target)) {
                hasError = true;
                break;
            }
        if (!hasError) {
            return target.GetString();
        }
        return std::wstring();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> UTF16转换成UTF8. </summary>
    ///
    /// <remarks> Dx, 2019/3/11. </remarks>
    ///
    /// <param name="str"> The string. </param>
    ///
    /// <returns> A std::string. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline std::string utf16To8(const std::wstring& str)
    {
        using namespace rapidjson;
        StringStreamW source(str.c_str());
        StringBuffer target;
        bool hasError = false;
        while (source.Peek() != L'\0')
            if (!Transcoder<UTF16<>, UTF8<>>::Transcode(source, target)) {
                hasError = true;
                break;
            }
        if (!hasError) {
            return target.GetString();
        }
        return std::string();
    }
};
} // namespace json
} // namespace dxlib