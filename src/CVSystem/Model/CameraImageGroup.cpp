#include "CameraImageGroup.h"
#include "dlog/dlog.h"

namespace dxlib {

void CameraImageGroup::addCameraImage(const CameraImage& camImage)
{
    this->vImage.push_back(camImage);

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
    return (float)(procEndTime - procStartTime) / CLOCKS_PER_SEC * 1000;
}

CameraImage CameraImageGroup::getImage(int camIndex) const
{
    //如果vector的index是能够对应上的，那么就直接返回这一项
    if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
        return vImage[camIndex];
    }
    //如果对应不上那么只能遍历搜索
    for (int i = 0; i < vImage.size(); i++) {
        if (vImage[i].camera->camIndex == camIndex) {
            return vImage[i];
        }
    }
    LogE("CameraImageGroup.getItem():输入的camIndex未能查找到!camIndex=%d", camIndex);
    return CameraImage();
}

bool CameraImageGroup::getImage(int camIndex, CameraImage& item) const
{
    //如果vector的index是能够对应上的，那么就直接返回这一项
    if (vImage.size() > camIndex && vImage[camIndex].camera->camIndex == camIndex) {
        item = vImage[camIndex];
        return true;
    }

    //如果对应不上那么只能遍历搜索
    for (int i = 0; i < vImage.size(); i++) {
        if (vImage[i].camera->camIndex == camIndex) {
            item = vImage[camIndex];
            return true;
        }
    }
    LogE("CameraImageGroup.getItem():输入的camIndex未能查找到!camIndex=%d", camIndex);
    return false;
}

} // namespace dxlib