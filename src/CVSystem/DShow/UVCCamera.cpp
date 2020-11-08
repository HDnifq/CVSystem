#if defined(_WIN32) || defined(_WIN64)
#    include "UVCCamera.h"
#    include "baseclasses/mtype.h"

namespace dxlib {

/**********************************************Implementation of UVCXU class*********************************************************/
UVCXU::UVCXU()
{
    SAFE_RELEASE(m_pKsControl);
    m_pKsControl = NULL;
    m_dwNodeId = 0;
    UvcXuGudi = {0};
}

UVCXU::~UVCXU()
{
    SAFE_RELEASE(m_pKsControl);
}

HRESULT
UVCXU::QueryUvcXuInterface(
    IBaseFilter *pDeviceFilter,
    GUID extensionGuid,
    DWORD FirstNodeId)
{
    HRESULT hr = S_OK;
    IKsTopologyInfo *pIksTopologyInfo = NULL;
    DWORD numberOfNodes;
    KSP_NODE ExtensionProp;

    if (pDeviceFilter == NULL)
        return E_POINTER;

    hr = pDeviceFilter->QueryInterface(__uuidof(IKsTopologyInfo), (void **)&pIksTopologyInfo);
    if (FAILED(hr))
        return hr;

    hr = pIksTopologyInfo->get_NumNodes(&numberOfNodes);
    if (FAILED(hr) || numberOfNodes == 0) {
        SAFE_RELEASE(pIksTopologyInfo);
        return E_FAIL;
    }

    DWORD i = 0, j = 0;
    GUID nodeGuid;
    if (FirstNodeId < numberOfNodes)
        i = FirstNodeId;

    for (; i < numberOfNodes; i++) {
        if (SUCCEEDED(pIksTopologyInfo->get_NodeType(i, &nodeGuid))) {
            if (IsEqualGUID(KSNODETYPE_DEV_SPECIFIC, nodeGuid)) {
#    if _DEBUG
                printf("found one xu node, NodeId = %d\n", i);
#    endif
                SAFE_RELEASE(m_pKsControl);
                m_pKsControl = NULL;
                hr = pIksTopologyInfo->CreateNodeInstance(i, IID_IKsControl, (void **)&m_pKsControl);
                if (SUCCEEDED(hr)) {
                    ULONG ulBytesReturned;
                    ExtensionProp.Property.Set = extensionGuid;
                    ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
                    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SETSUPPORT | KSPROPERTY_TYPE_TOPOLOGY;
                    ExtensionProp.NodeId = i;
                    ExtensionProp.Reserved = 0;

                    hr = m_pKsControl->KsProperty(
                        (PKSPROPERTY)&ExtensionProp,
                        sizeof(ExtensionProp),
                        NULL,
                        0,
                        &ulBytesReturned);

                    if (hr == S_OK) {
#    if _DEBUG
                        printf("CreateNodeInstance NodeId = %d\n", i);
#    endif
                        m_dwNodeId = i;
                        UvcXuGudi = extensionGuid;
                        return hr;
                    }
                }
                else {
#    if _DEBUG
                    printf("CreateNodeInstance failed - 0x%x, NodeId = %d\n", hr, i);
#    endif
                }
            }
        }
    }

    if (i == numberOfNodes) { // Did not find the node
        SAFE_RELEASE(m_pKsControl);
        m_pKsControl = NULL;
        hr = E_FAIL;
    }

    SAFE_RELEASE(pIksTopologyInfo);
    return hr;
}

HRESULT
UVCXU::FinalConstruct()
{
    if (m_pKsControl == NULL) return E_FAIL;
    return S_OK;
}

HRESULT
UVCXU::get_InfoSize(
    ULONG *pulSize)
{
    HRESULT hr = S_OK;
    ULONG ulBytesReturned;
    KSP_NODE ExtensionProp;

    if (!pulSize || m_pKsControl == NULL) return E_POINTER;

    ExtensionProp.Property.Set = UvcXuGudi;
    ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        NULL,
        0,
        &ulBytesReturned);

    if (hr == HRESULT_FROM_WIN32(ERROR_MORE_DATA)) {
        *pulSize = ulBytesReturned;
        hr = S_OK;
    }

