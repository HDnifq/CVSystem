#include "StereoCameraImageFactory.h"
#include "dlog/dlog.h"

namespace dxlib {

StereoCameraImageFactory::StereoCameraImageFactory(const pCameraDevice& device, const std::array<pCamera, 2>& stereoCamera)
{
    //给基类成员赋值
    this->device = device;
    this->stereoCamera = stereoCamera;

    //这个数据结构实际上没有用了,只是为了兼容ICameraImageFactory中的成员
    this->cameras.push_back(stereoCamera[0]);
    this->cameras.push_back(stereoCamera[1]);

    this->id = this->device->id;
    if (this->id < 0) {
        LogW("StereoCameraImageFactory.构造():目前未给device分配id,可能是错误...");
    }
}

std::vector<CameraImage> StereoCameraImageFactory::Create()
{
    std::vector<CameraImage> result;
    result.resize(cameras.size());

    //绑定相机采图设备来源
    for (size_t i = 0; i < cameras.size(); i++) {
        cameras[i]->device = device.get();
    }

    if (cameras.size() < 2 || cameras.size() % 2 != 0) {
        LogE("StereoCameraImageFactory.Create:当前的相机个数不正确,传入cameras的个数为%zu", cameras.size());
        return result;
    }

    for (size_t i = 0; i < result.size(); i++) {
        result[i].grabStartTime = clock();
    }

    cv::Mat image;
    if (device->read(image)) //阻塞的读取
    {
        //如果采图成功
        for (size_t i = 0; i < result.size() / 2; i++) {
            CameraImage& camImageL = result[2 * i];
            CameraImage& camImageR = result[2 * i + 1];

            camImageL.type = CameraImage::ImageType::StereoL;
            camImageL.grabEndTime = clock();
            camImageL.device = device.get();
            camImageL.camera = cameras[2 * i].get();
            camImageL.isSuccess = true;

            camImageR.type = CameraImage::ImageType::StereoR;
            camImageR.grabEndTime = clock();
            camImageR.device = device.get();
            camImageR.camera = cameras[2 * i + 1].get();
            camImageR.isSuccess = true;

            int w = image.cols;
            int h = image.rows;

            //第一个相机直接赋值,后面的相机图片是要拷贝一下
            if (i == 0) {
                LogD("StereoCameraImageFactory.Create():当前采图消耗时间 %f ms.", result[i].costTime());

                camImageL.image = cv::Mat(image, cv::Rect(0, 0, w / 2, h));     //等于图的左半边
                camImageR.image = cv::Mat(image, cv::Rect(w / 2, 0, w / 2, h)); //等于图的右半边
            }
            else {
                camImageL.image = result[0].image.clone();
                camImageR.image = result[1].image.clone();
            }
        }
    }
    else {
        //如果采图失败
        LogE("StereoCameraImageFactory.Create():硬件采图失败!!");

        //这里很可能是硬件设备挂了
        isDeviceError = true;

        for (size_t i = 0; i < result.size() / 2; i++) {
            CameraImage& camImageL = result[2 * i];
            CameraImage& camImageR = result[2 * i + 1];

            camImageL.grabEndTime = clock();
            camImageL.device = device.get();
            camImageL.camera = cameras[2 * i].get();
            camImageL.isSuccess = false;

            camImageR.grabEndTime = clock();
            camImageR.device = device.get();
            camImageR.camera = cameras[2 * i + 1].get();
            camImageR.isSuccess = false;
        }
    }
    return result;
}

} // namespace dxlib
