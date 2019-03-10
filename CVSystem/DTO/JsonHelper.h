#pragma once

#include <opencv2/opencv.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 转换到Json的公共方法.
///           使用Json::o2j(xxx).serialize()可以直接转到string.</summary>
///
/// <remarks> Dx, 2019/1/23. </remarks>
///-------------------------------------------------------------------------------------------------
class JsonHelper
{
  public:
    ///-------------------------------------------------------------------------------------------------
    /// <summary> Creat empty object document. </summary>
    ///
    /// <remarks> Surface, 2019/3/11. </remarks>
    ///
    /// <returns> A rapidjson::Document. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline rapidjson::Document creatEmptyObjectDoc()
    {
        rapidjson::Document document;
        document.SetObject();
        return document;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> mat转json. </summary>
    ///
    /// <remarks> Surface, 2019/3/11. </remarks>
    ///
    /// <param name="doc">   [in,out] The document. </param>
    /// <param name="value"> [in,out] The value. </param>
    /// <param name="m">     [in,out] A cv::Mat to process. </param>
    ///-------------------------------------------------------------------------------------------------
    static inline void o2j(rapidjson::Document& doc, rapidjson::Value& value, cv::Mat& m)
    {
        auto& allocator = doc.GetAllocator();
        int channels = m.channels();
        int cols = m.cols;
        int rows = m.rows;

        std::vector<double> arr;
        double* p = m.ptr<double>();
        for (size_t i = 0; i < cols * rows * channels; i++) {
            arr.push_back(p[i]);
        }

        value.AddMember("type", m.type(), allocator);
        value.AddMember("channels", m.channels(), allocator);
        value.AddMember("cols", m.cols, allocator);
        value.AddMember("rows", m.rows, allocator);

        rapidjson::Value data(rapidjson::kArrayType);
        for (size_t i = 0; i < cols * rows * channels; i++) {
            data.PushBack(p[i], allocator);
        }
        value.AddMember("data", data, allocator);
    }

    static inline cv::Mat j2o(rapidjson::Value& value)
    {
        int type = value["type"].GetInt();
        int channels = value["channels"].GetInt();
        int cols = value["cols"].GetInt();
        int rows = value["rows"].GetInt();
        value["data"].GetArray();
        return cv::Mat(rows, cols, type);
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> Converts a doc to a string. </summary>
    ///
    /// <remarks> Surface, 2019/3/11. </remarks>
    ///
    /// <param name="doc"> [in] The document. </param>
    ///
    /// <returns> Doc as a std::string. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline std::string toStr(rapidjson::Document& doc)
    {
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        doc.Accept(writer); // Accept() traverses the DOM and generates Handler events.
        return std::string(sb.GetString());
    }
};
} // namespace dxlib