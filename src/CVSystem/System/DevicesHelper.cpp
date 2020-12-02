#include "DevicesHelper.h"
#include "xuexuejson/Serialize.hpp"

#if defined _MSC_VER && _MSC_VER >= 100
//'sprintf': name was marked as #pragma deprecated
#    pragma warning(disable : 4995)
#endif

#ifdef __MINGW32__
// MinGW does not understand COM interfaces
#    pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <wchar.h>

#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#    include <tchar.h>
#    include "DShow.h"
//#    include "atlstr.h"
//#include "strmif.h"
//#include "Aviriff.h"
//#include "dvdmedia.h"
//#include "bdaiface.h"

#    pragma comment(lib, "Strmiids.lib")
#    pragma comment(lib, "Quartz.lib")

#else
//#    include <boost/filesystem.hpp>
#endif

#include <map>
#include <iostream>
#include <regex>
#include <mutex>

#include "../Common/StringHelper.h"
#include "dlog/dlog.h"

#include "Poco/Path.h"
#include "Poco/File.h"

using namespace dxlib::cvsystem;

namespace dxlib {

DevicesHelper* DevicesHelper::m_pInstance = NULL;

DevicesHelper::DevicesHelper()
{
}

DevicesHelper::~DevicesHelper()
{
}

int DevicesHelper::getIndexWithName(const std::string& name, bool isRegex, bool isWarning)
{
    for (int i = 0; i < devList.size(); i++) {
        if (isRegex) {
            const std::regex pattern(name);
            if (std::regex_search(devList.at(i), pattern)) {
                if (name != devList.at(i)) {
                    LogW("DevicesHelper.getIndexWithName():传入设备名不准确,实际上%s->%s!", name.c_str(), devList.at(i).c_str());
                }
                return i;
            }
        }
        else {
            if (devList.at(i) == name) {
                return i;
            }
        }
    }
    if (isWarning)
        LogW("DevicesHelper.getIndexWithName():未能找到摄像机 %s ,当前系统相机个数%d!", name.c_str(), devList.size());
    return -1;
}

std::map<int, std::string> DevicesHelper::getDevListWithNames(const std::vector<std::string>& names, bool isRegex, bool isWarning)
{
    //要打开的设备列表
    std::map<int, std::string> openDevList;
    for (int i = 0; i < names.size(); i++) {
        int index = getIndexWithName(names[i], isRegex, isWarning);
        if (index >= 0) {
            openDevList[index] = names[i];
        }
    }
    return openDevList;
}

#if defined(_WIN32) || defined(_WIN64)

// 用于com初始化的计数.
int comInitCount = 0;

bool comInit()
{
    HRESULT hr = NOERROR;

    //no need for us to start com more than once
    if (comInitCount == 0) {
        // Initialize the COM library.
        //CoInitializeEx so videoInput can run in another thread
#    ifdef VI_COM_MULTI_THREADED
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#    else
        hr = CoInitialize(NULL);
#    endif
        //this is the only case where there might be a problem
        //if another library has started com as single threaded
        //and we need it multi-threaded - send warning but don't fail
        if (hr == RPC_E_CHANGED_MODE) {
            {
            } //DebugPrintOut("SETUP - COM already setup - threaded VI might not be possible\n");
        }
    }

    comInitCount++;
    return true;
}

bool comUnInit()
{
    if (comInitCount > 0) comInitCount--; //decrease the count of instances using com

    if (comInitCount == 0) {
        CoUninitialize(); //if there are no instances left - uninitialize com
        return true;
    }

    return false;
}

//设备名.
char deviceNames[20][255];
std::mutex mut;

int DevicesHelper::listDevices(std::map<int, std::wstring>& devListout)
{
    mut.lock();

    memset(deviceNames, 0, 20 * 255);
    devListout.clear();
    //COM Library Intialization
    comInit();

    //if (!silent) {}//DebugPrintOut("\nVIDEOINPUT SPY MODE!\n\n");

    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    int deviceCounter = 0;

    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                                  reinterpret_cast<void**>(&pDevEnum));

