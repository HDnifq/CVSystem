/*
* 使用的dshow的UVC相机控制.
* 改写自:https://github.com/sky19938470/uvcamera-windows-project
*/
#ifndef __UVCCAMERALIBRARY_H__
#define __UVCCAMERALIBRARY_H__

#if defined(_WIN32) || defined(_WIN64)

#    include <windows.h>
#    include <vidcap.h>
#    include <dshow.h>
#    include <ks.h>
#    include <ksproxy.h>
#    include <ksmedia.h>
#    include <uuids.h>
#    include <stdio.h>

#    include <vector>
#    include <string>

namespace dxlib {

DEFINE_GUIDSTRUCT("A29E7641-DE04-47E3-8B2B-F4341AFF003B", PROPSETID_XU_H264);
#    define PROPSETID_XU_H264 DEFINE_GUIDNAMED(PROPSETID_XU_H264)

DEFINE_GUIDSTRUCT("3C16A808-701C-49C7-AF98-2EC6C125B963", PROPSETID_XU_PLUG_IN);
#    define PROPSETID_XU_PLUG_IN DEFINE_GUIDNAMED(PROPSETID_XU_PLUG_IN)

//DEFINE_GUIDSTRUCT("A7A6A934-3A4A-4351-85F4-042EF2CDCAAB", PROPSETID_XU_PLUG_IN_1700U);
//#define PROPSETID_XU_PLUG_IN_1700U DEFINE_GUIDNAMED(PROPSETID_XU_PLUG_IN_1700U)
DEFINE_GUIDSTRUCT("661DCD8C-ADB0-B344-8CB4-D335115FC18C", PROPSETID_XU_PLUG_IN_1700U);
#    define PROPSETID_XU_PLUG_IN_1700U DEFINE_GUIDNAMED(PROPSETID_XU_PLUG_IN_1700U)

#    ifndef ASSERT
#        define ASSERT(_x_) ((void)0)
#    endif

#    ifndef SAFE_RELEASE
#        define SAFE_RELEASE(x) \
            if (x) {            \
                x->Release();   \
                x = NULL;       \
            }
#    endif

typedef enum
{
    UVC_XU_PLUG_IN_FLIP_CTRL = 1,
    UVC_XU_PLUG_IN_OSD_RESTTING_CTRL,
    UVC_XU_PLUG_IN_SAVE_STTING_CTRL,
    UVC_XU_PLUG_IN_PRESET_CTRL,
    UVC_XU_PLUG_IN_SLEEP_CTRL,
    UVC_XU_PLUG_IN_RESET_PAN_TILT,
    UVC_XU_PLUG_IN_RED,
    UVC_XU_PLUG_IN_GREEN,
    UVC_XU_PLUG_IN_BLUE,
    UVC_XU_PLUG_IN_2D,
} uvc_xu_plug_in_id_t;

typedef enum
{
    UVC_1700U_XU_PLUG_IN_LDC = 3,
    UVC_1700U_XU_PLUG_IN_FOCUSA,
    UVC_1700U_XU_PLUG_IN_ZOOMLIMT,
    UVC_1700U_XU_PLUG_STATUS = 8,
    UVC_1700U_XU_PLUG_INFO,

} uvc_1700u_xu_plug_in_id_t;

typedef enum
{
    UVC_1702C_XU_PLUG_CTRL_UP = 1,
    UVC_1702C_XU_PLUG_CTRL_DOWN,
    UVC_1702C_XU_PLUG_CTRL_LEFT,
    UVC_1702C_XU_PLUG_CTRL_RIGHT,
    UVC_1702C_XU_PLUG_CTRL_OK,
    UVC_1702C_XU_PLUG_CTRL_BACK,
    UVC_1702C_XU_PLUG_CTRL_OPEN_CLOSE,

} uvc_17002c_xu_plug_in_id_t;

class UVCXU // : public IKsNodeControl
{
  protected:
    GUID UvcXuGudi;
    ULONG m_dwNodeId;
    IKsControl *m_pKsControl = NULL;

    //IKsPropertySet *m_pIKsPropertySet = NULL;

  public:
    UVCXU();
    virtual ~UVCXU();

    HRESULT
    QueryUvcXuInterface(
        IBaseFilter *pDeviceFilter,
        GUID extensionGuid,
        DWORD FirstNodeId);

    HRESULT FinalConstruct();

    HRESULT get_Info(
        ULONG ulSize,
        BYTE pInfo[]);
    HRESULT get_InfoSize(
        ULONG *pulSize);
    HRESULT get_PropertySize(
        ULONG PropertyId,
        ULONG *pulSize);
    HRESULT get_Property(
        ULONG PropertyId,
        ULONG ulSize,
        BYTE pValue[]);
    HRESULT put_Property(
        ULONG PropertyId,
        ULONG ulSize,
        BYTE pValue[]);
    HRESULT get_PropertyRange(
        ULONG PropertyId,
        ULONG ulSize,
        BYTE pMin[],
        BYTE pMax[],
        BYTE pSteppingDelta[],
        BYTE pDefault[]);
};

/**
 * 一项UVC属性.
 *
 * @author daixian
 * @date 2020/6/28
 */
class UVCProp
{
  public:
    std::string name;
    long Min = 0;
    long Max = 0;
    long Step = 0;
    long Default = 0;
    long Flags = 0;
    long Val = 0;
    HRESULT hr = E_FAIL;

