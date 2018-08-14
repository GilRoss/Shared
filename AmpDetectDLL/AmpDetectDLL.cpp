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
HANDLE					_arSiteHdls[kMaxADSites];
DCB						_dcbSerialParams = { 0 };
COMMTIMEOUTS			_timeouts = { 0 };
SysStatus				_sysStatusCache;
int						_nNumExpectedSites;
int						_nFirstSiteId;
std::vector<OpticsRec>	_arOpticalRecCache;
std::vector<ThermalRec>	_arThermalRecCache;

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_Initialize(int nNumExpectedSites, int nFirstSiteId)
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
				nErrCode = ErrCode::kSiteNotConnectedErr;;
			}
		}
	}

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
	return ErrCode::kNoError;
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
		_sysStatusCache = *response.GetSysStatusPtr();

	return nErrCode;
}
bool AMPDETECTLIBRARY_API AD_GetCachedRunningFlg()				{ return (_sysStatusCache._arSiteStatus[0]).GetRunningFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedPausedFlg()				{ return _sysStatusCache._arSiteStatus[0].GetPausedFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedCaptureCameraImageFlg()	{ return _sysStatusCache._arSiteStatus[0].GetCaptureCameraImageFlg(); }
bool AMPDETECTLIBRARY_API AD_GetCachedTempStableFlg()			{ return _sysStatusCache._arSiteStatus[0].GetTempStableFlg(); }
int  AMPDETECTLIBRARY_API AD_GetCachedCameraIdx()				{ return _sysStatusCache._arSiteStatus[0].GetCameraIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStableTimer()				{ return _sysStatusCache._arSiteStatus[0].GetStableTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedSegmentIdx()				{ return _sysStatusCache._arSiteStatus[0].GetSegmentIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedCycleNum()				{ return _sysStatusCache._arSiteStatus[0].GetCycleNum(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStepIdx()					{ return _sysStatusCache._arSiteStatus[0].GetStepIdx(); }
int  AMPDETECTLIBRARY_API AD_GetCachedRunTimer()				{ return _sysStatusCache._arSiteStatus[0].GetRunTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedStepTimer()				{ return _sysStatusCache._arSiteStatus[0].GetStepTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedHoldTimer()				{ return _sysStatusCache._arSiteStatus[0].GetHoldTimer(); }
int  AMPDETECTLIBRARY_API AD_GetCachedTemperature()				{ return _sysStatusCache._arSiteStatus[0].GetTemperature(); }
int  AMPDETECTLIBRARY_API AD_GetCachedNumOpticsRecs()			{ return _sysStatusCache._arSiteStatus[0].GetNumOpticsRecs(); }
int  AMPDETECTLIBRARY_API AD_GetCachedNumThermalRecs()			{ return _sysStatusCache._arSiteStatus[0].GetNumThermalRecs(); }

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetPcrProtocol(int nSiteIdx, LPCTSTR sProtName)
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
	return SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('S', 'R', 'u', 'n'));
}

/////////////////////////////////////////////////////////////////////////////////
int  AMPDETECTLIBRARY_API AD_StopRun(int nSiteIdx)
{
	return SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('S', 't', 'o', 'p'));
}

/////////////////////////////////////////////////////////////////////////////////
int  AMPDETECTLIBRARY_API AD_PauseRun(int nSiteIdx)
{
	return SimpleTransaction(nSiteIdx, HostMsg::MakeObjId('P', 'a', 'u', 's'));
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
AMPDETECTLIBRARY_API int AD_GetDiodeValue(int nSiteIdx, int nDiodeIdx, int* data)
{
	ErrCode nErrCode = ErrCode::kSiteNotConnectedErr;

	if (AD_IsConnected(nSiteIdx) == true)
	{
		GetOpticsDiodeReq request;
		GetOpticsDiodeRes response;
		request.SetDiodeIdx(nDiodeIdx);

		nErrCode = MsgTransaction(nSiteIdx, request, &response);
		*data = response.GetDiodeValue();
	}

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
AMPDETECTLIBRARY_API int AD_ReadOptics(int nSiteIdx, int nDiodeIdx, int nLedIdx, int nLedIntensity, int nIntegrationTime_us, int* data)
{
	ErrCode nErrCode = ErrCode::kSiteNotConnectedErr;

	if (AD_IsConnected(nSiteIdx) == true)
	{
		ReadOpticsReq request;
		ReadOpticsRes response;
		request.SetLedIdx(nLedIdx);
		request.SetDiodeIdx(nDiodeIdx);
		request.SetLedIntensity(nLedIntensity);
		request.SetIntegrationTime(nIntegrationTime_us);

		nErrCode = MsgTransaction(nSiteIdx, request, &response);

		*data = response.GetDiodeValue();
	}

	return nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetPidParams(int nSiteIdx, PidType nPidType, int nKp, int nKi, int nKd, int nSlope_m, int nYIntercept_m)
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
	request.SetPidParams(pidParams);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);

	return (int)nErrCode;
}

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_GetPidParams(int nSiteIdx, PidType nPidType, int* pKp, int* pKi, int* pKd, int* pSlope_m, int* pYIntercept_m)
{
	SetPidParamsReq	request;
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
		_arThermalRecCache.resize(*pNumRecsReturned);
		for (int i = 0; i < *pNumRecsReturned; i++)
			_arThermalRecCache[i] = response.GetThermalRec(i);
	}

	return (nErrCode);
}
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecTimeTag(int nIdx){ return _arThermalRecCache[nIdx]._nTimeTag_ms; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan1(int nIdx)	{ return _arThermalRecCache[nIdx]._nChan1_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan2(int nIdx)	{ return _arThermalRecCache[nIdx]._nChan2_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan3(int nIdx)	{ return _arThermalRecCache[nIdx]._nChan3_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecChan4(int nIdx)	{ return _arThermalRecCache[nIdx]._nChan4_mC; }
extern "C" int	AMPDETECTLIBRARY_API AD_GetCachedThermalRecCurrent(int nIdx){ return _arThermalRecCache[nIdx]._nCurrent_mA; }

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
		_arOpticalRecCache.resize(*pNumRecsReturned);
		for (int i = 0; i < *pNumRecsReturned; i++)
			_arOpticalRecCache[i] = response.GetOpticsRec(i);
	}

	return (nErrCode);
}
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecTimeTag(int nIdx)			{ return (_arOpticalRecCache[nIdx])._nTimeTag_ms; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecCycleNum(int nIdx)			{ return (_arOpticalRecCache[nIdx])._nCycleNum; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecLedIdx(int nIdx)				{ return (_arOpticalRecCache[nIdx])._nLedIdx; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDetectorIdx(int nIdx)		{ return (_arOpticalRecCache[nIdx])._nDetectorIdx; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecDarkRead(int nIdx)			{ return (_arOpticalRecCache[nIdx])._nDarkRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecIlluminatedRead(int nIdx)	{ return (_arOpticalRecCache[nIdx])._nIlluminatedRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefDarkRead(int nIdx)		{ return (_arOpticalRecCache[nIdx])._nRefDarkRead; }
int	AMPDETECTLIBRARY_API AD_GetCachedOpticalRecRefIlluminatedRead(int nIdx)	{ return (_arOpticalRecCache[nIdx])._nRefIlluminatedRead; }

/////////////////////////////////////////////////////////////////////////////////
int	AMPDETECTLIBRARY_API AD_SetTemperatureSetpoint(int nSiteIdx, int nSetpoint_mC)
{
	SetManControlSetpointReq	request;
	HostMsg						response;

	request.SetSetpoint(nSetpoint_mC);
	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	return (nErrCode);
}

/////////////////////////////////////////////////////////////////////////////////
ErrCode MsgTransaction(int nSiteIdx, HostMsg& request, HostMsg* pResponse)
{
	ErrCode	nErr = ErrCode::kDeviceCommErr;
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
	HostMsg			request(nMsgId);
	GetStatusRes	response;

	ErrCode nErrCode = MsgTransaction(nSiteIdx, request, &response);
	if (nErrCode == ErrCode::kNoError)
		_sysStatusCache = *response.GetSysStatusPtr();

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
