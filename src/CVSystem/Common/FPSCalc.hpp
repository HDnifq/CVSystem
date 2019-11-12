#pragma once

#include <time.h>

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 计算FPS的小辅助类. </summary>
///
/// <remarks> Dx, 2018/11/14. </remarks>
///-------------------------------------------------------------------------------------------------
class FPSCalc
{
  public:
    FPSCalc() {}
    ~FPSCalc() {}

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 重置记录. </summary>
    ///
    /// <remarks> Dx, 2018/11/14. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void reset()
    {
        _lastTime = clock();
        _lastfnumber = 0;
        fps = 0;
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 使用当前帧号来计算FPS. </summary>
    ///
    /// <remarks> Dx, 2018/11/14. </remarks>
    ///
    /// <param name="fnumber"> 当前帧数. </param>
    ///
    /// <returns> A float. </returns>
    ///-------------------------------------------------------------------------------------------------
    float update(long long fnumber)
    {
        if (fnumber < _lastfnumber) { //如果后来送进来的帧序号比上次记录的小，那么可能是忘了重置了
            _lastfnumber = 0;
        }

        clock_t now = clock();
        float costTime = (float)(now - _lastTime) / CLOCKS_PER_SEC;

        //这里认为消耗时间至少应该小于1微秒（防止除零）
        if (costTime < 1E-03f) {
            costTime = 1E-03f;
        }

        if (costTime > 1.0f) {                                                   //如果经过了1秒
            fps = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位

            _lastTime = now;        //记录现在的最近一次的时间
            _lastfnumber = fnumber; //记录现在最近的帧数
        }
        else if (costTime > 0.5f) {                                              //在0.5秒之后可以开始计算了
            fps = ((int)(((fnumber - _lastfnumber) / costTime) * 100)) / 100.0f; //这里把float截断后两位
        }
        return fps;
    }

    /// <summary> fps结果. </summary>
    float fps = 0;

  private:
    clock_t _lastTime = 0;
    long long _lastfnumber = 0;
};

} // namespace dxlib
