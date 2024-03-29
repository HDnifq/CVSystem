﻿#pragma once
#include <vector>
#include <array>
#include <map>
//#include <boost/range/adaptor/map.hpp>
//#include <boost/range/algorithm/copy.hpp>
//#include <boost/assign.hpp>
#include <algorithm>
#include "string.h"

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
    template <typename T>
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
    template <typename T>
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
                        foundCount++;                    //计数加一
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
    /// <param name="vec">    待选的整个数组. </param>
    /// <param name="indexs"> 使用这一组index来选择. </param>
    ///
    /// <returns> 选择出的几项组成的新数组; </returns>
    ///-------------------------------------------------------------------------------------------------
    template <typename T>
    static std::vector<T> select(const std::vector<T>& vec, const std::vector<int> indexs)
    {
        std::vector<T> result;
        for (size_t i = 0; i < indexs.size(); i++) {
            if (indexs[i] < vec.size() && indexs[i] > 0)
                result.push_back(vec[indexs[i]]);
        }
        return result;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 从map里按一组key选出几项来组成一个新的数组. </summary>
    ///
    /// <remarks> Surface, 2019/2/4. </remarks>
    ///
    /// <typeparam name="K"> Generic type parameter. </typeparam>
    /// <typeparam name="V"> Generic type parameter. </typeparam>
    /// <param name="m">    待选的整个map. </param>
    /// <param name="keys"> 使用这一组index来选择. </param>
    ///
    /// <returns> 选择出的几项组成的新数组; </returns>
    ///-------------------------------------------------------------------------------------------------
    template <typename K, typename V>
    static std::vector<V> select(const std::map<K, V>& m, const std::vector<K>& keys)
    {
        std::vector<V> result; //本来有一个pushback了拷贝了一次,后面不差再拷贝一次了,直接返回算了= =
        for (size_t i = 0; i < keys.size(); i++) {
            //如果map里存在这个key值.
            if (m.find(keys[i]) != m.end()) {
                result.push_back(m.at(keys[i]));
            }
        }
        return result;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到map的所有Key. </summary>
    ///
    /// <remarks> Surface, 2019/2/4. </remarks>
    ///
    /// <typeparam name="K"> map的key类型. </typeparam>
    /// <typeparam name="V"> map的value类型. </typeparam>
    /// <param name="m">    map. </param>
    /// <param name="keys"> [out] 所有key. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename K, typename V>
    static void mapKeys(const std::map<K, V>& m, std::vector<K>& keys)
    {
        //std::copy(m | std::adaptors::map_keys, std::back_inserter(keys));
        //std::map<K, V>::const_iterator iter = m.cbegin();
        auto iter = m.cbegin();
        keys.clear();
        while (iter != m.cend()) {
            keys.push_back(iter->first);
            iter++;
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 得到map的所有Key. </summary>
    ///
    /// <remarks> Surface, 2019/2/4. </remarks>
    ///
    /// <typeparam name="K"> map的key类型. </typeparam>
    /// <typeparam name="V"> map的value类型. </typeparam>
    /// <param name="m">     map. </param>
    /// <param name="keys"> [out] 所有value. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename K, typename V>
    static void mapValues(const std::map<K, V>& m, std::vector<V>& values)
    {
        //std::copy(m | std::adaptors::map_values, std::back_inserter(values));
        auto iter = m.cbegin();
        values.clear();
        while (iter != m.cend()) {
            values.push_back(iter->second);
            iter++;
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 是否包含键值key. </summary>
    ///
    /// <remarks> Dx, 2019/2/5. </remarks>
    ///
    /// <typeparam name="K"> map的key类型. </typeparam>
    /// <typeparam name="V"> map的value类型. </typeparam>
    /// <param name="m">   map. </param>
    /// <param name="key"> The key. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename K, typename V>
    static bool isContainKey(const std::map<K, V>& m, const K& key)
    {
        if (m.find(key) != m.end()) {
            return true;
        }
        else {
            return false;
        }
        //也可以使用m.count(key)判断是0还是1
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 根据一个value在map里寻找第一个. </summary>
    ///
    /// <remarks> Dx, 2019/2/5. </remarks>
    ///
    /// <typeparam name="K"> map的key类型. </typeparam>
    /// <typeparam name="V"> map的value类型. </typeparam>
    /// <param name="m">   map. </param>
    /// <param name="val"> The value. </param>
    ///
    /// <returns> 返回值使用了auto,因为搞不清楚返回值类型,模板嵌套了模板. </returns>
    ///-------------------------------------------------------------------------------------------------
    template <typename K, typename V>
    static auto find(const std::map<K, V>& m, const V val)
    {
        return std::find_if(
            m.begin(),
            m.end(),
            [val](const auto& itr) { return itr.second == val; });
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 如果V类型可以从T类型构造,那么就可以执行这样的拷贝(添加). </summary>
    ///
    /// <remarks> Surface, 2019/3/16. </remarks>
    ///
    /// <typeparam name="T"> 拷贝源的类型. </typeparam>
    /// <typeparam name="V"> 拷贝目标的类型. </typeparam>
    /// <param name="src"> 源. </param>
    /// <param name="dst"> [out] 目标结果. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename T, typename V>
    static void clone(const std::vector<std::vector<T>>& src, std::vector<std::vector<V>>& dst)
    {
        for (size_t i = 0; i < src.size(); i++) {
            dst.push_back(clone<T, V>(src[i]));
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 如果V类型可以从T类型构造,那么就可以执行这样的拷贝(添加). </summary>
    ///
    /// <remarks> Dx, 2019/3/13. </remarks>
    ///
    /// <typeparam name="T"> 拷贝源的类型. </typeparam>
    /// <typeparam name="V"> 拷贝目标的类型. </typeparam>
    /// <param name="src"> 输入的数据源. </param>
    ///
    /// <returns> 拷贝结果. </returns>
    ///-------------------------------------------------------------------------------------------------
    template <typename T, typename V>
    static std::vector<V> clone(const std::vector<T>& src)
    {
        std::vector<V> dst;
        for (size_t i = 0; i < src.size(); i++) {
            dst.push_back({src[i]});
        }
        return dst;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 如果V类型可以从T类型构造,那么就可以执行这样的拷贝(添加). </summary>
    ///
    /// <remarks> Dx, 2019/3/13. </remarks>
    ///
    /// <typeparam name="T"> 拷贝源的类型. </typeparam>
    /// <typeparam name="V"> 拷贝目标的类型. </typeparam>
    /// <param name="src"> 输入的数据源. </param>
    /// <param name="dst"> [out] 拷贝结果. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename T, typename V>
    static void clone(const std::vector<T>& src, std::vector<V>& dst)
    {
        for (size_t i = 0; i < src.size(); i++) {
            dst.push_back({src[i]});
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 将一段数组内容(基本类型)添加到vector容器的末尾. </summary>
    ///
    /// <remarks> Surface, 2019/4/2. </remarks>
    ///
    /// <param name="vec">      [in,out] vector容器. </param>
    /// <param name="buf">      数组. </param>
    /// <param name="buf_size"> 拷贝长度. </param>
    ///-------------------------------------------------------------------------------------------------
    template <typename T>
    static void append(std::vector<T>& vec, const T* buf, size_t buf_size)
    {
        size_t size = vec.size();
        if (vec.capacity() < size + buf_size) {
            vec.reserve((vec.capacity() + buf_size) * 2); //扩容到一个安全容量
        }
        vec.resize(size + buf_size);
#if defined(_WIN32) || defined(_WIN64)
        memcpy_s(&vec[size], buf_size * sizeof(T), buf, buf_size * sizeof(T)); //拷贝数组到末尾
#endif
        memcpy(&vec[size], buf, buf_size * sizeof(T)); //拷贝数组到末尾
    }
};
} // namespace dxlib