    bool isAuto()
    {
        if (Flags == CameraControl_Flags_Auto) {
            return true;
        }
        return false;
    }
};

class UVCCamera
{
  public:
    UVCCamera();
    ~UVCCamera();

    /**
     * 静态方法,列出设备.
     *
     * @author daixian
     * @date 2020/6/29
     *
     * @param [out] devs The devs.
     */
    static void listDevices(std::vector<std::string> &devs);

    /**
     * 连接一个摄像头设备.
     *
     * @author daixian
     * @date 2020/6/28
     *
     * @param  deviceName 设备名称.
     *
     * @returns 连接成功返回true.
     */
    bool connectDevice(const std::string &deviceName);

    /**
     * 关闭连接.
     *
     * @author daixian
     * @date 2020/6/29
     */
    void disconnectDevice();

    /**
     * 得到相机支持的分辨率等信息.
     *
     * @author daixian
     * @date 2020/6/29
     *
     * @returns The capabilities.
     */
    std::vector<VIDEO_STREAM_CONFIG_CAPS> getCapabilities();

    //camera control functions
    //pan,tilt
    HRESULT movePanOneLeft(int pan);
    HRESULT movePanOneRight(int pan);
    HRESULT moveTiltOneTop(int tilt);
    HRESULT moveTiltOneBottom(int tilt);
    /*HRESULT angleUpLeft(int pan, int tilt);
	HRESULT angleUpRight(int pan, int tilt);
	HRESULT angleDownLeft(int pan, int tilt);
	HRESULT anglueDownRight(int pan, int tilt);*/
    //zoom
    HRESULT zoomOneIn(int zoom);
    HRESULT zoomOneOut(int zoom);
    //focus
    HRESULT focusOneIn(int focus);
    HRESULT focusOneOut(int focus);
    //home
    HRESULT moveHome();
    //move to absolute position
    HRESULT moveTo(int pan, int tilt, int zoom);

    //stop moving, zooming, focusing, 曝光
    HRESULT stopMoving();
    HRESULT stopZooming();
    HRESULT stopFocusing();
    HRESULT stopExposuring();

    /********* 相机属性(VideoProcAmpProperty和CameraControl) *********/
    bool getAutoFocus();
    bool getAutoExposure();
    bool getLowLightCompensation();

    UVCProp getPan();
    UVCProp getTilt();
    UVCProp getZoom();

    UVCProp getBrightness();
    UVCProp getContrast();
    UVCProp getHue();
    UVCProp getSaturation();
    UVCProp getSharpness();
    UVCProp getGamma();
    UVCProp getColorEnable();
    UVCProp getWhiteBalance();
    UVCProp getBacklightCompensation();
    UVCProp getGain();

    UVCProp getFocus();
    UVCProp getExposure();

    //设置 (Auto/Manual)
    HRESULT setAutoFocus(bool af);
    HRESULT setAutoExposure(bool ae);
    HRESULT setLowLightCompensation(bool al);

    HRESULT setBrightness(long val);
    HRESULT setContrast(long val);
    HRESULT setHue(long val);
    HRESULT setSaturation(long val);
    HRESULT setSharpness(long val);
    HRESULT setGamma(long val);
    HRESULT setColorEnable(long val);
    HRESULT setWhiteBalance(long val);
    HRESULT setBacklightCompensation(long val);
    HRESULT setGain(long val);

    HRESULT setFocus(long val);
    HRESULT setExposure(long val);

    /*OSD menu tool 这个可能是他们自定义的功能*/
    HRESULT osdMenuOpenClose();
    HRESULT osdMenuEnter();
    HRESULT osdMenuBack();
    HRESULT osdMenuUp();
    HRESULT osdMenuDown();
    HRESULT osdMenuLeft();
    HRESULT osdMenuRight();

    //This function must be called after connection to use other functions for osd menu
    HRESULT checkOSDMenu(); //check if the uvc camera supports osd menu

  private:
    //base directshow filter
    IBaseFilter *pDeviceFilter = NULL;

    // to select a video input device
    ICreateDevEnum *pCreateDevEnum = NULL;
    IEnumMoniker *pEnumMoniker = NULL;

    ULONG nFetched = 0;
    void getEnumMoniker();

    //change the property of camera according to @prop
    HRESULT moveCamera(KSPROPERTY_VIDCAP_CAMERACONTROL prop, int step);
    //stop change of the property
    HRESULT stopControling(KSPROPERTY_VIDCAP_CAMERACONTROL prop);

    //get Auto/Manual status of property
    bool getAuto(KSPROPERTY_VIDCAP_CAMERACONTROL prop);

    //设置auto
    HRESULT setAuto(KSPROPERTY_VIDCAP_CAMERACONTROL prop, bool isAuto);

    //get value of the property
    UVCProp getVal(KSPROPERTY_VIDCAP_CAMERACONTROL prop);
    UVCProp getVal(VideoProcAmpProperty prop);

    //设置属性值
    HRESULT setVal(KSPROPERTY_VIDCAP_CAMERACONTROL prop, long val);
    HRESULT setVal(VideoProcAmpProperty prop, long val);

    /*OSD Menu 这个可能是他们自定义的功能*/
    UVCXU uvcxu;
    ULONG ulUvcRedSize = 0;
    ULONG ulUvcGreenSize = 0;
    ULONG ulUvcBlueSize = 0;
    ULONG ulUvc2dSize = 0;
};

} // namespace dxlib

#endif
#endif
