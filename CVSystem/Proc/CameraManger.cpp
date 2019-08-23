#include "CameraManger.h"

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

////目前未使用
//void CameraManger::load(std::string filePath)
//{
//    cv::FileStorage fsr(filePath, cv::FileStorage::READ);
//    if (camMap.find(0) != camMap.end()) {//因为目前倒过来了
//        fsr["M2"] >> camMap[0]->camMatrix;
//        fsr["D2"] >> camMap[0]->distCoeffs;
//        fsr["R2"] >> camMap[0]->R;
//        fsr["P2"] >> camMap[0]->P;
//        initUndistortRectifyMap(camMap[0]);
//    }
//    if (camMap.find(1) != camMap.end()) {
//        fsr["M1"] >> camMap[1]->camMatrix;
//        fsr["D1"] >> camMap[1]->distCoeffs;
//        fsr["R1"] >> camMap[1]->R;
//        fsr["P1"] >> camMap[1]->P;
//        initUndistortRectifyMap(camMap[1]);
//    }
//    if (camMap.find(2) != camMap.end()) {
//        //复制一份参数
//        fsr["M2"] >> camMap[2]->camMatrix;
//        fsr["D2"] >> camMap[2]->distCoeffs;
//        fsr["R2"] >> camMap[2]->R;
//        fsr["P2"] >> camMap[2]->P;
//        initUndistortRectifyMap(camMap[2]);
//    }
//    if (camMap.find(3) != camMap.end()) {
//        //复制一份参数
//        fsr["M1"] >> camMap[3]->camMatrix;
//        fsr["D1"] >> camMap[3]->distCoeffs;
//        fsr["R1"] >> camMap[3]->R;
//        fsr["P1"] >> camMap[3]->P;
//        initUndistortRectifyMap(camMap[3]);
//    }
//    fsr.release();

//}

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

void CameraManger::initUndistortRectifyMap(pCamera& camera)
{
    cv::initUndistortRectifyMap(camera->camMatrix, camera->distCoeffs, camera->R, camera->P, camera->size, CV_16SC2, camera->rmap1, camera->rmap2);
}
} // namespace dxlib