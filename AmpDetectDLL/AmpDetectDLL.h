#pragma once


#ifdef AMPDETECTDLL_EXPORTS
#define AMPDETECTLIBRARY_API __declspec(dllexport)
#else
#define AMPDETECTLIBRARY_API __declspec(dllimport)
#endif

#include "HostMessages.h"

//Initialization routines.
extern "C" int	AMPDETECTLIBRARY_API AD_Initialize(int nNumExpectedSites, int nFirstSiteId);
extern "C" int	AMPDETECTLIBRARY_API AD_Uninitialize();

//General routines.
extern "C" bool AMPDETECTLIBRARY_API AD_IsConnected(int nSiteIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetNumExpectedSites();
extern "C" int	AMPDETECTLIBRARY_API AD_SetTemperatureSetpoint(int nSiteIdx, int nSetpoint_mC);

// Update this DLLs cache of the system status. All routines in this section return values from this cache.
extern "C" int  AMPDETECTLIBRARY_API AD_UpdateSysStatusCache(int nSiteIdx);
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedRunningFlg();
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedPausedFlg();
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedCaptureCameraImageFlg();
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedTempStableFlg();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedCameraIdx();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStableTimer();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedSegmentIdx();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedCycleNum();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStepIdx();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedRunTimer();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStepTimer();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedHoldTimer();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedTemperature();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedNumOpticsRecs();
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedNumThermalRecs();

//Running PCR.
extern "C" int	AMPDETECTLIBRARY_API AD_SetPcrProtocol(int nSiteIdx, LPCTSTR sProtName);
extern "C" int  AMPDETECTLIBRARY_API AD_StartRun(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_StopRun(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_PauseRun(int nSiteIdx);

//Optics routines.
extern "C" int	AMPDETECTLIBRARY_API AD_SetOpticsLed(int nSiteIdx, int nLedIdx, int nLedIntensity, int nLedDuration_us);
extern "C" int	AMPDETECTLIBRARY_API AD_GetDiodeValue(int nSiteIdx, int nDiodeIdx, int* pDiodeValue);
extern "C" int	AMPDETECTLIBRARY_API AD_ReadOptics(int nSiteIdx, int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* pDiodeValue);
extern "C" int	AMPDETECTLIBRARY_API AD_UpdateOpticalRecCache(int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecTimeTag(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecCycleNum(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecLedIdx(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDetectorIdx(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDarkRead(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecIlluminatedRead(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefDarkRead(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefIlluminatedRead(int nIdx);

//Thermal routines.
extern "C" int	AMPDETECTLIBRARY_API AD_SetPidParams(int nSiteIdx, PidType nPidType, int nKp, int nKi, int nKd, int nSlope_m, int nYIntercept_m);
extern "C" int	AMPDETECTLIBRARY_API AD_GetPidParams(int nSiteIdx, PidType nPidType, int* pKp, int* pKi, int* pKd, int* pSlope_m, int* pYIntercept_m);
extern "C" int	AMPDETECTLIBRARY_API AD_UpdateThermalRecCache(int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecTimeTag(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan1(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan2(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan3(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan4(int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecCurrent(int nIdx);

/////////////////////////////////////////////////////////////////////////////////////////////////
ErrCode	MsgTransaction(int nSiteIdx, HostMsg& request, HostMsg* pResponse);
ErrCode	SimpleTransaction(int nSiteIdx, uint32_t nMsgId);
void	ErrorExit();