    return hr;
}

HRESULT
UVCXU::get_Info(
    ULONG ulSize,
    BYTE pInfo[])
{
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    if (m_pKsControl == NULL) return E_POINTER;

    ExtensionProp.Property.Set = UvcXuGudi;
    ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pInfo,
        ulSize,
        &ulBytesReturned);

    return hr;
}

HRESULT
UVCXU::get_PropertySize(
    ULONG PropertyId,
    ULONG *pulSize)
{
    HRESULT hr = S_OK;
    ULONG ulBytesReturned;
    KSP_NODE ExtensionProp;

    if (!pulSize || m_pKsControl == NULL) return E_POINTER;

    ExtensionProp.Property.Set = UvcXuGudi;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        NULL,
        0,
        &ulBytesReturned);

    if (hr == HRESULT_FROM_WIN32(ERROR_MORE_DATA)) {
        *pulSize = ulBytesReturned;
        hr = S_OK;
    }

    return hr;
}

HRESULT
UVCXU::get_Property(
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[])
{
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    if (m_pKsControl == NULL) return E_POINTER;

    ExtensionProp.Property.Set = UvcXuGudi;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;
    ExtensionProp.Reserved = 0;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pValue,
        ulSize,
        &ulBytesReturned);

    if (ulSize != ulBytesReturned)
        return E_FAIL;

    return hr;
}

HRESULT
UVCXU::put_Property(
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[])
{
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    if (m_pKsControl == NULL) return E_POINTER;

    ExtensionProp.Property.Set = UvcXuGudi;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;
    ExtensionProp.Reserved = 0;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pValue,
        ulSize,
        &ulBytesReturned);

    return hr;
}

typedef struct
{
    KSPROPERTY_MEMBERSHEADER MembersHeader;
    const VOID *Members;
} KSPROPERTY_MEMBERSLIST, *PKSPROPERTY_MEMBERSLIST;

typedef struct
{
    KSIDENTIFIER PropTypeSet;
    ULONG MembersListCount;
    _Field_size_(MembersListCount)
        const KSPROPERTY_MEMBERSLIST *MembersList;
} KSPROPERTY_VALUES, *PKSPROPERTY_VALUES;

HRESULT
UVCXU::get_PropertyRange(
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pMin[],
    BYTE pMax[],
    BYTE pSteppingDelta[],
    BYTE pDefault[])
{
    if (m_pKsControl == NULL) return E_POINTER;

    // IHV may add code here, current stub just returns S_OK
    HRESULT hr = S_OK;
    return hr;
}

/************************************************************End of UVCXU class******************************************************************/

/********************************************************Implementation of UVCCamera class************************************************/

/*
* Constructor of the class
*/
UVCCamera::UVCCamera()
{
    // initialize COM
    CoInitialize(0);
}

/*
* Destructor of the class
*/
UVCCamera::~UVCCamera()
{
    disconnectDevice();

    // release directshow class instances
    if (pEnumMoniker != NULL)
        pEnumMoniker->Release();
    if (pCreateDevEnum != NULL)
        pCreateDevEnum->Release();

    // finalize COM
    CoUninitialize();
}

void UVCCamera::listDevices(std::vector<std::string> &devs)
{
    int nDevices = 0;
    IBaseFilter *pDeviceFilter = NULL;
    // to select a video input device
    ICreateDevEnum *pCreateDevEnum = NULL;
    IEnumMoniker *pEnumMoniker = NULL;
    IMoniker *pMoniker = NULL;
    ULONG nFetched = 0;
    HRESULT hr;
    // initialize COM
    CoInitialize(0);
    // Create CreateDevEnum to list device
    CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                     IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);

    // Create EnumMoniker to list VideoInputDevice
    pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                          &pEnumMoniker, 0);
    if (pEnumMoniker == NULL) {
        // this will be shown if there is no capture device
        printf("no device\n");
        return;
    }

    // reset EnumMoniker
    pEnumMoniker->Reset();
    devs.clear();

    while (pEnumMoniker->Next(1, &pMoniker, &nFetched) == S_OK) {

        IPropertyBag *pPropertyBag;
        //unique name with suffix
        char devname[256];

        // bind to IPropertyBag
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                                     (void **)&pPropertyBag);

        if (FAILED(hr)) {
            pMoniker->Release();
            continue; // Skip this one, maybe the next one will work.
        }

        VARIANT var;
        var.vt = VT_BSTR;
        hr = pPropertyBag->Read(L"FriendlyName", &var, 0); //DevicePath
        if (SUCCEEDED(hr)) {
            WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, devname, sizeof(devname), 0, 0);
            VariantClear(&var);

            //添加这项记录
            devs.push_back(devname);
        }
        else {
            devs.push_back("Can No Read FriendlyName!");
        }

        // release
        pMoniker->Release();
        pPropertyBag->Release();

        nDevices++;
    }
    // release

    pEnumMoniker->Release();
    pCreateDevEnum->Release();

    // finalize COM
    CoUninitialize();
}

