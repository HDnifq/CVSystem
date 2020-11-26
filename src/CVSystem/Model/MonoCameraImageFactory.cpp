#include "MonoCameraImageFactory.h"
#include "dlog/dlog.h"

namespace dxlib {

std::vector<CameraImage> MonoCameraImageFactory::Create()
{
    std::vector<CameraImage> result;
    result.resize(cameras.size());

    //绑定相机采图设备来源
    for (size_t i = 0; i < cameras.size(); i++) {
        cameras[i]->device = device.get();
    }

    for (size_t i = 0; i < result.size(); i++) {
        result[i].grabStartTime = clock();
    }
    cv::Mat image;
    LogD("MonoCameraImageFactory.Create():开始阻塞采图...");
    if (device->read(image)) //阻塞的读取
    {
        //如果采图成功
        for (size_t i = 0; i < result.size(); i++) {
            result[i].grabEndTime = clock();
            result[i].device = device.get();
            result[i].camera = cameras[i].get();
            result[i].isSuccess = true;

            //第一个相机直接赋值,后面的相机图片是要拷贝一下
            if (i == 0) {
                LogD("MonoCameraImageFactory.Create():当前采图消耗时间 %f ms.", result[i].costTime());
                result[i].image = image;
            }
            else {
                result[i].image = image.clone();
            }
        }
    }
    else {
        //在切换相机属性的时候居然会采图失败
        LogE("MonoCameraImageFactory.Create():采图失败了!!");
        //如果采图失败
        for (size_t i = 0; i < result.size(); i++) {
            result[i].grabEndTime = clock();
            result[i].device = device.get();
            result[i].camera = cameras[i].get();
            result[i].isSuccess = false;
        }
    }
    return result;
}

} // namespace dxlib