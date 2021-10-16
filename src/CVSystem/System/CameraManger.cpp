#include "CameraManger.h"
#include "dlog/dlog.h"

#include "../Model/MonoCameraImageFactory.h"
#include "../Model/StereoCameraImageFactory.h"

namespace dxlib {

CameraManger* CameraManger::m_pInstance = new CameraManger();

CameraManger::CameraManger()
{
}

CameraManger::~CameraManger()
{
}

void CameraManger::CreateCamera(const std::string devName, int w, int h, pCameraDevice& device, pCamera& camera)
{
    device = pCameraDevice(new CameraDevice(devName, cv::Size(w, h), 0));
    CameraManger::GetInst()->add(device);

    camera = pCamera(new Camera(devName, cv::Size(w, h)));
    CameraManger::GetInst()->add(camera);

    pCameraImageFactory factory = pCameraImageFactory(new MonoCameraImageFactory(device, camera));
    CameraManger::GetInst()->add(factory);

    LogI("CameraManger.CreateStereoCamera():工厂方法创建一个立体相机,camIndex=%d", camera->camIndex);
}

void CameraManger::CreateStereoCamera(const std::string devName, int w, int h, pCameraDevice& device, pStereoCamera& stereo)
{
    device = pCameraDevice(new CameraDevice(devName, cv::Size(w, h), 0));
    CameraManger::GetInst()->add(device);

    pCamera cameraL = pCamera(new Camera(devName + "-L", cv::Size(w / 2, h)));
    CameraManger::GetInst()->add(cameraL);

    pCamera cameraR = pCamera(new Camera(devName + "-R", cv::Size(w / 2, h)));
    CameraManger::GetInst()->add(cameraR);

    stereo = pStereoCamera(new StereoCamera());
    CameraManger::GetInst()->add(stereo);
    stereo->setCamera(stereo->scID, device, cameraL, cameraR); //设置sc数据

    ICameraImageFactory* pfactory = new StereoCameraImageFactory(device, {cameraL, cameraR});
    pCameraImageFactory factory = pCameraImageFactory(pfactory);
    CameraManger::GetInst()->add(factory);
    LogI("CameraManger.CreateStereoCamera():工厂方法创建一个立体相机,scID=%d", stereo->scID);
}

pCameraDevice CameraManger::add(const pCameraDevice& device)
{
    //检察重复
    for (size_t i = 0; i < vDevice.size(); i++) {
        if (vDevice[i] == device) {
            LogE("CameraManger.add():传入了一个重复设备指针%s,当前相机个数%zu,直接返回！", device->devName.c_str(), vDevice.size());
            return device;
        }
        if (vDevice[i]->devName == device->devName) {
            LogE("CameraManger.add():传入了一个重复设备名称%s,当前相机个数%zu,直接返回！", device->devName.c_str(), vDevice.size());
            return device;
        }
    }

    if (device->id < 0)
        device->id = (int)vDevice.size(); //分配一个id
    vDevice.push_back(device);
    LogI("CameraManger.add():添加了一个设备%s,当前设备个数%d！", device->devName.c_str(), vDevice.size());
    return device;
}

pCamera CameraManger::add(const pCamera& camera)
{
    if (camera->camIndex < 0) {
        camera->camIndex = (int)mCamera.size(); //分配一个camIndex
    }
    else if (mCamera.find(camera->camIndex) != mCamera.end()) {
        LogE("CameraManger.add():添加相机有重复的CamIndex=%d，添加失败!", camera->camIndex);
        return nullptr;
    }

    mCamera[camera->camIndex] = camera;
    vCamera.push_back(camera); //添加一个

    //检查一下vCamera
    for (int i = 0; i < vCamera.size(); i++) {
        CV_Assert(vCamera[i]->camIndex == i);
    }
    LogI("CameraManger.add():添加了一个相机%s,当前相机个数%zu！", camera->name.c_str(), mCamera.size());
    return camera;
}

pCameraImageFactory CameraManger::add(const pCameraImageFactory& cmf)
{
    //检察重复
    for (size_t i = 0; i < vCameraImageFactory.size(); i++) {
        if (vCameraImageFactory[i] == cmf) {
            LogE("CameraManger.add():传入了一个重复图片转换器%s,直接返回！", cmf->device->devName.c_str());
            return cmf;
        }
    }

    vCameraImageFactory.push_back(cmf);
    LogI("CameraManger.add():添加了一个图片转换器%s,当前ImageFactory个数%zu！", cmf->device->devName.c_str(), vCameraImageFactory.size());
    return cmf;
}

pStereoCamera CameraManger::add(const pStereoCamera& sc)
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

pCameraPair CameraManger::add(const pCameraPair& cp)
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
    if (mCamera.find(camIndex) != mCamera.end()) {
        return mCamera[camIndex];
    }
    return nullptr;
}