/*
* get moniker enum
* one moniker corresponds to one camera
*/
void UVCCamera::getEnumMoniker()
{
    // Create CreateDevEnum to list device
    CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                     IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);

    // Create EnumMoniker to list VideoInputDevice
    pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                          &pEnumMoniker, 0);
    if (pEnumMoniker == NULL) {
        // this will be shown if there is no capture device
        printf("no device\n");
        return;
    }

    // reset EnumMoniker
    pEnumMoniker->Reset();
}

bool UVCCamera::connectDevice(const std::string &deviceName)
{
    if (deviceName.empty()) {
        return false;
    }

    getEnumMoniker();

    if (pEnumMoniker == NULL)
        return false;
    IMoniker *pMoniker = NULL;
    while (pEnumMoniker->Next(1, &pMoniker, &nFetched) == S_OK) {
        IPropertyBag *pPropertyBag;
        char devname[256];

        // bind to IPropertyBag
        pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropertyBag);

        VARIANT var;
        // get FriendlyName
        var.vt = VT_BSTR;
        pPropertyBag->Read(L"FriendlyName", &var, 0);
        //var.bstrVal是wchar,转换一下
        WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, devname, sizeof(devname), 0, 0);
        VariantClear(&var);

        //如果名字符合(输入的名字包含在实际设备名中)
        if (std::string(devname).find(deviceName) != std::string::npos) {
            pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pDeviceFilter);
            if (pDeviceFilter != NULL) {
                pMoniker->Release();
                pPropertyBag->Release();
                return true;
            }
        }

        // release
        pMoniker->Release();
        pPropertyBag->Release();
    }
    return false;
}

void UVCCamera::disconnectDevice()
{
    //release directshow filter
    if (pDeviceFilter != NULL)
        pDeviceFilter->Release();
    pDeviceFilter = NULL;
}

std::vector<VIDEO_STREAM_CONFIG_CAPS> UVCCamera::getCapabilities()
{
    std::vector<VIDEO_STREAM_CONFIG_CAPS> result;
    if (pDeviceFilter != NULL) {
        IAMStreamConfig *pAMStreamConfg;
        HRESULT hr = E_FAIL;
        //首先在filter上查找Interface
        hr = pDeviceFilter->QueryInterface(IID_IAMStreamConfig, (void **)&pAMStreamConfg);
        if (!SUCCEEDED(hr)) {
            //如果找不到那么遍历filter的Pin来查找interface
            IEnumPins *pEnum = 0;
            if (SUCCEEDED(pDeviceFilter->EnumPins(&pEnum))) {
                // Query every pin for the interface.
                IPin *pPin = 0;
                while (S_OK == pEnum->Next(1, &pPin, 0)) {
                    hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pAMStreamConfg);
                    pPin->Release();
                    if (SUCCEEDED(hr)) {
                        break;
                    }
                }
                pEnum->Release();
            }
        }
        //如果成功的找到了Interface
        if (SUCCEEDED(hr)) {
            int iCount = 0, iSize = 0;
            hr = pAMStreamConfg->GetNumberOfCapabilities(&iCount, &iSize);
            // Check the size to make sure we pass in the correct structure.
            if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
                // Use the video capabilities structure.
                for (int iFormat = 0; iFormat < iCount; iFormat++) {
                    VIDEO_STREAM_CONFIG_CAPS scc; //这个是媒体信息
                    AM_MEDIA_TYPE *pmtConfig;
                    hr = pAMStreamConfg->GetStreamCaps(iFormat, &pmtConfig, (BYTE *)&scc);
                    //pAMStreamConfg->GetFormat(&pmtConfig);
                    if (SUCCEEDED(hr)) {
                        result.push_back(scc);
                        /*   if ((pmtConfig->majortype == MEDIATYPE_Video) &&
                            (pmtConfig->subtype == MEDIASUBTYPE_RGB24) &&
                            (pmtConfig->formattype == FORMAT_VideoInfo) &&
                            (pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
                            (pmtConfig->pbFormat != NULL)) {
                            VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)pmtConfig->pbFormat;
                            pVih->bmiHeader.biWidth = 1280;
                            pVih->bmiHeader.biHeight = 720;
                            pVih->bmiHeader.biSizeImage = DIBSIZE(pVih->bmiHeader);
                            hr = pAMStreamConfg->SetFormat(pmtConfig);
                        }*/
                        DeleteMediaType(pmtConfig);
                    }
                }
            }
        }
    }
    return result;
}

