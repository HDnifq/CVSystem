#pragma once
#include "JsonHelper.h"

#include <opencv2/opencv.hpp>

namespace dxlib {
namespace dxjson {

class Serialize
{
  public:
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
};

} // namespace Json

} // namespace dxlib