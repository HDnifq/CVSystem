#include "StereoCameraImageFactory.h"
#include "dlog/dlog.h"

namespace dxlib {

std::vector<CameraImage> StereoCameraImageFactory::Create()
{
    std::vector<CameraImage> result;
    result.resize(cameras.size());

    //绑定相机采图设备来源
    for (size_t i = 0; i < cameras.size(); i++) {
        cameras[i]->device = device;
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

            camImageL.grabEndTime = clock();
            camImageL.device = device;
            camImageL.camera = cameras[2 * i];
            camImageL.isSuccess = true;

            camImageR.grabEndTime = clock();
            camImageR.device = device;
            camImageR.camera = cameras[2 * i + 1];
            camImageR.isSuccess = true;

            int w = image.cols;
            int h = image.rows;

            //第一个相机直接赋值,后面的相机图片是要拷贝一下
            if (i == 0) {
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
        for (size_t i = 0; i < result.size() / 2; i++) {
            CameraImage& camImageL = result[2 * i];
            CameraImage& camImageR = result[2 * i + 1];

            camImageL.grabEndTime = clock();
            camImageL.device = device;
            camImageL.camera = cameras[2 * i];
            camImageL.isSuccess = false;

            camImageR.grabEndTime = clock();
            camImageR.device = device;
            camImageR.camera = cameras[2 * i + 1];
            camImageR.isSuccess = false;
        }
    }

    return result;
}

} // namespace dxlib