/*
* move pan to left one step
* @pan: (in) step of the panning
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::movePanOneLeft(int pan)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_PAN_RELATIVE, -pan);
}

/*
* move pan to right one step
* @pan: (in) step of the panning
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::movePanOneRight(int pan)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_PAN_RELATIVE, pan);
}

/*
* move tilt to top one step
* @tilt: (in) step of the tilting
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::moveTiltOneTop(int tilt)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_TILT_RELATIVE, tilt);
}

/*
* move tilt to bottom one step
* @tilt: (in) step of the tilting
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::moveTiltOneBottom(int tilt)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_TILT_RELATIVE, -tilt);
}

/*HRESULT angleUpLeft(int pan, int tilt)
{
    HRESULT hr;
    return hr;
}
HRESULT angleUpRight(int pan, int tilt)
{
    HRESULT hr;
    return hr;
}
HRESULT angleDownLeft(int pan, int tilt)
{
    HRESULT hr;
    return hr;
}
HRESULT anglueDownRight(int pan, int tilt)
{
    HRESULT hr;
    return hr;
}*/

/*
* zoom in one step
* @tilt: (in) step of the zooming
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::zoomOneIn(int zoom)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE, zoom);
}

/*
* zoom out one step
* @tilt: (in) step of the zooming
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::zoomOneOut(int zoom)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE, -zoom);
}

/*
* focus in one step
* does work if focus mode is set as Auto
* @tilt: (in) step of the focusing
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::focusOneIn(int focus)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_FOCUS_RELATIVE, focus);
}
/*
* focus out one step
* does work if focus mode is set as Auto
* @tilt: (in) step of the focusing
* @return: HRESULT structure if success returns S_OK
*/
HRESULT UVCCamera::focusOneOut(int focus)
{
    return moveCamera(KSPROPERTY_CAMERACONTROL_FOCUS_RELATIVE, -focus);
}

