#include "CameraImageGroup.h"
#include "dlog/dlog.h"

namespace dxlib {

void CameraImageGroup::addCameraImage(const CameraImage& camImage)
{
    //记录图片
    this->vImage.push_back(camImage);

    //记录立体相机的结构的图片
    if (camImage.camera->scID >= 0) {
        int size = camImage.camera->scID + 1;
        if (camImage.type == CameraImage::ImageType::StereoL) {
            this->vStereoImage.resize(size);
            this->vStereoImage[camImage.camera->scID][0] = camImage;
        }
        else if (camImage.type == CameraImage::ImageType::StereoR) {
            this->vStereoImage.resize(size);
            this->vStereoImage[camImage.camera->scID][1] = camImage;
        }
    }

    if (grabStartTime == 0 || grabStartTime > camImage.grabStartTime) {
        grabStartTime = camImage.grabStartTime;
    }
    if (grabEndTime == 0 || grabEndTime < camImage.grabEndTime) {
        grabEndTime = camImage.grabEndTime;
    }
}

float CameraImageGroup::grabCostTime()
{
    return (float)(grabEndTime - grabStartTime) / CLOCKS_PER_SEC * 1000;
}

float CameraImageGroup::waitProcTime()
{
    return (float)(procStartTime - grabEndTime) / CLOCKS_PER_SEC * 1000;
}

float CameraImageGroup::procCostTime()
{
    //如果还没有标记当前的这个计时,那就使用当前时间好了
    if (procEndTime == 0) {
        procEndTime = clock();
    }
    return (float)(procEndTime - procStartTime) / CLOCKS_PER_SEC * 1000;
}

const CameraImage& CameraImageGroup::getImage(int camIndex) const
{
    //如果vector的index是能够对应上的，那么就直接返回这一项
    if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
        //pCameraImage pCamImage = std::make_shared<CameraImage>();
        //*pCamImage = vImage[camIndex];
        return vImage[camIndex];
    }
    //如果对应不上那么只能遍历搜索
    for (int i = 0; i < vImage.size(); i++) {
        if (vImage[i].camera->camIndex == camIndex) {
            //pCameraImage pCamImage = std::make_shared<CameraImage>();
            //*pCamImage = vImage[camIndex];
            return vImage[camIndex];
        }
    }
    LogE("CameraImageGroup.getItem():输入的camIndex未能查找到!camIndex=%d", camIndex);
    /*return nullptr;*/
    throw std::exception();
}

} // namespace dxlib