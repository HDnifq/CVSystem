#include "CameraManger.h"
#include "dlog/dlog.h"

namespace dxlib {

CameraManger::CameraManger()
{
}

CameraManger::~CameraManger()
{
}

CameraManger* CameraManger::m_pInstance = NULL;

void CameraManger::add(const pCamera& cp, bool isVirtualCamera)
{
    camMap[cp->camIndex] = cp;
    cp->isVirtualCamera = isVirtualCamera;
}

void CameraManger::add(pStereoCamera sc)
{
    sc->scID = vStereo.size(); //这个id就是vStereo里的index
    this->vStereo.push_back(sc);
}

void CameraManger::clear()
{
    this->camMap.clear();
    this->vStereo.clear();
}

pStereoCamera CameraManger::getStereo(pCamera camera)
{
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL == camera) {
            return vStereo[i];
        }
        if (vStereo[i]->camR == camera) {
            return vStereo[i];
        }
    }
    return nullptr;
}

pStereoCamera CameraManger::getStereo(int camIndex)
{
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL->camIndex == camIndex) {
            return vStereo[i];
        }
        if (vStereo[i]->camR->camIndex == camIndex) {
            return vStereo[i];
        }
    }
    return nullptr;
}

bool CameraManger::setProp(int camIndex, int CAP_PROP, double value)
{
    auto iter = camMap.find(camIndex);
    if (iter != camMap.end()) {
        iter->second->setProp(CAP_PROP, value);
        return true;
    }
    return false;
}

bool CameraManger::isAllCameraIsOpen()
{
    bool isAllOpen = true;
    for (auto& kvp : this->camMap) {
        pCamera& camera = kvp.second;
        if (!camera->isVirtualCamera && !camera->isOpened()) {
            isAllOpen = false;
            LogW("CameraManger.isAllCameraIsOpen():相机 %s 没有打开！", camera->devNameA.c_str());
        }
    }
    return isAllOpen;
}

void CameraManger::initUndistortRectifyMap(pCamera& camera)
{
    cv::initUndistortRectifyMap(camera->camMatrix, camera->distCoeffs, camera->R, camera->P, camera->size, CV_16SC2, camera->rmap1, camera->rmap2);
}
} // namespace dxlib