/*
* change the property of the camera
* @prop: (in) property e.g. KSPROPERTY_CAMERACONTROL_PAN_RELATIVE, KSPROPERTY_CAMERACONTROL_TILT_RELATIVE, KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE ...
* Use KSPROPERTIES for continuous movement
* @return: if success returns S_OK
*/
HRESULT UVCCamera::moveCamera(KSPROPERTY_VIDCAP_CAMERACONTROL prop, int step)
{
    HRESULT hr;
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
        printf("Min %d , Max %d , Step %d", Min, Max, Step);
        if (SUCCEEDED(hr)) {
            hr = pCameraControl->Set(prop, step, CameraControl_Flags_Manual);
        }
        else {
            printf("This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

/*
* move to absolute position
* @pan: pan
* @tilt: tilt
* @zoom: zoom
* @return: if success returns S_OK
* must add min values from pan, tilt, zoom
* the range of the pan, tilt, zoom values are like this -1 t0 1
* but the available properties are like 0 to 255
*/
HRESULT UVCCamera::moveTo(int pan, int tilt, int zoom)
{
    HRESULT hr;
    hr = stopMoving();
    hr = stopZooming();
    hr = stopFocusing();
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(CameraControl_Pan, &Min, &Max, &Step, &Default, &Flags);
        long panFlag = Flags;
        pan += Min;
        if (pan < Min)
            pan = Min;
        if (pan > Max)
            pan = Max;
        hr = pCameraControl->GetRange(CameraControl_Tilt, &Min, &Max, &Step, &Default, &Flags);
        long tiltFlag = Flags;
        tilt += Min;
        if (tilt < Min)
            tilt = Min;
        if (tilt > Max)
            tilt = Max;
        hr = pCameraControl->GetRange(CameraControl_Zoom, &Min, &Max, &Step, &Default, &Flags);
        long zoomFlag = Flags;
        zoom += Min;
        if (zoom < Min)
            zoom = Min;
        if (zoom > Max)
            zoom = Max;

        if (SUCCEEDED(hr)) {
            //use CameraControl_Pan, Tilt, Zoom for absolute movement
            hr = pCameraControl->Set(CameraControl_Pan, pan, CameraControl_Flags_Manual);

            hr = pCameraControl->Set(CameraControl_Tilt, tilt, CameraControl_Flags_Manual);

            hr = pCameraControl->Set(CameraControl_Zoom, zoom, CameraControl_Flags_Manual);
        }
        else {
            printf("This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

//move home
HRESULT UVCCamera::moveHome()
{
    HRESULT hr;
    hr = stopMoving();
    hr = stopZooming();
    hr = stopFocusing();
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(CameraControl_Pan, &Min, &Max, &Step, &Default, &Flags);
        long panFlag = Flags;
        long panDefault = Default;
        hr = pCameraControl->GetRange(CameraControl_Tilt, &Min, &Max, &Step, &Default, &Flags);
        long tiltFlag = Flags;
        long tiltDefault = Default;
        hr = pCameraControl->GetRange(CameraControl_Zoom, &Min, &Max, &Step, &Default, &Flags);
        long zoomFlag = Flags;
        long zoomDefault = Default;
        hr = pCameraControl->Get(CameraControl_Focus, &Val, &Flags);
        long focusFlag = Flags;
        long focusVal = Val;
        if (SUCCEEDED(hr)) {
            hr = pCameraControl->Set(CameraControl_Pan, panDefault, CameraControl_Flags_Manual);

            hr = pCameraControl->Set(CameraControl_Tilt, tiltDefault, CameraControl_Flags_Manual);

            hr = pCameraControl->Set(CameraControl_Zoom, zoomDefault, CameraControl_Flags_Manual);
            if (focusFlag != CameraControl_Flags_Auto)
                hr = pCameraControl->Set(CameraControl_Focus, Val, CameraControl_Flags_Auto);
        }
        else {
            printf("This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

bool UVCCamera::getAutoFocus()
{
    return getAuto(KSPROPERTY_CAMERACONTROL_FOCUS);
}

bool UVCCamera::getAutoExposure()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_EXPOSURE);
    return result.isAuto();
}

bool UVCCamera::getLowLightCompensation()
{
    //低亮度补偿的val为1表示使能,为0表示关闭
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY);
    if (result.Val) {
        return true;
    }
    else {
        return false;
    }
}

UVCProp UVCCamera::getPan()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_PAN);
    result.name = "PAN";
    return result;
}

UVCProp UVCCamera::getTilt()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_TILT);
    result.name = "TILT";
    return result;
}

UVCProp UVCCamera::getZoom()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_ZOOM);
    result.name = "ZOOM";
    return result;
}

UVCProp UVCCamera::getBrightness()
{
    UVCProp result = getVal(VideoProcAmp_Brightness);
    result.name = "Brightness";
    return result;
}

UVCProp UVCCamera::getContrast()
{
    UVCProp result = getVal(VideoProcAmp_Contrast);
    result.name = "Contrast";
    return result;
}

