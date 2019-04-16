#include "DevicesHelper.h"

#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Quartz.lib")

#if defined _MSC_VER && _MSC_VER >= 100
//'sprintf': name was marked as #pragma deprecated
#pragma warning(disable : 4995)
#endif

#ifdef __MINGW32__
// MinGW does not understand COM interfaces
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <wchar.h>

#include <vector>

#include "DShow.h"
//#include "strmif.h"
//#include "Aviriff.h"
//#include "dvdmedia.h"
//#include "bdaiface.h"
#include <map>
#include <iostream>

#include "../Common/StringHelper.h"
#include "../dlog/dlog.h"

namespace dxlib {

DevicesHelper* DevicesHelper::m_pInstance = NULL;

DevicesHelper::DevicesHelper()
{
}

DevicesHelper::~DevicesHelper()
{
}

bool DevicesHelper::comInit()
{
    HRESULT hr = NOERROR;

    //no need for us to start com more than once
    if (comInitCount == 0) {
        // Initialize the COM library.
        //CoInitializeEx so videoInput can run in another thread
#ifdef VI_COM_MULTI_THREADED
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
        hr = CoInitialize(NULL);
#endif
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

bool DevicesHelper::comUnInit()
{
    if (comInitCount > 0) comInitCount--; //decrease the count of instances using com

    if (comInitCount == 0) {
        CoUninitialize(); //if there are no instances left - uninitialize com
        return true;
    }

    return false;
}

int DevicesHelper::getIndexWithName(std::wstring name)
{

    for (int i = 0; i < devList.size(); i++) {
        if (devList.at(i) == name) {
            return i;
        }
    }
    LogW("DevicesHelper.getIndexWithName() :未能找到摄像机 %s ,当前系统相机个数%d!", StringHelper::ws2s(name).c_str(), devList.size());
    return -1;
}

std::map<int, std::wstring> DevicesHelper::getDevListWithNames(const std::wstring name[], int length)
{
    //要打开的设备列表
    std::map<int, std::wstring> openDevList;
    for (int i = 0; i < length; i++) {
        int index = getIndexWithName(name[i]);
        if (index < 0) {
            LogW("DevicesHelper.getDevListWithNames() :未能找到摄像机 %s!", StringHelper::ws2s(name[i]).c_str());
        }
        else {
            openDevList[index] = name[i];
        }
    }
    return openDevList;
}

int DevicesHelper::listDevices(std::map<int, std::wstring>& devListout)
{
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

    return deviceCounter;
}

int DevicesHelper::listDevices()
{
    this->devList.clear();
    return listDevices(devList);
}
} // namespace dxlib