pCamera CameraManger::getCamera(const std::string& name)
{
    for (auto& kvp : mCamera) {
        if (kvp.second->name == name) {
            return kvp.second;
        }
    }
    return nullptr;
}

pCameraDevice CameraManger::getDevice(const std::string& name)
{
    for (auto& dev : vDevice) {
        if (dev->devName == name) {
            return dev;
        }
    }
    return nullptr;
}

std::vector<pCamera>& CameraManger::getCameraVec()
{
    return this->vCamera;
}

void CameraManger::clear()
{
    this->mCamera.clear();
    this->vCamera.clear();
    this->vDevice.clear();
    this->vCameraImageFactory.clear();
    this->vStereo.clear();
    this->vCameraPair.clear();
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

pStereoCamera CameraManger::getStereo(const pCamera& camera)
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

pStereoCamera CameraManger::getStereo(const pCamera& cameraL, const pCamera& cameraR)
{
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL == cameraL &&
            vStereo[i]->camR == cameraR) {
            return vStereo[i];
        }
    }
    return nullptr;
}

pStereoCamera CameraManger::getStereo(int camIndexL, int camIndexR)
{
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL->camIndex == camIndexL &&
            vStereo[i]->camR->camIndex == camIndexR) {
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

pCameraPair CameraManger::getCameraPair(const pCamera& cameraL, const pCamera& cameraR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL == cameraL &&
            vCameraPair[i]->camR == cameraR) {
            return vCameraPair[i];
        }
    }
    return nullptr;
}

pCameraPair CameraManger::getCameraPair(int camIndexL, int camIndexR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL->camIndex == camIndexL &&
            vCameraPair[i]->camR->camIndex == camIndexR) {
            return vCameraPair[i];
        }
    }
    return nullptr;
}

IStereo* CameraManger::getIStereo(const pCamera& cameraL, const pCamera& cameraR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL == cameraL &&
            vCameraPair[i]->camR == cameraR) {
            return vCameraPair[i].get();
        }
    }
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL == cameraL &&
            vStereo[i]->camR == cameraR) {
            return vStereo[i].get();
        }
    }
    return nullptr;
}

IStereo* CameraManger::getIStereo(int camIndexL, int camIndexR)
{
    for (size_t i = 0; i < vCameraPair.size(); i++) {
        if (vCameraPair[i]->camL->camIndex == camIndexL &&
            vCameraPair[i]->camR->camIndex == camIndexR) {
            return vCameraPair[i].get();
        }
    }
    for (size_t i = 0; i < vStereo.size(); i++) {
        if (vStereo[i]->camL->camIndex == camIndexL &&
            vStereo[i]->camR->camIndex == camIndexR) {
            return vStereo[i].get();
        }
    }
    return nullptr;
}

bool CameraManger::setProp(int camIndex, cv::VideoCaptureProperties CAP_PROP, double value)
{
    bool success = false;
    auto iter = mCamera.find(camIndex);
    if (iter != mCamera.end()) {
        if (iter->second->device != nullptr)
            iter->second->device->setProp(CAP_PROP, value);
        success = true;
    }
    return success;
}

bool CameraManger::isAllCameraIsOpen()
{
    bool isAllOpen = true;
    for (auto& v : this->vDevice) {
        if (!v->isOpened()) {
            isAllOpen = false;
            LogW("CameraManger.isAllCameraIsOpen():相机 %s 没有打开！", v->devName.c_str());
        }
    }
    return isAllOpen;
}

bool CameraManger::checkInputData()
{
    bool isRight = true;

    for (auto kvp : mCamera) {
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
        if ((psc->camL->camTR4x4.empty() || psc->camR->camTR4x4.empty()) && !psc->camTR4x4.empty()) {
            LogE("CameraManger.checkDataInput():CameraManger设置了立体相机%s,但是没有设置camL的camTR4x4.", psc->name.c_str());
            isRight = false;
        }
    }
    return isRight;
}

} // namespace dxlib
