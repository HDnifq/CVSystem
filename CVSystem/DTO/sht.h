#pragma once

#include "Serialize.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 写给sht的一个模板. </summary>
///
/// <remarks> Dx, 2019/3/25. </remarks>
///-------------------------------------------------------------------------------------------------
class sht
{
  public:
    sht() {}
    ~sht() {}

    std::string md5_value = "shtdx";

    //sht的需求里这两个成员要很小白的并成一个对象
    std::vector<int> index1 = {12, 3};
    std::vector<int> index2 = {4, 5};

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 转换物体到json. </summary>
    ///
    /// <remarks> Dx, 2019/3/21. </remarks>
    ///
    /// <returns> This object as a std::string. </returns>
    ///-------------------------------------------------------------------------------------------------
    std::wstring toJson() const
    {
        using namespace rapidjson;
        using namespace dxlib::json;

        auto doc = JsonHelper::creatEmptyObjectDocW();
        auto& allocator = doc.GetAllocator();

        Serialize::AddMember(doc, L"md5_value", md5_value, allocator);
        ValueW v_index(kObjectType);
        Serialize::AddMember(v_index, L"001", index1, allocator);
        Serialize::AddMember(v_index, L"002", index2, allocator);
        doc.AddMember(L"index", v_index, allocator);
        return JsonHelper::toStr(doc);
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Json转到obj. </summary>
    ///
    /// <remarks> Dx, 2019/3/25. </remarks>
    ///
    /// <param name="json"> [in,out] The JSON. </param>
    ///-------------------------------------------------------------------------------------------------
    void readJson(std::wstring& json)
    {
        using namespace rapidjson;
        auto doc = JsonHelper::creatDocW(json);
        md5_value = JsonHelper::utf16To8(doc[L"md5_value"].GetString());

        dxlib::json::Serialize::GetObj(doc[L"index"][L"001"], index1);
        dxlib::json::Serialize::GetObj(doc[L"index"][L"002"], index2);
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 添加到一个json对象里. </summary>
    ///
    /// <remarks> Dx, 2019/3/21. </remarks>
    ///
    /// <param name="jsonObj"> [in,out] If non-null, the
    ///                        JSON object. </param>
    ///-------------------------------------------------------------------------------------------------
    void AddMember(rapidjson::ValueW& value, const rapidjson::GenericStringRef<wchar_t>& name, const std::wstring& str, rapidjson::MemoryPoolAllocator<>& allocator)
    {

    }
};

} // namespace dxlib
