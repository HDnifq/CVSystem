#pragma once
#include "Camera.h"

namespace dxlib {

    ///-------------------------------------------------------------------------------------------------
    /// <summary> 所有相机的参数，单例. </summary>
    ///
    /// <remarks> Dx, 2018/11/8. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class CameraManger
    {
    public:
        CameraManger();
        ~CameraManger();

        static CameraManger* GetInst()
        {
            if (m_pInstance == NULL)
                m_pInstance = new CameraManger();
            return m_pInstance;
        }

        /// <summary> 所有相机的map，以camIndex为key. </summary>
        std::map<int, pCamera> camMap;

        /// <summary> 立体相机对. </summary>
        std::vector<pStereoCamera> vStereo;

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 清空记录. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///-------------------------------------------------------------------------------------------------
        void clear();

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 添加一个相机，添加注意camIndex搞对. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///
        /// <param name="cp">              要添加的相机. </param>
        /// <param name="isVirtualCamera"> (Optional) 是否这是一个虚拟相机. </param>
        ///-------------------------------------------------------------------------------------------------
        void add(const pCamera& cp, bool isVirtualCamera = false);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 从文件里载入参数，这个函数将来应该做更灵活的操作. </summary>
        ///
        /// <remarks> Dx, 2018/11/12. </remarks>
        ///-------------------------------------------------------------------------------------------------
        //void load(std::string filePath);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 得到一个立体相机. </summary>
        ///
        /// <remarks> Dx, 2018/11/29. </remarks>
        ///
        /// <returns> The stereo. </returns>
        ///-------------------------------------------------------------------------------------------------
        pStereoCamera getStereo(pCamera camera);

        ///-------------------------------------------------------------------------------------------------
        /// <summary> 设置某一个相机的属性. </summary>
        ///
        /// <remarks> Dx, 2019/1/13. </remarks>
        ///
        /// <param name="camIndex"> 相机的index. </param>
        /// <param name="CAP_PROP"> The capability property. </param>
        /// <param name="value">    The value. </param>
        ///
        /// <returns> 如果存在这个相机就进行设置并且返回true. </returns>
        ///-------------------------------------------------------------------------------------------------
        bool setProp(int camIndex, int CAP_PROP, double value);

    private:

        /// <summary> 单例. </summary>
        static CameraManger* m_pInstance;

        /// <summary> 初始化一个相机的Map. </summary>
        void initUndistortRectifyMap(pCamera& camera);
    };

}