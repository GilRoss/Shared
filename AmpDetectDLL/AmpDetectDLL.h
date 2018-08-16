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
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedRunningFlg(int nSiteIdx);
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedPausedFlg(int nSiteIdx);
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedCaptureCameraImageFlg(int nSiteIdx);
extern "C" bool AMPDETECTLIBRARY_API AD_GetCachedTempStableFlg(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedCameraIdx(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStableTimer(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedSegmentIdx(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedCycleNum(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStepIdx(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedRunTimer(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedStepTimer(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedHoldTimer(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedTemperature(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedNumOpticsRecs(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_GetCachedNumThermalRecs(int nSiteIdx);

//Running PCR.
extern "C" int	AMPDETECTLIBRARY_API AD_SetPcrProtocol(int nSiteIdx, LPCSTR sProtName);
extern "C" int  AMPDETECTLIBRARY_API AD_StartRun(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_StopRun(int nSiteIdx);
extern "C" int  AMPDETECTLIBRARY_API AD_PauseRun(int nSiteIdx);

//Optics routines.
extern "C" int	AMPDETECTLIBRARY_API AD_SetOpticsLed(int nSiteIdx, int nLedIdx, int nLedIntensity, int nLedDuration_us);
extern "C" int	AMPDETECTLIBRARY_API AD_GetDiodeValue(int nSiteIdx, int nDiodeIdx, int* pDiodeValue);
extern "C" int	AMPDETECTLIBRARY_API AD_ReadOptics(int nSiteIdx, int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* pDiodeValue);
extern "C" int	AMPDETECTLIBRARY_API AD_UpdateOpticalRecCache(int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecTimeTag(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecCycleNum(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecLedIdx(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDetectorIdx(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDarkRead(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecIlluminatedRead(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefDarkRead(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefIlluminatedRead(int nSiteIdx, int nIdx);

//Thermal routines.
extern "C" int	AMPDETECTLIBRARY_API AD_SetPidParams(int nSiteIdx, PidType nPidType, int nKp, int nKi, int nKd, int nSlope_m, int nYIntercept_m);
extern "C" int	AMPDETECTLIBRARY_API AD_GetPidParams(int nSiteIdx, PidType nPidType, int* pKp, int* pKi, int* pKd, int* pSlope_m, int* pYIntercept_m);
extern "C" int	AMPDETECTLIBRARY_API AD_UpdateThermalRecCache(int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecTimeTag(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan1(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan2(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan3(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan4(int nSiteIdx, int nIdx);
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecCurrent(int nSiteIdx, int nIdx);

/////////////////////////////////////////////////////////////////////////////////////////////////
ErrCode	MsgTransaction(int nSiteIdx, HostMsg& request, HostMsg* pResponse);
ErrCode	SimpleTransaction(int nSiteIdx, uint32_t nMsgId);
void	ErrorExit();
