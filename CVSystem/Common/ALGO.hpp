#pragma once
#include <vector>
#include <array>
#include <map>

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 算法类. </summary>
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
            for (size_t i = 0; i < vec.size(); i++) {
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

            for (size_t i = 0; i < vec.size(); i++) {
                //匹配和vals中的值是否一致
                for (size_t j = 0; j < vals.size(); j++) {
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

    };
}