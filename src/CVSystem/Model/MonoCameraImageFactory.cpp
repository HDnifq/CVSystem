#include "MonoCameraImageFactory.h"

namespace dxlib {

std::vector<CameraImage> MonoCameraImageFactory::Create()
{
    std::vector<CameraImage> result;
    result.resize(cameras.size());

    //绑定相机采图设备来源
    for (size_t i = 0; i < cameras.size(); i++) {
        cameras[i]->device = device;
    }

    for (size_t i = 0; i < result.size(); i++) {
        result[i].grabStartTime = clock();
    }
    cv::Mat image;
    if (device->read(image)) //阻塞的读取
    {
        //如果采图成功
        for (size_t i = 0; i < result.size(); i++) {
            result[i].grabEndTime = clock();
            result[i].device = device;
            result[i].camera = cameras[i];
            result[i].isSuccess = true;

            //第一个相机直接赋值,后面的相机图片是要拷贝一下
            if (i == 0) {
                result[i].image = image;
            }
            else {
                result[i].image = image.clone();
            }
        }
    }
    else {
        //如果采图失败
        for (size_t i = 0; i < result.size(); i++) {
            result[i].grabEndTime = clock();
            result[i].device = device;
            result[i].camera = cameras[i];
            result[i].isSuccess = false;
        }
    }
}

} // namespace dxlib