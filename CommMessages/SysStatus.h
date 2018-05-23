#ifndef __SysStatus_H
#define __SysStatus_H

#include <cstdint>
#include <vector>
#include "StreamingObj.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct OpticsRec
{
public:
    uint32_t    _nTimeTag_ms;
    uint32_t    _nCycleIdx;
    uint32_t    _nDarkRead;
    uint32_t    _nIlluminatedRead;
    int32_t     _nShuttleTemp_mC;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ThermalRec
{
public:
    uint32_t    _nTimeTag_ms;
    int32_t     _nSampleTemp_mC;
    int32_t     _nSinkTemp_mC;
    int32_t     _nBlockTemp_mC;
    int32_t     _nCurrent_mA;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SiteStatus : public StreamingObj
{
public:
    SiteStatus()
        :StreamingObj(MakeObjId('S', 't', 'e', 'S'))
		, _bRunning(false)
        , _nSegmentIdx(0)
        , _nCycle(0)
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
        nSize += 10 * sizeof(uint32_t);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(pData + StreamingObj::GetStreamSize());
        _bRunning           = *pSrc++ == 1;
        _nSegmentIdx        = *pSrc++;
        _nCycle             = *pSrc++;
        _nStepIdx           = *pSrc++;
        _nRunTimer_ms       = *pSrc++;
        _nStepTimer_ms      = *pSrc++;
		_nHoldTimer_ms	    = *pSrc++;
		_nTemperature_mC    = *pSrc++;
		_nNumThermalRecs    = *pSrc++;
        _nNumOpticsRecs     = *pSrc++;
  	}

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(pData + StreamingObj::GetStreamSize());
        *pDst++ = _bRunning ? 1 : 0;
        *pDst++ = _nSegmentIdx;
        *pDst++ = _nCycle;
        *pDst++ = _nStepIdx;
        *pDst++ = _nRunTimer_ms;
        *pDst++ = _nStepTimer_ms;
		*pDst++ = _nHoldTimer_ms;
		*pDst++ = _nTemperature_mC;
		*pDst++ = _nNumThermalRecs;
		*pDst++ = _nNumOpticsRecs;
	}
    
	void        SetTemperature(uint32_t n)		        { _nTemperature_mC = n; }
	uint32_t    GetTemperature() const			        { return _nTemperature_mC; }
    void        SetRunningFlg(bool b)                   {_bRunning = b;}
    bool        GetRunningFlg() const                   {return _bRunning;}
    void        SetTempStableFlg(bool b)                {_bTempStable = b;}
    bool        GetTempStableFlg() const                {return _bTempStable;}
    void        SetStableTimer(uint32_t t)              { _nStableTimer_ms = t; }
    uint32_t    GetStableTimer() const                  { return _nStableTimer_ms; }
    void        SetSegmentIdx(uint32_t nIdx)            { _nSegmentIdx = nIdx; }
    uint32_t    GetSegmentIdx() const                   { return _nSegmentIdx; }
    void        SetCycle(uint32_t nCyc)                 {_nCycle = nCyc;}
    uint32_t    GetCycle() const                        {return _nCycle;}
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
                    _nStepIdx++;
                    _nStepTimer_ms = 0;
                    _nHoldTimer_ms = 0;
                    _bTempStable = false;
                    _nStableTimer_ms = 0;
                }
    
    void        NextCycle()
                {
                    _nCycle++;
                    _nStepIdx = 0;
                    _nStepTimer_ms = 0;
                    _nHoldTimer_ms = 0;
                    _bTempStable = false;
                    _nStableTimer_ms = 0;
                }
    
    void        NextSegment()
                {
                    _nSegmentIdx++;
                    _nCycle = 0;
                    _nStepIdx = 0;
                    _nStepTimer_ms = 0;
                    _nHoldTimer_ms = 0;
                    _bTempStable = false;
                    _nStableTimer_ms = 0;
                }
    
    void        ResetForNewRun()
                {
                    _bRunning = false;
                    _bTempStable = false;
                    _nStableTimer_ms = 0;
                    _nSegmentIdx = 0;
                    _nCycle = 0;
                    _nStepIdx = 0;
                    _nStepTimer_ms = 0;
                    _nHoldTimer_ms = 0;
                    _nRunTimer_ms = 0;
                    _nNumThermalRecs = 0;
                    _nNumOpticsRecs = 0;
                }
        
protected:
  
private:
    bool            _bRunning;
    bool            _bTempStable;
    uint32_t        _nStableTimer_ms;
    uint32_t        _nSegmentIdx;
    uint32_t        _nCycle;
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
		uint32_t nNumSites = *pSrc++;
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
		*pDst++ = (uint32_t)_arSiteStatus.size();
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
    std::vector<SiteStatus>       _arSiteStatus;
};

#endif // __SysStatus_H
