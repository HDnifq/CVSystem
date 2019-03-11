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
    /// <summary> obj -> json. </summary>
    ///
    /// <remarks> Surface, 2019/3/11. </remarks>
    ///
    /// <param name="doc">   [in,out] The document. </param>
    /// <param name="value"> [in,out] The value. </param>
    /// <param name="m">     A cv::Mat to process. </param>
    ///-------------------------------------------------------------------------------------------------
    static inline void o2j(rapidjson::Document& doc, rapidjson::Value& value, const cv::Mat& m)
    {
        auto& allocator = doc.GetAllocator();
        int channels = m.channels();
        int cols = m.cols;
        int rows = m.rows;

        value.AddMember("type", m.type(), allocator);
        value.AddMember("channels", m.channels(), allocator);
        value.AddMember("cols", cols, allocator);
        value.AddMember("rows", rows, allocator);

        rapidjson::Value data(rapidjson::kArrayType);
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
        value.AddMember("data", data, allocator);
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> json -> obj. </summary>
    ///
    /// <remarks> Dx, 2019/3/11. </remarks>
    ///
    /// <param name="value"> The value. </param>
    ///
    /// <returns> A cv::Mat. </returns>
    ///-------------------------------------------------------------------------------------------------
    static inline cv::Mat j2o(const rapidjson::Value& value)
    {
        int type = value["type"].GetInt();
        int channels = value["channels"].GetInt();
        int cols = value["cols"].GetInt();
        int rows = value["rows"].GetInt();
        const auto& data = value["data"];

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