    if (SUCCEEDED(hr)) {
        // Create an enumerator for the video capture category.
        hr = pDevEnum->CreateClassEnumerator(
            CLSID_VideoInputDeviceCategory,
            &pEnum, 0);

        if (hr == S_OK) {
            //if (!silent) {}//DebugPrintOut("SETUP: Looking For Capture Devices\n");
            IMoniker* pMoniker = NULL;

            while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
                IPropertyBag* pPropBag;
                hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                                             (void**)(&pPropBag));

                if (FAILED(hr)) {
                    pMoniker->Release();
                    continue; // Skip this one, maybe the next one will work.
                }

                // Find the description or friendly name.
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropBag->Read(L"Description", &varName, 0);

                if (FAILED(hr)) hr = pPropBag->Read(L"FriendlyName", &varName, 0);

                if (SUCCEEDED(hr)) {
                    hr = pPropBag->Read(L"FriendlyName", &varName, 0);

                    int count = 0;
                    int maxLen = sizeof(deviceNames[0]) / sizeof(deviceNames[0][0]) - 2;
                    while (varName.bstrVal[count] != 0x00 && count < maxLen) {
                        deviceNames[deviceCounter][count] = (char)varName.bstrVal[count];
                        count++;
                    }
                    deviceNames[deviceCounter][count] = 0;

                    //if (!silent) {}//DebugPrintOut("SETUP: %i) %s\n", deviceCounter, deviceNames[deviceCounter]);
                }
                devListout[deviceCounter] = varName.bstrVal;
                pPropBag->Release();
                pPropBag = NULL;

                pMoniker->Release();
                pMoniker = NULL;

                deviceCounter++;
            }

            pDevEnum->Release();
            pDevEnum = NULL;

            pEnum->Release();
            pEnum = NULL;
        }

        //if (!silent) {}//DebugPrintOut("SETUP: %i Device(s) found\n\n", deviceCounter);
    }

    comUnInit();

    mut.unlock();
    return deviceCounter;
}

void setCameraProperties(std::string cameraName)
{
}

#elif __linux

//int DevicesHelper::listDevices(std::map<int, std::wstring>& devListout)
//{
//    namespace fs = boost::filesystem;
//    using namespace std;
//
//    //设备在v4l里
//    fs::path pv4l = "/dev/v4l";
//    if (!fs::exists(pv4l)) //不存在相机
//    {
//        return 0;
//    }
//    fs::directory_iterator di(pv4l / "by-id");
//    for (auto& de : di) {
//        wstring name = de.path().filename().wstring(); //设备名字
//        //看看能不能匹配到usb-F3D_USB_Camera_201903050001-video-index0
//        //usb-046d_HD_Pro_Webcam_C920_A15963EF-video-index0
//        const std::wregex pattern(L".*index0$");
//        if (regex_match(name, pattern)) {
//            LogI("DevicesHelper.listDevices():找到了设备%s", StringHelper::ws2s(name).c_str());
//            devListout[devListout.size()] = de.path().wstring();
//        }
//    }
//    return devListout.size();
//}
int DevicesHelper::listDevices(std::map<int, std::wstring>& devListout)
{

    //设备在v4l里
    Poco::File pv4l = "/dev/v4l";
    if (!pv4l.exists()) //不存在相机
    {
        return 0;
    }

    Poco::File pv4ldir = ("/dev/v4l/by-id/");
    if (!pv4ldir.exists()) //不存在相机
    {
        return 0;
    }
    std::vector<Poco::File> vFiles;
    pv4ldir.list(vFiles);

    for (size_t i = 0; i < vFiles.size(); i++) {
        Poco::File& file = vFiles[i];
        Poco::Path filePath(file.path());
        std::string name = filePath.getFileName(); //设备名字

        //看看能不能匹配到usb-F3D_USB_Camera_201903050001-video-index0
        //usb-046d_HD_Pro_Webcam_C920_A15963EF-video-index0
        const std::regex pattern(".*index0$");
        if (regex_match(name, pattern)) {
            LogI("DevicesHelper.listDevices():找到了设备%s", name.c_str());
            devListout[devListout.size()] = xuexue::json::JsonHelper::utf8To16(filePath.toString());
        }
    }
    return devListout.size();
}
#endif

int DevicesHelper::listDevices()
{
    this->devList.clear();
    std::map<int, std::wstring> wDevListout;
    listDevices(wDevListout);

    for (auto& kvp : wDevListout) {
        devList[kvp.first] = xuexue::json::JsonHelper::utf16To8(kvp.second);
    }
    return devList.size();
}
} // namespace dxlib