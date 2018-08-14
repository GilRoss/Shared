#ifndef __SysStatus_H
#define __SysStatus_H

#include <cstdint>
#include <vector>
#include "Common.h"
#include "StreamingObj.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SiteStatus : public StreamingObj
{
public:
    SiteStatus()
        :StreamingObj(MakeObjId('S', 't', 'e', 'S'))
		, _bRunning(false)
        , _bPaused(false)
        , _bCaptureCameraImage(false)
        , _nCameraIdx(0)
        , _nOpticsDetectorExposureTime_us(0)
        , _nLedIdx(0)
        , _nLedIntensity(0)
        , _nSegmentIdx(0)
        , _nCycleNum(0)
        , _nStepIdx(0)
        , _nRunTimer_ms(0)
        , _nStepTimer_ms(0)
        , _nHoldTimer_ms(0)
		, _nTemperature_mC(0)
        , _nNumOpticsRecs(0)
        , _nNumThermalRecs(0)
    {
    }

	virtual uint32_t GetStreamSize() const
	{
        uint32_t nSize = StreamingObj::GetStreamSize();
        nSize += 13 * sizeof(uint32_t);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(pData + StreamingObj::GetStreamSize());
        _bRunning           = swap_uint32(*pSrc++) != 0;
        _bPaused            = swap_uint32(*pSrc++) != 0;
        _bCaptureCameraImage= swap_uint32(*pSrc++) != 0;
        _nCameraIdx         = swap_uint32(*pSrc++);
        _nOpticsDetectorExposureTime_us = swap_uint32(*pSrc++);
        _nLedIdx            = swap_uint32(*pSrc++);
        _nLedIntensity      = swap_uint32(*pSrc++);
        _nSegmentIdx        = swap_uint32(*pSrc++);
		_nCycleNum			= swap_uint32(*pSrc++);
        _nStepIdx           = swap_uint32(*pSrc++);
        _nRunTimer_ms       = swap_uint32(*pSrc++);
        _nStepTimer_ms      = swap_uint32(*pSrc++);
		_nHoldTimer_ms	    = swap_uint32(*pSrc++);
		_nTemperature_mC    = swap_uint32(*pSrc++);
		_nNumThermalRecs    = swap_uint32(*pSrc++);
        _nNumOpticsRecs     = swap_uint32(*pSrc++);
  	}

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(pData + StreamingObj::GetStreamSize());
        *pDst++ = swap_uint32(_bRunning ? 1 : 0);
        *pDst++ = swap_uint32(_bPaused ? 1 : 0);
        *pDst++ = swap_uint32(_bCaptureCameraImage ? 1 : 0);
        *pDst++ = swap_uint32(_nCameraIdx);
        *pDst++ = swap_uint32(_nOpticsDetectorExposureTime_us);
        *pDst++ = swap_uint32(_nLedIdx);
        *pDst++ = swap_uint32(_nLedIntensity);
        *pDst++ = swap_uint32(_nSegmentIdx);
        *pDst++ = swap_uint32(_nCycleNum);
        *pDst++ = swap_uint32(_nStepIdx);
        *pDst++ = swap_uint32(_nRunTimer_ms);
        *pDst++ = swap_uint32(_nStepTimer_ms);
		*pDst++ = swap_uint32(_nHoldTimer_ms);
		*pDst++ = swap_uint32(_nTemperature_mC);
		*pDst++ = swap_uint32(_nNumThermalRecs);
		*pDst++ = swap_uint32(_nNumOpticsRecs);
	}
    
	void        SetTemperature(uint32_t n)		        { _nTemperature_mC = n; }
	uint32_t    GetTemperature() const			        { return _nTemperature_mC; }
    void        SetRunningFlg(bool b)                   {_bRunning = b;}
    bool        GetRunningFlg() const                   {return _bRunning;}
    void        SetPausedFlg(bool b)                    {_bPaused = b;}
    bool        GetPausedFlg() const                    {return _bPaused;}
    void        SetCaptureCameraImageFlg(bool b)        {_bCaptureCameraImage = b;}
    bool        GetCaptureCameraImageFlg() const        {return _bCaptureCameraImage;}
    void        SetCameraIdx(uint32_t nIdx)             {_nCameraIdx = nIdx;}
    uint32_t    GetCameraIdx() const                    {return _nCameraIdx;}
    void        SetOpticsDetectorExposureTime(uint32_t n){_nOpticsDetectorExposureTime_us = n;}
    uint32_t    GetOpticsDetectorExposureTime() const    {return _nOpticsDetectorExposureTime_us;}
    void        SetLedIdx(uint32_t nIdx)                {_nLedIdx = nIdx;}
    uint32_t    GetLedIdx() const                       {return _nLedIdx;}
    void        SetLedIntensity(uint32_t n)             {_nLedIntensity = n;}
    uint32_t    GetLedIntensity() const                 {return _nLedIntensity;}
    void        SetTempStableFlg(bool b)                {_bTempStable = b;}
    bool        GetTempStableFlg() const                {return _bTempStable;}
    void        SetStableTimer(uint32_t t)              { _nStableTimer_ms = t; }
    uint32_t    GetStableTimer() const                  { return _nStableTimer_ms; }
    void        SetSegmentIdx(uint32_t nIdx)            { _nSegmentIdx = nIdx; }
    uint32_t    GetSegmentIdx() const                   { return _nSegmentIdx; }
    void        SetCycleNum(uint32_t nCyc)              { _nCycleNum = nCyc;}
    uint32_t    GetCycleNum() const                     {return _nCycleNum;}
    void        SetStepIdx(uint32_t nIdx)               {_nStepIdx = nIdx;}
    uint32_t    GetStepIdx() const                      {return _nStepIdx;}
    void        SetStepTimer(uint32_t nTime)            {_nStepTimer_ms = nTime;}
    uint32_t    GetStepTimer() const                    {return _nStepTimer_ms;}
    void        AddStepTimer(uint32_t nTime)            {_nStepTimer_ms += nTime;}
    void        SetHoldTimer(uint32_t nTime)            {_nHoldTimer_ms = nTime;}
    uint32_t    GetHoldTimer() const                    {return _nHoldTimer_ms;}
    void        AddHoldTimer(uint32_t nTime)            {_nHoldTimer_ms += nTime;}
    void		SetRunTimer(uint32_t nTime)             {_nRunTimer_ms = nTime;}
	uint32_t	GetRunTimer() const						{ return _nRunTimer_ms; }
	void		AddRunTimer(uint32_t nTime)				{ _nRunTimer_ms += nTime; }
	uint32_t	GetNumThermalRecs() const				{ return _nNumThermalRecs; }
	void		SetNumThermalRecs(uint32_t n)			{ _nNumThermalRecs = n; }
	uint32_t	GetNumOpticsRecs() const				{ return _nNumOpticsRecs; }
	void		SetNumOpticsRecs(uint32_t n)			{ _nNumOpticsRecs = n; }
    
   
    void        NextStep()
                {
                    uint32_t nextStep = GetStepIdx() + 1;
                    ResetForNewStep();
                    SetStepIdx(nextStep);
                }
    
    void        NextCycle()
                {
                    uint32_t nextCycle = GetCycleNum() + 1;
                    ResetForNewStep();
                    SetCycleNum(nextCycle);
                }
    
    void        NextSegment()
                {
					_nCycleNum = 0;
                    uint32_t nextSegment = GetSegmentIdx() + 1;
                    ResetForNewStep();
                    SetSegmentIdx(nextSegment);
                }
    
    void        ResetForNewStep()
                {
                    _bCaptureCameraImage = false;
                    _nCameraIdx = 0;
                    _bTempStable = false;
                    _nStableTimer_ms = 0;
                    _nStepIdx = 0;
                    _nStepTimer_ms = 0;
                    _nHoldTimer_ms = 0;
                    _nOpticsDetectorExposureTime_us = 0;
                    _nLedIdx = 0;
                    _nLedIntensity = 0;

                }

    void        ResetForNewRun()
                {
                    ResetForNewStep();
                    _bRunning = false;
                    _bPaused = false;
					_nCycleNum = 0;
                    _nSegmentIdx = 0;
                    _nRunTimer_ms = 0;
                    _nNumThermalRecs = 0;
                    _nNumOpticsRecs = 0;
                }
        
