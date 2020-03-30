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

pCamera CameraManger::add(pCamera cp)
{
    if (camMap.find(cp->camIndex) != camMap.end()) {
        LogE("CameraManger.add():添加相机有重复的CamIndex=%d，添加失败!", cp->camIndex);
        return nullptr;
    }

    camMap[cp->camIndex] = cp;
    LogI("CameraManger.add():添加了一个相机%s,当前相机个数%d！", cp->devNameA.c_str(), camMap.size());
    return camMap[cp->camIndex];
}

pCamera CameraManger::addVirtual(pCamera cp)
{
    cp->isVirtualCamera = true;
    return add(cp);
}

pCamera CameraManger::addAssist(pCamera cp)
{
    cp->isAssist = true;
    return add(cp);
}

void CameraManger::add(pStereoCamera sc)
{
    sc->scID = vStereo.size(); //这个id就是vStereo里的index
    this->vStereo.push_back(sc);
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
            LogW("CameraManger.isAllCameraIsOpen():相机 %s 没有打开！", camera->devNameA.c_str());
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
        uint scID = vStereo[i]->scID;
        if (i != scID) {
            LogE("CameraManger.checkDataInput():CameraManger中立体相机的scID录入有问题,和vStereo的index不一致.");
            isRight = false;
        }
    }

    return isRight;
}

void CameraManger::initUndistortRectifyMap(pCamera& camera)
{
    cv::initUndistortRectifyMap(camera->camMatrix, camera->distCoeffs, camera->R, camera->P, camera->size, CV_16SC2, camera->rmap1, camera->rmap2);
}
} // namespace dxlib