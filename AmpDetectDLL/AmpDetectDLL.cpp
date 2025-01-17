// AmpDetectDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include "AmpDetectDLL.h"
#include "SysStatus.h"
#include "PcrProtocol.h"


/////////////////////////////////////////////////////////////////////////////////
enum { kMaxADSites = 100 };


/////////////////////////////////////////////////////////////////////////////////
bool					_bInitialized = false;
HANDLE					_arSiteHdls[kMaxADSites];
DCB						_dcbSerialParams = { 0 };
COMMTIMEOUTS			_timeouts = { 0 };
SysStatus				_arSysStatus[kMaxADSites];
int						_nNumExpectedSites = 0;
int						_nFirstSiteId;
std::vector<OpticsRec>	_arOpticalRecCache[kMaxADSites];
std::vector<ThermalRec>	_arThermalRecCache[kMaxADSites];

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_Initialize(int nFirstSiteId, int nNumExpectedSites)
{
	_nFirstSiteId = nFirstSiteId;
	_nNumExpectedSites = nNumExpectedSites;
	for (int i = 0; i < kMaxADSites; i++)
		_arSiteHdls[i] = nullptr;
	AD_Uninitialize();

	ErrCode nErrCode = ErrCode::kNoError;
	for (int nSiteIdx = 0; nSiteIdx < _nNumExpectedSites; nSiteIdx++)
	{
		std::string stemp("\\\\.\\COM" + std::to_string(nFirstSiteId + nSiteIdx));
		std::wstring sPortName = std::wstring(stemp.begin(), stemp.end());

		// Open the highest available serial port number
		_arSiteHdls[nSiteIdx] = CreateFile(
			sPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (_arSiteHdls[nSiteIdx] == INVALID_HANDLE_VALUE)
		{
			ErrorExit();
			_arSiteHdls[nSiteIdx] = nullptr;
			nErrCode = ErrCode::kSiteNotConnectedErr;
		}
		else
		{
			_dcbSerialParams = { 0 };
			_dcbSerialParams.DCBlength = sizeof(DCB);
			GetCommState(_arSiteHdls[nSiteIdx], &_dcbSerialParams);

			_dcbSerialParams = { 0 };
			_dcbSerialParams.DCBlength = sizeof(DCB);
			_dcbSerialParams.BaudRate = CBR_38400;
			_dcbSerialParams.ByteSize = 8;
			_dcbSerialParams.StopBits = ONESTOPBIT;
			_dcbSerialParams.Parity = NOPARITY;
			if (SetCommState(_arSiteHdls[nSiteIdx], &_dcbSerialParams) == 0)
			{
				CloseHandle(_arSiteHdls[nSiteIdx]);
				_arSiteHdls[nSiteIdx] = nullptr;
				nErrCode = ErrCode::kSiteNotConnectedErr;;
			}

			// Set COM port timeout settings
			_timeouts.ReadIntervalTimeout = 50;
			_timeouts.ReadTotalTimeoutConstant = 50;
			_timeouts.ReadTotalTimeoutMultiplier = 10;
			_timeouts.WriteTotalTimeoutConstant = 50;
			_timeouts.WriteTotalTimeoutMultiplier = 10;
			if (SetCommTimeouts(_arSiteHdls[nSiteIdx], &_timeouts) == 0)
			{
				CloseHandle(_arSiteHdls[nSiteIdx]);
				_arSiteHdls[nSiteIdx] = nullptr;
				nErrCode = ErrCode::kSiteNotConnectedErr;
			}
		}
	}
	_bInitialized = true;

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_Uninitialize()
{
	for (int nSiteIdx = 0; nSiteIdx < _nNumExpectedSites; nSiteIdx++)
	{
		if (CloseHandle(_arSiteHdls[nSiteIdx]))
			_arSiteHdls[nSiteIdx] = nullptr;
	}
	_bInitialized = false;
	return ErrCode::kNoError;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API bool AD_GetInitializedFlg()
{
	return _bInitialized;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API bool AD_IsConnected(int nSiteIdx)
{
	return (_arSiteHdls[nSiteIdx] != nullptr);
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_GetNumExpectedSites()
{
	return _nNumExpectedSites;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_UpdateSysStatusCache(int nSiteIdx)
{
	HostMsg			request(HostMsg::MakeObjId('G', 'S', 't', 't'));
	GetStatusRes	response;

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	if (nErrCode == ErrCode::kNoError)
		_arSysStatus[nSiteIdx] = *response.GetSysStatusPtr();

	return nErrCode;
}
bool AMPDETECTLIBRARY_API AD_GetCachedRunningFlg(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetRunningFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedPausedFlg(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetPausedFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedCaptureCameraImageFlg(int nSiteIdx)	{ return _arSysStatus[nSiteIdx]._siteStatus.GetCaptureCameraImageFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedTempStableFlg(int nSiteIdx)			{ return _arSysStatus[nSiteIdx]._siteStatus.GetTempStableFlg(); }
int  AMPDETECTLIBRARY_API AD_GetCachedCameraIdx(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetCameraIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedOpticsDetectorExposureTime(int nSiteIdx) { return _arSysStatus[nSiteIdx]._siteStatus.GetOpticsDetectorExposureTime(); }
int  AMPDETECTLIBRARY_API AD_GetCachedLedIntensity(int nSiteIdx)			{ return _arSysStatus[nSiteIdx]._siteStatus.GetLedIntensity(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStableTimer(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetStableTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedSegmentIdx(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetSegmentIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedCycleNum(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetCycleNum(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStepIdx(int nSiteIdx)					{ return _arSysStatus[nSiteIdx]._siteStatus.GetStepIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedRunTimer(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetRunTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStepTimer(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetStepTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedHoldTimer(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetHoldTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedTemperature(int nSiteIdx)				{ return _arSysStatus[nSiteIdx]._siteStatus.GetTemperature(); }
int  AMPDETECTLIBRARY_API AD_GetCachedNumOpticsRecs(int nSiteIdx)			{ return _arSysStatus[nSiteIdx]._siteStatus.GetNumOpticsRecs(); }
int  AMPDETECTLIBRARY_API AD_GetCachedNumThermalRecs(int nSiteIdx)			{ return _arSysStatus[nSiteIdx]._siteStatus.GetNumThermalRecs(); }

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetPcrProtocol(int nSiteIdx, LPCSTR sProtName)
{
	ErrCode nErrCode = ErrCode::kNoError;

	//Open the specified PCR protocol.
	std::ifstream pcrFile;
	pcrFile.open(sProtName, std::ios::in | std::ios::binary);
	if (pcrFile.is_open())
	{
		//Get the serialized PCR protocol from file.
		pcrFile.seekg(0, pcrFile.end);
		int nSize = (int)pcrFile.tellg();
		pcrFile.seekg(0, pcrFile.beg);
		std::unique_ptr<char[]> pDstBuf{ new char[nSize] };
		pcrFile.read(pDstBuf.get(), nSize);
		pcrFile.close();

		//Deserialize the PCR protocol into PCR object.
		PcrProtocol pcrProtocol;
		pcrProtocol << (uint8_t*)pDstBuf.get();

		//Prepare request message.
		LoadPcrProtocolReq loadReq;
		HostMsg response;
		loadReq.SetPcrProtocol(pcrProtocol);

		//Send selected PCR protocol to instrument.
		nErrCode = MsgTransaction(nSiteIdx, loadReq, &response);
	}
	else
		nErrCode = ErrCode::kFileNotFound;

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int  AMPDETECTLIBRARY_API AD_StartRun(int nSiteIdx)
{
	StartRunReq	request;
	HostMsg		response;

	request.SetMeerstetterPidFlg(false);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int  AMPDETECTLIBRARY_API AD_StopRun(int nSiteIdx)
{
	return SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('S', 't', 'o', 'p'));
}

/////////////////////////////////////////////////////////////////////////////////
int  AMPDETECTLIBRARY_API AD_PauseRun(int nSiteIdx)
{
	SetPauseRunReq request;
	HostMsg response;

	request.SetSiteIdx(nSiteIdx);
	request.SetCaptureCameraImageFlg(FALSE);
	request.SetPausedFlg(FALSE);

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	nErrCode = response.GetError();
	//return SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('P', 'a', 'u', 's'));
	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_SetOpticsLed(int nSiteIdx, int nLedIdx, int nLedIntensity, int nLedDuration_us)
{
	SetOpticsLedReq request;
	HostMsg			response;
	request.SetSiteIdx(nSiteIdx);
	request.SetChanIdx(nLedIdx);
	request.SetIntensity(nLedIntensity);
	request.SetDuration(nLedDuration_us);

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	nErrCode = response.GetError();

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_StartIntegration(int nSiteIdx, int nLedDuration_us)
{
	StartIntegrationReq request;
	HostMsg			response;
	request.SetDuration(nLedDuration_us);

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	nErrCode = response.GetError();

	return nErrCode;
}
/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_GetDiodeValue(int nSiteIdx, int nDiodeIdx, int* data)
{
	ErrCode nErrCode = ErrCode::kSiteNotConnectedErr;

	if (AD_IsConnected(nSiteIdx) == true)
	{
		GetOpticsDiodeReq request;
		GetOpticsDiodeRes response;
		request.SetSiteIdx(nSiteIdx);
		request.SetDiodeIdx(nDiodeIdx);

		nErrCode = MsgTransaction(nSiteIdx, request, &response);
		*data = response.GetDiodeValue();
	}

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_GetLedAdcValue(int nSiteIdx, int nLedAdcIdx, int* data)
{
	ErrCode nErrCode = ErrCode::kSiteNotConnectedErr;

	if (AD_IsConnected(nSiteIdx) == true)
	{
		GetOpticsLedAdcReq request;
		GetOpticsLedAdcRes response;
		request.SetSiteIdx(nSiteIdx);
		request.SetLedAdcIdx(nLedAdcIdx);

		nErrCode = MsgTransaction(nSiteIdx, request, &response);
		*data = response.GetLedAdcValue();
	}

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_ReadOptics(int nSiteIdx, int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* nDiodeValue, int* nLedMonitor, int* nLedTemp, int* nDiodeTemp)
{
	ErrCode nErrCode = ErrCode::kSiteNotConnectedErr;

	if (AD_IsConnected(nSiteIdx) == true)
	{
		ReadOpticsReq request;
		ReadOpticsRes response;
		request.SetSiteIdx(nSiteIdx);
		request.SetLedIdx(nLedIdx);
		request.SetDiodeIdx(nDiodeIdx);
		request.SetLedIntensity(nLedIntensity);
		request.SetIntegrationTime(nIntegrationTime_us);

		nErrCode = MsgTransaction(nSiteIdx, request, &response);

		*nDiodeValue = response.GetDiodeValue();
		*nLedMonitor = response.GetActiveLedMonitorValue();
		*nLedTemp = response.GetActiveLedTemp();
		*nDiodeTemp = response.GetActiveDiodeTemp();
	}

	return nErrCode;
}


/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetPidParams(	int nSiteIdx, PidType nPidType, float nKp, float nKi, float nKd,
											float nSlope_m, float nYIntercept_m, float nStabilizationTolerance_C, float nStabilizationTime_s)
{
	PidParams		pidParams;
	SetPidParamsReq	request;
	HostMsg			response;

	request.SetType(nPidType);
	pidParams.SetKp(nKp);
	pidParams.SetKi(nKi);
	pidParams.SetKd(nKd);
	pidParams.SetSlope(nSlope_m);
	pidParams.SetYIntercept(nYIntercept_m);
	pidParams.SetStabilizationTolerance(nStabilizationTolerance_C);
	pidParams.SetStabilizationTime(nStabilizationTime_s);
	request.SetPidParams(pidParams);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);

	return (int)nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_GetPidParams(	int nSiteIdx, PidType nPidType, float* pKp, float* pKi, float* pKd,
											float* pSlope_m, float* pYIntercept_m, float* pStabilizationTolerance_C, float* nStabilizationTime_s)
{
	GetPidParamsReq	request;
	GetPidParamsRes	response;

	request.SetType(nPidType);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	if (nErrCode == ErrCode::kNoError)
	{
		PidParams		pidParams;
		pidParams = response.GetPidParams();
		*pKp			= pidParams.GetKp();
		*pKi			= pidParams.GetKi();
		*pKd			= pidParams.GetKd();
		*pSlope_m		= pidParams.GetSlope();
		*pYIntercept_m	= pidParams.GetYIntercept();
		*pStabilizationTolerance_C	= pidParams.GetStabilizationTolerance();
		*nStabilizationTime_s		= pidParams.GetStabilizationTime();
	}

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_UpdateThermalRecCache(	int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned)
{
	GetThermalRecsReq	request;
	GetThermalRecsRes	response;

	*pNumRecsReturned = 0;
	request.SetFirstRecToReadIdx(nFirstRecToReadIdx);
	request.SetMaxRecsToRead(nMaxRecsToRead);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	if (nErrCode == ErrCode::kNoError)
	{
		*pNumRecsReturned = response.GetNumThermalRecs();
		_arThermalRecCache[nSiteIdx].resize(*pNumRecsReturned);
		for (int i = 0; i < *pNumRecsReturned; i++)
			_arThermalRecCache[nSiteIdx][i] = response.GetThermalRec(i);
	}

	return (nErrCode);
}
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecTimeTag(int nSiteIdx, int nRecIdx)	{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nTimeTag_ms; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan1(int nSiteIdx, int nRecIdx)		{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nChan1_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan2(int nSiteIdx, int nRecIdx)		{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nChan2_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan3(int nSiteIdx, int nRecIdx)		{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nChan3_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan4(int nSiteIdx, int nRecIdx)		{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nChan4_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecCurrent(int nSiteIdx, int nRecIdx)	{ return _arThermalRecCache[nSiteIdx][nRecIdx]._nCurrent_mA; }

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_UpdateOpticalRecCache(	int nSiteIdx, int nFirstRecToReadIdx, int nMaxRecsToRead, int* pNumRecsReturned)
{
	GetOpticsRecsReq	request;
	GetOpticsRecsRes	response;

	*pNumRecsReturned = 0;
	request.SetFirstRecToReadIdx(nFirstRecToReadIdx);
	request.SetMaxRecsToRead(nMaxRecsToRead);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	if (nErrCode == ErrCode::kNoError)
	{
		*pNumRecsReturned = response.GetNumOpticsRecs();
		_arOpticalRecCache[nSiteIdx].resize(*pNumRecsReturned);
		for (int i = 0; i < *pNumRecsReturned; i++)
			_arOpticalRecCache[nSiteIdx][i] = response.GetOpticsRec(i);
	}

	return (nErrCode);
}
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecTimeTag(int nSiteIdx, int nRecIdx)			{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nTimeTag_ms; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecCycleNum(int nSiteIdx, int nRecIdx)			{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nCycleNum; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecLedIdx(int nSiteIdx, int nRecIdx)			{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nLedIdx; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDetectorIdx(int nSiteIdx, int nRecIdx)		{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nDetectorIdx; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDarkRead(int nSiteIdx, int nRecIdx)			{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nDarkRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecIlluminatedRead(int nSiteIdx, int nRecIdx)	{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nIlluminatedRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefDarkRead(int nSiteIdx, int nRecIdx)		{ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nRefDarkRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefIlluminatedRead(int nSiteIdx, int nRecIdx){ return (_arOpticalRecCache[nSiteIdx][nRecIdx])._nRefIlluminatedRead; }

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_DisableManualControl(int nSiteIdx)
{
	return (SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('D', 'M', 'a', 'n')));
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetTemperatureSetpoint(int nSiteIdx, int nSetpoint_C)
{
	SetManControlTemperatureReq	request;
	HostMsg						response;

	request.SetSetpoint(nSetpoint_C);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	return (nErrCode);
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetCurrentSetpoint(int nSiteIdx, int nSetpoint_A)
{
	SetManControlCurrentReq	request;
	HostMsg					response;

	request.SetSetpoint(nSetpoint_A);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	return (nErrCode);
}

/////////////////////////////////////////////////////////////////////////////////
ErrCode MsgTransaction(int nSiteIdx, HostMsg& request, HostMsg* pResponse)
{
	ErrCode	nErr = ErrCode::kNoError;
	DWORD nBytesRead = 0;

	if (AD_IsConnected(nSiteIdx) == false)
		return ErrCode::kSiteNotConnectedErr;

	try
	{
		//Send request to firmware.
		request.SetMsgSize(request.GetStreamSize());
		uint8_t arReqBuf[HostMsg::kMaxRequestSize];
		request >> arReqBuf;
		DWORD nBytesWritten;
		if (WriteFile(_arSiteHdls[nSiteIdx], arReqBuf, request.GetStreamSize(), &nBytesWritten, NULL) == FALSE)
			return ErrCode::kDeviceCommErr;

		//Get response header.
		HostMsg msgHdr;
		uint8_t arResBuf[HostMsg::kMaxResponseSize];
		int	nTimeout_ms = 0;
		while ((nBytesRead < msgHdr.GetStreamSize()) && (nTimeout_ms < 2000))
		{
			ReadFile(_arSiteHdls[nSiteIdx], arResBuf, msgHdr.GetStreamSize(), &nBytesRead, NULL);
			::Sleep(5);
			nTimeout_ms += 5;
		}

		//If response header received successfully.
		if (nTimeout_ms < 2000)
		{
			//If there is more data in this response than just the header.
			msgHdr << arResBuf;
			if ((msgHdr.GetMsgSize() > msgHdr.GetStreamSize()) && (nErr == ErrCode::kNoError))
			{
				int nTotalBytes = msgHdr.GetStreamSize();
				nTimeout_ms = 0;

				//Get remaining bytes in response.
				while ((nTotalBytes < (int)msgHdr.GetMsgSize()) && (nTimeout_ms < 2000))
				{
					ReadFile(_arSiteHdls[nSiteIdx], &arResBuf[(int)msgHdr.GetStreamSize()], msgHdr.GetMsgSize() - msgHdr.GetStreamSize(), &nBytesRead, NULL);
					Sleep(5);
					nTimeout_ms += 5;
					nTotalBytes += nBytesRead;
				}

				//If we got all data successfully.
				if (nTimeout_ms < 2000)
				{
					nErr = pResponse->GetError();	//The AmpDetect site might have generated an error.
					*pResponse << arResBuf;
				}
				else //timeout when receiving the response message.
					nErr = ErrCode::kDeviceCommErr;
			}
		}
		else //timeout when receiving the response header.
			nErr = ErrCode::kDeviceCommErr;
	}
	catch (...)
	{
		nErr = ErrCode::kDeviceCommErr;
	}

	return nErr;
}

/////////////////////////////////////////////////////////////////////////////////
ErrCode SimpleTransaction(int nSiteIdx, uint32_t nMsgId)
{
	HostMsg		request(nMsgId);
	HostMsg		response;

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
void ErrorExit()
{
	// Retrieve the system error message for the last-error code

	LPWSTR pBuffer = NULL;
//	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&pBuffer,
		0, NULL);

	// Display the error message and exit the process

/*	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);*/

	LocalFree(pBuffer);
//	LocalFree(lpDisplayBuf);
//	ExitProcess(dw);
}
