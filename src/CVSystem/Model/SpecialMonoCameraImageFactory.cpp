#include "SpecialMonoCameraImageFactory.h"
#include "dlog/dlog.h"

namespace dxlib {

SpecialMonoCameraImageFactory::SpecialMonoCameraImageFactory(const pCameraDevice& device, const std::vector<pCamera>& cameras)
{
    //给基类成员赋值
    this->device = device;
    this->cameras = cameras;

    this->id = this->device->id;
    if (this->id < 0) {
        LogW("MonoCameraImageFactory.构造():目前未给device分配id,可能是错误...");
    }
}

SpecialMonoCameraImageFactory::SpecialMonoCameraImageFactory(const pCameraDevice& device, const pCamera& cameras)
{
    //给基类成员赋值
    this->device = device;
    this->cameras.push_back(cameras);

    this->id = this->device->id;
    if (this->id < 0) {
        LogW("MonoCameraImageFactory.构造():目前未给device分配id,可能是错误...");
    }
}

std::vector<CameraImage> SpecialMonoCameraImageFactory::Create()
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
    if (device->read(image)) //阻塞的读取
    {
        //如果采图成功
        for (size_t i = 0; i < result.size(); i++) {
            result[i].grabEndTime = clock();
            result[i].device = device.get();
            result[i].camera = cameras[i].get();
            result[i].isSuccess = true;

            int w = image.cols;
            int h = image.rows;

            //第一个相机直接赋值,后面的相机图片是要拷贝一下
            if (i == 0) {
                LogD("MonoCameraImageFactory.Create():当前采图消耗时间 %f ms.", result[i].costTime());

                if (mode == Mode::HalfLeft)
                    result[i].image = cv::Mat(image, cv::Rect(0, 0, w / 2, h)); //等于图的左半边
                else if (mode == Mode::HalfRight)
                    result[i].image = cv::Mat(image, cv::Rect(w / 2, 0, w / 2, h)); //等于图的右半边
                else
                    result[i].image = image;
            }
            else {
                result[i].image = result[0].image.clone();
            }
        }
    }
    else {
        //在切换相机属性的时候居然会采图失败
        LogE("MonoCameraImageFactory.Create():采图失败了!!");
        //这里很可能是硬件设备挂了
        isDeviceError = true;
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
