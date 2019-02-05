#pragma once

#include <time.h>

namespace dxlib {
///-------------------------------------------------------------------------------------------------
/// <summary> 类似游戏的冷却时间的判断. </summary>
///
/// <remarks> Dx, 2018/4/10. </remarks>
///-------------------------------------------------------------------------------------------------
class CoolTime
{
  public:
    CoolTime(float cd)
    {
        _lastTime = clock();
        CD = cd;
        isStart = true;
        count = 0;
    }

    CoolTime()
    {
        _lastTime = clock();
        CD = 1; //默认1秒
        isStart = true;
        count = 0;
    }

    ~CoolTime()
    {
    }

    /// <summary> CD时间，以秒计算. </summary>
    float CD;

    /// <summary> 是否开启了冷却计时. </summary>
    bool isStart;

    /// <summary> cd结束的次数. </summary>
    int count;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>
    /// 是否CD结束，可以刷新了。
    /// </summary>
    ///
    /// <remarks> Dx, 2017/9/4. </remarks>
    ///
    /// <returns> True if can flush, false if not. </returns>
    ///-------------------------------------------------------------------------------------------------
    bool isDone()
    {
        clock_t now = clock();
        float costTime = (float)(now - _lastTime) / CLOCKS_PER_SEC;
        if (costTime > CD || count == 0) //如果经过了（CD时间）秒,或者是初始状态
        {
            _lastTime = now; //记录现在的最近一次的时间
            count++;
            return true;
        }
        else
            return false;
    }

  private:
    clock_t _lastTime;
};
} // namespace dxlib