#pragma once

#include "../Proc/MultiCamera.h"
#include "CameraManger.h"

namespace dxlib {

///-------------------------------------------------------------------------------------------------
/// <summary> 仅仅只显示相机画面的proc,需要相机载入过参数. </summary>
///
/// <remarks> Dx, 2018/11/19. </remarks>
///-------------------------------------------------------------------------------------------------
class CamImageProc : public FrameProc
{
  public:
    CamImageProc()
    {
    }
    ~CamImageProc()
    {
    }

    bool isRemap = false;

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 处理函数接口. </summary>
    ///
    /// <remarks> Xian Dai, 2018/10/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void process(pCameraImage camImage, int& key) override
    {
        //显示所有校正过的相机图像
        for (auto& item : camImage->vImage) {

            //应该只有采图成功的相机才处理
            if (!item.isSuccess) {
                continue;
            }

            cv::Mat out;
            if (isRemap) {
                cv::remap(item.image, out, item.camera->rmap1, item.camera->rmap2, cv::INTER_LINEAR);
            }
            else {
                out = item.image;
            }
            cv::namedWindow("CamImage" + std::to_string(item.camera->camIndex), cv::WINDOW_AUTOSIZE);
            cv::imshow("CamImage" + std::to_string(item.camera->camIndex), out);
        }

        try {
            key = cv::waitKey(1); //一定要加waitKey无法显示图片
        }
        catch (const std::exception& e) {
        }

        if (key == 'r') {
            isRemap = !isRemap;
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 被使能的时候的响应. </summary>
    ///
    /// <remarks> Xian Dai, 2018/10/18. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void onEnable() override
    {
        for (auto& kvp : CameraManger::GetInst()->camMap) {
            kvp.second->setProp(CV_CAP_PROP_BRIGHTNESS, 64);
        }
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 不使能的时候的响应. </summary>
    ///
    /// <remarks> Xian Dai, 2018/9/28. </remarks>
    ///-------------------------------------------------------------------------------------------------
    void onDisable() override
    {
        cv::destroyAllWindows();
    }

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 返回自己的标示名. </summary>
    ///
    /// <remarks> Dx, 2019/3/19. </remarks>
    ///
    /// <returns> 标示名. </returns>
    ///-------------------------------------------------------------------------------------------------
    const char* name() override
    {
        return "CamImageProc";
    }

  private:
};

} // namespace dxlib