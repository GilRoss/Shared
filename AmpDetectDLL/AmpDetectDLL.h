#pragma once

#include "HostMessages.h"


#ifdef AMPDETECTDLL_EXPORTS
#define AMPDETECTLIBRARY_API __declspec(dllexport)
#else
#define AMPDETECTLIBRARY_API __declspec(dllimport)
#endif

uint32_t MsgTransaction(HostMsg& request, HostMsg* pResponse);

// Open device communications port.
extern "C" AMPDETECTLIBRARY_API int OpenComm(const char* pPortName);

// Close device communications port.
extern "C" AMPDETECTLIBRARY_API int CloseComm();

// Determine if device communications port is open.
extern "C" AMPDETECTLIBRARY_API bool IsPortOpenFlg();

// Set LED state.
extern "C" AMPDETECTLIBRARY_API int SetOpticsLed(int nSiteIdx, int nLedIdx, int nLedIntensity, int nLedDuration_us);

// Get Diode value.
extern "C" AMPDETECTLIBRARY_API int GetDiodeValue(int nDiodeIdx, int* data);

// Read Optics.
extern "C" AMPDETECTLIBRARY_API int ReadOptics(int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* data);