UVCProp UVCCamera::getHue()
{
    UVCProp result = getVal(VideoProcAmp_Hue);
    result.name = "Hue";
    return result;
}
UVCProp UVCCamera::getSaturation()
{
    UVCProp result = getVal(VideoProcAmp_Saturation);
    result.name = "Saturation";
    return result;
}

UVCProp UVCCamera::getSharpness()
{
    UVCProp result = getVal(VideoProcAmp_Sharpness);
    result.name = "Sharpness";
    return result;
}

UVCProp UVCCamera::getGamma()
{
    UVCProp result = getVal(VideoProcAmp_Gamma);
    result.name = "Gamma";
    return result;
}

UVCProp UVCCamera::getColorEnable()
{
    UVCProp result = getVal(VideoProcAmp_ColorEnable);
    result.name = "ColorEnable";
    return result;
}

UVCProp UVCCamera::getWhiteBalance()
{
    UVCProp result = getVal(VideoProcAmp_WhiteBalance);
    result.name = "WhiteBalance";
    return result;
}

UVCProp UVCCamera::getBacklightCompensation()
{
    UVCProp result = getVal(VideoProcAmp_BacklightCompensation);
    result.name = "BacklightCompensation";
    return result;
}

UVCProp UVCCamera::getGain()
{
    UVCProp result = getVal(VideoProcAmp_Gain);
    result.name = "Gain";
    return result;
}

UVCProp UVCCamera::getFocus()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_FOCUS);
    result.name = "Focus";
    return result;
}

UVCProp UVCCamera::getExposure()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_EXPOSURE);
    result.name = "Exposure";
    return result;
}

UVCProp UVCCamera::getLowLightCompensationProp()
{
    UVCProp result = getVal(KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY);
    result.name = "LowLightCompensation";
    return result;
}

std::vector<UVCProp> UVCCamera::getAllProp()
{
    std::vector<UVCProp> result;

    result.push_back(getBrightness());
    result.push_back(getContrast());
    result.push_back(getHue());
    result.push_back(getSaturation());
    result.push_back(getSharpness());
    result.push_back(getGamma());
    result.push_back(getColorEnable());
    result.push_back(getWhiteBalance());
    result.push_back(getBacklightCompensation());
    result.push_back(getGain());
    result.push_back(getFocus());
    result.push_back(getExposure());
    result.push_back(getLowLightCompensationProp());

    return result;
}

/*
* set fucus mode
* @af: if true set as Auto otherwise set as Manual
* @return: if success returns S_OK
*/
HRESULT UVCCamera::setAutoFocus(bool af)
{
    stopFocusing();
    return setAuto(KSPROPERTY_CAMERACONTROL_FOCUS, af);
}

HRESULT UVCCamera::setAutoExposure(bool ae)
{
    stopExposuring();
    return setAuto(KSPROPERTY_CAMERACONTROL_EXPOSURE, ae);
}

HRESULT UVCCamera::setLowLightCompensation(bool al)
{
    // 关于低亮度补偿的设置
    // https://sourceforge.net/p/directshownet/discussion/460697/thread/562ef6cf/
    // KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY maps to the Low-Light Compensation check box on the USB Video Class property page.
    // https://docs.microsoft.com/zh-cn/windows-hardware/drivers/stream/ksproperty-cameracontrol-auto-exposure-priority?redirectedfrom=MSDN
    // 它实际上是这个枚举: KSPROPERTY_VIDCAP_CAMERACONTROL
    //stopControling(KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY);
    if (al)
        return setVal(KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, 1);
    else
        return setVal(KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY, 0);
}

HRESULT UVCCamera::setBrightness(long val)
{
    return setVal(VideoProcAmp_Brightness, val);
}

HRESULT UVCCamera::setContrast(long val)
{
    return setVal(VideoProcAmp_Contrast, val);
}

HRESULT UVCCamera::setHue(long val)
{
    return setVal(VideoProcAmp_Hue, val);
}

HRESULT UVCCamera::setSaturation(long val)
{
    return setVal(VideoProcAmp_Saturation, val);
}

HRESULT UVCCamera::setSharpness(long val)
{
    return setVal(VideoProcAmp_Sharpness, val);
}

HRESULT UVCCamera::setGamma(long val)
{
    return setVal(VideoProcAmp_Gamma, val);
}

