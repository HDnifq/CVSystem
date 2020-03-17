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

pCamera CameraManger::add(pCamera cp, bool isVirtualCamera)
{
    camMap[cp->camIndex] = cp;
    cp->isVirtualCamera = isVirtualCamera;
    LogI("CameraManger.add():添加了一个相机%s,当前相机个数%d！", cp->devNameA.c_str(), camMap.size());
    return camMap[cp->camIndex];
}

pCamera CameraManger::addAssist(pCamera cp)
{
    camMapAssist[cp->camIndex] = cp;
    cp->isAssist = true;
    LogI("CameraManger.addAssist():添加了一个相机%s,当前Assist相机个数%d！", cp->devNameA.c_str(), camMapAssist.size());
    return camMapAssist[cp->camIndex];
}

void CameraManger::add(pStereoCamera sc)
{
    sc->scID = vStereo.size(); //这个id就是vStereo里的index
    this->vStereo.push_back(sc);
}

pCamera CameraManger::getCamera(const std::string& devName)
{
    for (auto& kvp : camMap) {
        if (kvp.second->devNameA == devName) {
            return kvp.second;
        }
    }
    return nullptr;
}

void CameraManger::clear()
{
    this->camMap.clear();
    this->camMapAssist.clear();
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
    bool success = false;
    auto iter = camMap.find(camIndex);
    if (iter != camMap.end()) {
        iter->second->setProp(CAP_PROP, value);
        success = true;
    }

    auto iter2 = camMapAssist.find(camIndex);
    if (iter2 != camMapAssist.end()) {
        iter2->second->setProp(CAP_PROP, value);
        success = true;
    }
    return success;
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

    for (auto& kvp : this->camMapAssist) {
        pCamera& camera = kvp.second;
        if (!camera->isVirtualCamera && !camera->isOpened()) {
            isAllOpen = false;
            LogW("CameraManger.isAllCameraIsOpen():Assist相机 %s 没有打开！", camera->devNameA.c_str());
        }
    }
    return isAllOpen;
}

void CameraManger::initUndistortRectifyMap(pCamera& camera)
{
    cv::initUndistortRectifyMap(camera->camMatrix, camera->distCoeffs, camera->R, camera->P, camera->size, CV_16SC2, camera->rmap1, camera->rmap2);
}
} // namespace dxlib