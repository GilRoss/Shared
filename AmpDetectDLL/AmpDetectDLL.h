#pragma once

#include "HostMessages.h"


#ifdef AMPDETECTDLL_EXPORTS
#define AMPDETECTLIBRARY_API __declspec(dllexport)
#else
#define AMPDETECTLIBRARY_API __declspec(dllimport)
#endif

enum DllErrors : int
{
	kNoError = 0,
	kSiteNotConnected = 1000
};

uint32_t MsgTransaction(HostMsg& request, HostMsg* pResponse);

// Open device communications port.
extern "C" int AMPDETECTLIBRARY_API OpenComm(const char* pPortName);

// Close device communications port.
extern "C" int AMPDETECTLIBRARY_API CloseComm();

// Determine if device communications port is open.
extern "C" bool AMPDETECTLIBRARY_API IsPortOpenFlg();

// Set LED state.
extern "C" int AMPDETECTLIBRARY_API SetOpticsLed(int nSiteIdx, int nLedIdx, int nLedIntensity, int nLedDuration_us);

// Get Diode value.
extern "C" int AMPDETECTLIBRARY_API GetDiodeValue(int nDiodeIdx, int* data);

// Read Optics.
extern "C" int AMPDETECTLIBRARY_API ReadOptics(int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* data);