HRESULT UVCCamera::setColorEnable(long val)
{
    return setVal(VideoProcAmp_ColorEnable, val);
}

HRESULT UVCCamera::setWhiteBalance(long val)
{
    return setVal(VideoProcAmp_WhiteBalance, val);
}

HRESULT UVCCamera::setBacklightCompensation(long val)
{
    return setVal(VideoProcAmp_BacklightCompensation, val);
}

HRESULT UVCCamera::setGain(long val)
{
    return setVal(VideoProcAmp_Gain, val);
}

HRESULT UVCCamera::setFocus(long val)
{
    return setVal(KSPROPERTY_CAMERACONTROL_FOCUS, val);
}

HRESULT UVCCamera::setExposure(long val)
{
    return setVal(KSPROPERTY_CAMERACONTROL_EXPOSURE, val);
}

bool UVCCamera::getAuto(KSPROPERTY_VIDCAP_CAMERACONTROL prop)
{
    HRESULT hr;
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        if (pCameraControl != NULL)
            pCameraControl->Release();
        printf("UVCCamera.getAuto():This device does not support IAMCameraControl\n");
        return false;
    }
    else {
        long Flags, Val;

        // Get the range and default values
        hr = pCameraControl->Get(prop, &Val, &Flags);
        if (pCameraControl != NULL)
            pCameraControl->Release();
        if (SUCCEEDED(hr)) {
            if (Flags == CameraControl_Flags_Auto)
                return true;
            else
                return false;
        }
        else {
            printf("UVCCamera.getAuto():This device does not support PTZControl\n");
            return false;
        }
    }
}

UVCProp UVCCamera::getVal(KSPROPERTY_VIDCAP_CAMERACONTROL prop)
{
    UVCProp result;
    IAMCameraControl *pCameraControl = 0;
    result.hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(result.hr)) {
        // The device does not support IAMCameraControl
        if (pCameraControl != NULL)
            pCameraControl->Release();
        printf("UVCCamera.getVal():This device does not support IAMCameraControl\n");
        return result;
    }
    else {
        //long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        result.hr = pCameraControl->GetRange(prop, &result.Min, &result.Max, &result.Step, &result.Default, &result.Flags);
        result.hr = pCameraControl->Get(prop, &result.Val, &result.Flags);
        if (pCameraControl != NULL)
            pCameraControl->Release();
        if (SUCCEEDED(result.hr)) {
            return result; //这里原来是Val-Min,改成直接返回就行
        }
        else {
            printf("UVCCamera.getVal():This device does not support Prop=%d \n", prop);
            return result;
        }
    }
}

UVCProp UVCCamera::getVal(VideoProcAmpProperty prop)
{
    UVCProp result;
    HRESULT &hr = result.hr;
    IAMVideoProcAmp *pProcAmp = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        if (pProcAmp != NULL)
            pProcAmp->Release();
        printf("UVCCamera.getVal():This device does not support IAMVideoProcAmp\n");
        return result;
    }
    else {
        //long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pProcAmp->GetRange(prop, &result.Min, &result.Max, &result.Step, &result.Default, &result.Flags);
        hr = pProcAmp->Get(prop, &result.Val, &result.Flags);
        if (pProcAmp != NULL)
            pProcAmp->Release();
        if (SUCCEEDED(hr)) {
            return result; //这里原来是Val-Min,改成直接返回就行
        }
        else {
            printf("UVCCamera.getVal():This device does not support PTZControl\n");
            return result;
        }
    }
}

