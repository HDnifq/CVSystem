#include "CameraManger.h"
#include "dlog/dlog.h"

#define XUEXUE_JSON_SUPPORT_OPENCV
#include "xuexuejson/Serialize.hpp"

namespace dxlib {

CameraManger::CameraManger()
{
}

CameraManger::~CameraManger()
{
}

CameraManger* CameraManger::m_pInstance = NULL;

pCamera CameraManger::add(pCamera cp)
{
    if (camMap.find(cp->camIndex) != camMap.end()) {
        LogE("CameraManger.add():添加相机有重复的CamIndex=%d，添加失败!", cp->camIndex);
        return nullptr;
    }
    if (mNamePCamera.find(cp->devName) != mNamePCamera.end()) {
        LogE("CameraManger.add():添加相机有重复的devName=%s，添加失败!", cp->devName.c_str());
        return nullptr;
    }

    cp->camIndex = (int)camMap.size(); //分配一个camIndex
    camMap[cp->camIndex] = cp;
    mNamePCamera[cp->devName] = cp;
    vCamera.push_back(cp);

    //检查一下vCamera
    for (int i = 0; i < vCamera.size(); i++) {
        CV_Assert(vCamera[i]->camIndex == i);
    }

    LogI("CameraManger.add():添加了一个相机%s,当前相机个数%d！", cp->devName.c_str(), camMap.size());
    return cp;
}

pCamera CameraManger::addVirtual(pCamera cp)
{
    cp->isVirtualCamera = true;
    return add(cp);
}

pStereoCamera CameraManger::add(pStereoCamera sc)
{
    if (sc->scID < 0) {
        sc->scID = static_cast<int>(vStereo.size()); //这个id就是vStereo里的index
    }
    else {
        if (sc->scID != vStereo.size()) {
            LogE("CameraManger.add():添加立体相机逻辑scID有错误,已经修改scId为%d", vStereo.size());
            sc->scID = static_cast<int>(vStereo.size());
        }
    }
    this->vStereo.push_back(sc);
    return sc;
}

pCameraPair CameraManger::add(pCameraPair cp)
{
    if (cp->cpID < 0) {
        cp->cpID = static_cast<int>(vCameraPair.size()); //这个id就是vCameraPair里的index
    }
    else {
        if (cp->cpID != vCameraPair.size()) {
            LogE("CameraManger.add():添加相机对逻辑cpID有错误,已经修改cpID为%d", vCameraPair.size());
            cp->cpID = static_cast<int>(vCameraPair.size());
        }
    }
    this->vCameraPair.push_back(cp);
    return cp;
}

pCamera CameraManger::getCamera(const int camIndex)
{
    if (camMap.find(camIndex) != camMap.end()) {
        return camMap[camIndex];
    }
    return nullptr;
}

pCamera CameraManger::getCamera(const std::string& devName)
{
    if (mNamePCamera.find(devName) != mNamePCamera.end()) {
        return mNamePCamera[devName];
    }
    return nullptr;
}

std::vector<pCamera>& CameraManger::getCameraVec()
{
    return this->vCamera;
}

void CameraManger::clear()
{
    this->camMap.clear();
    this->vCamera.clear();
    this->mNamePCamera.clear();
    this->vStereo.clear();
}

pStereoCamera CameraManger::getStereo(const std::string& name)
{
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->name == name) {
            return vStereo[i];
        }
    }
    return nullptr;
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

pCameraPair CameraManger::getCameraPair(const std::string& name)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->name == name) {
            return vCameraPair[i];
        }
    }
    return nullptr;
}

pCameraPair CameraManger::getCameraPair(pCamera cameraL, pCamera cameraR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL == cameraL &&
            vCameraPair[i]->camR == cameraR) {
            return vCameraPair[i];
        }
    }
    return nullptr;
}

pCameraPair CameraManger::getCameraPair(int cameraL, int cameraR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL->camIndex == cameraL &&
            vCameraPair[i]->camR->camIndex == cameraR) {
            return vCameraPair[i];
        }
    }
    return nullptr;
}

bool CameraManger::setProp(int camIndex, cv::VideoCaptureProperties CAP_PROP, double value)
{
    bool success = false;
    auto iter = camMap.find(camIndex);
    if (iter != camMap.end()) {
        iter->second->setProp(CAP_PROP, value);
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
            LogW("CameraManger.isAllCameraIsOpen():相机 %s 没有打开！", camera->devName.c_str());
        }
    }
    return isAllOpen;
}

bool CameraManger::checkInputData()
{
    bool isRight = true;

    for (auto kvp : camMap) {
        if (kvp.first != kvp.second->camIndex) {
            LogE("CameraManger.checkDataInput():相机的camIndex录入有错误,和map的key值不同.");
            isRight = false;
        }
    }

    for (size_t i = 0; i < vStereo.size(); i++) {
        auto& psc = vStereo[i];
        uint scID = vStereo[i]->scID;
        if (i != scID) {
            LogE("CameraManger.checkDataInput():CameraManger中立体相机的scID=%lu录入有问题,和vStereo的index不一致.", scID);
            isRight = false;
        }
        if (psc->camL->camTR4x4.empty() && !psc->camTR4x4.empty()) {
            LogW("CameraManger.checkDataInput():CameraManger设置了立体相机%s,但是没有设置camL的camTR4x4.", psc->name.c_str());
            psc->camL->camTR4x4 = psc->camTR4x4;
            psc->camL->camPos = psc->camPos;
            psc->camL->camRotate = psc->camRotate;
        }
    }

    return isRight;
}

} // namespace dxlib