protected:
  
private:
    bool            _bRunning;
    bool            _bPaused;
    bool            _bCaptureCameraImage;
    uint32_t        _nCameraIdx;
    uint32_t        _nOpticsDetectorExposureTime_us;
    uint32_t        _nLedIdx;
    uint32_t        _nLedIntensity;
    bool            _bTempStable;
    uint32_t        _nStableTimer_ms;
    uint32_t        _nSegmentIdx;
    uint32_t        _nCycleNum;
    uint32_t        _nStepIdx;
    uint32_t        _nRunTimer_ms;
    uint32_t        _nStepTimer_ms;
	uint32_t        _nHoldTimer_ms;
	uint32_t        _nTemperature_mC;
    uint32_t        _nNumOpticsRecs;
	uint32_t        _nNumThermalRecs;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SysStatus : public StreamingObj
{
public:    
	std::vector<SiteStatus>       _arSiteStatus;
	
	SysStatus(uint32_t nNumSites = 1)
        :StreamingObj(MakeObjId('S', 'y', 's', 'S'))
        ,_arSiteStatus(nNumSites)
    {
    }

	uint32_t			GetNumSites() const { return (uint32_t)_arSiteStatus.size(); }
	void            	SetSiteStatus(uint32_t nSiteIdx, const SiteStatus& ss)  { _arSiteStatus[nSiteIdx] = ss; }
	const SiteStatus&	GetSiteStatus(uint32_t nSiteIdx) const { return _arSiteStatus[nSiteIdx]; }

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = StreamingObj::GetStreamSize();
		nSize += sizeof(uint32_t);
		for (int i = 0; i < (int)_arSiteStatus.size(); i++)
			nSize += _arSiteStatus[i].GetStreamSize();

		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(pData + StreamingObj::GetStreamSize());
		uint32_t nNumSites = swap_uint32(*pSrc++);
		_arSiteStatus.resize(nNumSites);
		for (int i = 0; i < (int)nNumSites; i++)
		{
			_arSiteStatus[i] << (uint8_t*)pSrc;
			pSrc = (uint32_t*)((uint8_t*)pSrc + _arSiteStatus[i].GetStreamSize());
		}
    }

    virtual void     operator>>(uint8_t* pData)
    {
		StreamingObj::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + StreamingObj::GetStreamSize());
		*pDst++ = swap_uint32((uint32_t)_arSiteStatus.size());
		for (int i = 0; i < (int)_arSiteStatus.size(); i++)
		{
			_arSiteStatus[i] >> (uint8_t*)pDst;
			pDst = (uint32_t*)((uint8_t*)pDst + _arSiteStatus[i].GetStreamSize());
		}
	}

	SysStatus& operator=(const SysStatus& rhs)
	{
		_arSiteStatus.resize(rhs.GetNumSites());
		for (int i = 0; i < (int)_arSiteStatus.size(); i++)
			_arSiteStatus[i] = rhs.GetSiteStatus(i);

		return *this;
	}

protected:
  
private:
};

#endif // __SysStatus_H