HRESULT UVCCamera::setAuto(KSPROPERTY_VIDCAP_CAMERACONTROL prop, bool isAuto)
{
    HRESULT hr;
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("UVCCamera.setAuto():This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr)) {
            hr = pCameraControl->Get(prop, &Val, &Flags);
            if (isAuto)
                hr = pCameraControl->Set(prop, Val, CameraControl_Flags_Auto);
            else
                hr = pCameraControl->Set(prop, Val, CameraControl_Flags_Manual);
        }
        else {
            printf("UVCCamera.setAuto():This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

HRESULT UVCCamera::setVal(KSPROPERTY_VIDCAP_CAMERACONTROL prop, long val)
{
    HRESULT hr;
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("UVCCamera.setVal():This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr)) {
            hr = pCameraControl->Get(prop, &Val, &Flags);
            hr = pCameraControl->Set(prop, val, Flags);
        }
        else {
            printf("UVCCamera.setVal():This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

HRESULT UVCCamera::setVal(VideoProcAmpProperty prop, long val)
{
    HRESULT hr;
    IAMVideoProcAmp *pProcAmp = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("UVCCamera.setVal():This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pProcAmp->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr)) {
            hr = pProcAmp->Get(prop, &Val, &Flags);
            hr = pProcAmp->Set(prop, val, Flags);
        }
        else {
            printf("UVCCamera.setVal():This device does not support PTZControl\n");
        }
    }
    if (pProcAmp != NULL)
        pProcAmp->Release();
    return hr;
}

HRESULT UVCCamera::stopMoving()
{
    stopControling(KSPROPERTY_CAMERACONTROL_PAN_RELATIVE);
    return stopControling(KSPROPERTY_CAMERACONTROL_TILT_RELATIVE);
}
HRESULT UVCCamera::stopZooming()
{
    return stopControling(KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE);
}
HRESULT UVCCamera::stopFocusing()
{
    return stopControling(KSPROPERTY_CAMERACONTROL_FOCUS_RELATIVE);
}

HRESULT UVCCamera::stopExposuring()
{
    return stopControling(KSPROPERTY_CAMERACONTROL_EXPOSURE_RELATIVE);
}

HRESULT UVCCamera::stopControling(KSPROPERTY_VIDCAP_CAMERACONTROL prop)
{
    HRESULT hr;
    IAMCameraControl *pCameraControl = 0;
    hr = pDeviceFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl);
    if (FAILED(hr)) {
        // The device does not support IAMCameraControl
        printf("UVCCamera.stopControling():This device does not support IAMCameraControl\n");
    }
    else {
        long Min, Max, Step, Default, Flags, Val;

        // Get the range and default values
        hr = pCameraControl->GetRange(prop, &Min, &Max, &Step, &Default, &Flags);
        if (SUCCEEDED(hr)) {
            printf("UVCCamera.stopControling():Min %d , Max %d , Step %d\n", Min, Max, Step);
            hr = pCameraControl->Set(prop, 0, KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE);
        }
        else {
            printf("UVCCamera.stopControling():This device does not support PTZControl\n");
        }
    }
    if (pCameraControl != NULL)
        pCameraControl->Release();
    return hr;
}

/*OSD menu tool*/
HRESULT UVCCamera::osdMenuOpenClose()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_OPEN_CLOSE, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuEnter()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_OK, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuBack()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_BACK, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuUp()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_UP, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuDown()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_DOWN, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuLeft()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_LEFT, ulUvcRedSize, (BYTE *)&nPos);
}
HRESULT UVCCamera::osdMenuRight()
{
    int nPos = 0;
    return uvcxu.put_Property(UVC_1702C_XU_PLUG_CTRL_RIGHT, ulUvcRedSize, (BYTE *)&nPos);
}

HRESULT UVCCamera::checkOSDMenu()
{
    HRESULT hr = E_FAIL;

    if (pDeviceFilter == NULL)
        return E_FAIL;

    hr = uvcxu.QueryUvcXuInterface(pDeviceFilter, PROPSETID_XU_PLUG_IN_1700U, 0);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    hr = E_FAIL;

    if (uvcxu.get_PropertySize(UVC_1702C_XU_PLUG_CTRL_OPEN_CLOSE, &ulUvcRedSize) == S_OK) {
        BYTE *pbPropertyValue;
        pbPropertyValue = new BYTE[ulUvcRedSize];
        if (pbPropertyValue) {
            if (uvcxu.get_Property(UVC_1702C_XU_PLUG_CTRL_OPEN_CLOSE, ulUvcRedSize, (BYTE *)pbPropertyValue) == S_OK) {
                hr = S_OK;
            }
        }
        delete pbPropertyValue;
    }

    return hr;
}
/********************************************************End of UVCCamera class************************************************************/

} // namespace dxlib

#endif