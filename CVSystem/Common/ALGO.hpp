#pragma once
#include <vector>
#include <array>
#include <map>

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 小算法类. </summary>
    ///
    /// <remarks> Surface, 2019/2/4. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class ALGO
    {
    public:
        ///-------------------------------------------------------------------------------------------------
        /// <summary>
        /// 寻找第一个index,如果没有找到index为-1(暂时就不传引用了，不然常量不太方便).
        /// </summary>
        ///
        /// <remarks> Surface, 2019/2/4. </remarks>
        ///
        /// <typeparam name="T"> Generic type parameter. </typeparam>
        /// <param name="vec"> 要搜寻的vector. </param>
        /// <param name="val"> 搜寻的值. </param>
        ///
        /// <returns> The found index. </returns>
        ///-------------------------------------------------------------------------------------------------
        template<typename T>
        static int findIndex(const std::vector<T>& vec, const T val)
        {
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i] == val) {
                    return i;
                }
            }
            return -1;
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>
        /// 根据一个数组值寻找对应的一组index,如果没有找到index为-1.
        /// </summary>
        ///
        /// <remarks> Surface, 2019/2/4. </remarks>
        ///
        /// <typeparam name="T"> Generic type parameter. </typeparam>
        /// <param name="vec">  要搜寻的vector. </param>
        /// <param name="vals"> 搜寻的值. </param>
        ///
        /// <returns> 寻找到的对应的index. </returns>
        ///-------------------------------------------------------------------------------------------------
        template<typename T>
        static std::vector<int> findIndex(const std::vector<T>& vec, const std::vector<T>& vals)
        {
            std::vector<int> result;
            result.resize(vals.size(), -1);
            int foundCount = 0;

            for (int i = 0; i < vec.size(); i++) {
                //匹配和vals中的值是否一致
                for (int j = 0; j < vals.size(); j++) {
                    if (result[j] == -1) { //只有等于-1才进行寻找
                        if (vec[i] == vals[j]) {
                            result[j] = i;
                            foundCount++;//计数加一
                            if (foundCount == vals.size()) { //如果全部找到了
                                return result;
                            }
                        }
                        continue;
                    }
                }
            }
            return result;
        }

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 从数组里按index选出几项来组成一个新的数组. </summary>
        ///
        /// <remarks> Surface, 2019/2/4. </remarks>
        ///
        /// <typeparam name="T"> Generic type parameter. </typeparam>
        /// <param name="vec">    The vector. </param>
        /// <param name="indexs"> 使用这一组index来选择. </param>
        ///
        /// <returns> 选择出的几项组成的新数组; </returns>
        ///-------------------------------------------------------------------------------------------------
        template<typename T>
        static std::vector<T> select(const std::vector<T>& vec, const std::vector<int> indexs)
        {
            std::vector<T> result;
            for (size_t i = 0; i < indexs.size(); i++) {
                //如果indexs里有错误的index，那么就直接崩溃算了
                result.push_back(vec[indexs[i]]);
            }
            return result;
        }

    };
}