#pragma once

namespace dxlib {
namespace dto {

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

} // namespace dto
} // namespace dxlib