#ifndef  __PcrProtocol_H
#define  __PcrProtocol_H

#include <cstdint>
#include <vector>
#include "StreamingObj.h"

enum {kNumOpticChans = 6};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Step : public StreamingObj
{
public:
    Step()
        :StreamingObj(MakeObjId('S','t','e','p'))
        , _nTargetTemp_mC(0)
        , _nHoldTimer_ms(0)
		, _bOpticalAcquisition(false)
        , _bMelt(false)
        , _nRampRate_mCPerSec(0)
    {
    }
    
    void        SetTargetTemp(int32_t nTargetTemp_mC)   {_nTargetTemp_mC = nTargetTemp_mC;}
    int32_t     GetTargetTemp() const                   {return _nTargetTemp_mC;}
    void        SetHoldTimer(uint32_t nHoldTime_ms)     {_nHoldTimer_ms = nHoldTime_ms;}
    uint32_t    GetHoldTimer() const                    {return _nHoldTimer_ms;}
	void        SetOpticalAcqFlg(bool b)				{ _bOpticalAcquisition = b; }
	bool        GetOpticalAcqFlg() const				{ return _bOpticalAcquisition; }
	void        SetMeltFlg(bool b)						{ _bMelt = b; }
	bool        GetMeltFlg() const						{ return _bMelt; }
	void        SetRampRate(int32_t nRampRate_mCPerSec) { _nRampRate_mCPerSec = nRampRate_mCPerSec; }
	int32_t     GetRampRate() const						{ return _nRampRate_mCPerSec; }

    void        SetTargetTemp(int32_t nTargetTemp_mC, uint32_t nHoldTime_ms,
                              bool bOpticalAcq = false, bool bMelt = false, int32_t nRampRate_mCPerSec = 0)
                {
                    _nTargetTemp_mC			= nTargetTemp_mC;
                    _nHoldTimer_ms			= nHoldTime_ms;
					_bOpticalAcquisition	= bOpticalAcq;
					_bMelt					= bMelt;
					_nRampRate_mCPerSec		= nRampRate_mCPerSec;
                }
    
	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = StreamingObj::GetStreamSize();
		nSize += sizeof(_nTargetTemp_mC);
		nSize += sizeof(_nHoldTimer_ms);
		nSize += sizeof(uint32_t);	//_bOpticalAcquisition
		nSize += sizeof(uint32_t);	//_bMelt
		nSize += sizeof(_nRampRate_mCPerSec);
		return nSize;
	}

    virtual void    operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		uint32_t*   pSrc = (uint32_t*)(pData + StreamingObj::GetStreamSize());
		_nTargetTemp_mC			= swap_uint32(*pSrc++);
        _nHoldTimer_ms			= swap_uint32(*pSrc++);
		_bOpticalAcquisition	= swap_uint32(*pSrc++) != 0;
		_bMelt					= swap_uint32(*pSrc++) != 0;
        _nRampRate_mCPerSec		= swap_uint32(*pSrc++);
	}

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		*pDst++ = swap_uint32(_nTargetTemp_mC);
		*pDst++ = swap_uint32(_nHoldTimer_ms);
		*pDst++ = swap_uint32(_bOpticalAcquisition ? 1 : 0);
		*pDst++ = swap_uint32(_bMelt ? 1 : 0);
		*pDst++ = swap_uint32(_nRampRate_mCPerSec);
	}

protected:
  
private:
    int32_t     _nTargetTemp_mC;
    uint32_t    _nHoldTimer_ms;
	bool        _bOpticalAcquisition;
	bool        _bMelt;
	int32_t     _nRampRate_mCPerSec;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Segment : public StreamingObj
{
public:
    Segment()
        :StreamingObj(MakeObjId('S','e','g',' '))
        ,_nNumCycles(0)
    {
    }

    virtual ~Segment()
    {
        Clear();
    }

    void        SetNumCycles(uint32_t nNumCycs) {_nNumCycles = nNumCycs;}
    uint32_t    GetNumCycles() const            {return _nNumCycles;}
    uint32_t    GetNumSteps() const             {return (uint32_t)_vSteps.size();}
    const Step& GetStep(uint32_t stpIdx) const  {return _vSteps[stpIdx];}
    void        Clear()                         {_vSteps.clear();}

	void AddStep(const Step& step)
	{
		_vSteps.push_back(step);
	}

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = StreamingObj::GetStreamSize();
		nSize += sizeof(_nNumCycles);
		nSize += sizeof(uint32_t);	//Number of steps.
        for (int i = 0; i < (int)_vSteps.size(); i++)
            nSize += _vSteps[i].GetStreamSize();
        
        return nSize;
    }

    virtual void    operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		uint32_t*   pSrc = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		_nNumCycles		= swap_uint32(*pSrc++);

		_vSteps.clear();
		int nNumSteps = swap_uint32(*pSrc++);
		_vSteps.resize(nNumSteps);
		for (int i = 0; i < nNumSteps; i++)
        {
			_vSteps[i] << (uint8_t*)pSrc;
			pSrc += _vSteps[i].GetStreamSize() / sizeof(uint32_t);
        }
    }

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		*pDst++ = swap_uint32(_nNumCycles);
		*pDst++ = swap_uint32((uint32_t)_vSteps.size());

		for (int i = 0; i < (int)_vSteps.size(); i++)
        {
            _vSteps[i] >> (uint8_t*)pDst;
			pDst += _vSteps[i].GetStreamSize() / sizeof(uint32_t);
        }
    }

    Segment& operator=(const Segment& rhs)
    {
        Clear();
        _vSteps.resize(rhs.GetNumSteps());
		_nNumCycles = rhs.GetNumCycles();
        for (int nStepIdx = 0; nStepIdx < (int)rhs.GetNumSteps(); nStepIdx++)
            _vSteps[nStepIdx] = rhs.GetStep(nStepIdx);

        return *this;
    }
    
protected:
  
private:
    uint32_t            _nNumCycles;
    std::vector<Step>   _vSteps;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PcrProtocol : public StreamingObj
{
public:
	enum DetectorType : uint32_t
	{
		kPhotoDiode,
		kCamera
	};

    PcrProtocol()
        :StreamingObj(MakeObjId('P','r','o','t'))
		, _nDetectorType(DetectorType::kPhotoDiode)
		, _nLedIdx(0)
		, _nLedIntensity(0)
		, _nLedStablizationTime_us(0)
		, _nDetectorIdx(0)
		, _nDetectorIntegrationTime_us(0)
	{
    }

    virtual ~PcrProtocol()
    {
        Clear();
    }

	void			SetDetectorType(uint32_t n)					{ _nDetectorType = n; }
	uint32_t		GetDetectorType() const						{ return _nDetectorType; }
	void			SetLedIdx(uint32_t n)						{ _nLedIdx = n; }
	uint32_t		GetLedIdx() const							{ return _nLedIdx; }
	void			SetLedIntensity(uint32_t n)					{ _nLedIntensity = n; }
	uint32_t		GetLedIntensity() const						{ return _nLedIntensity; }
	void			SetLedStablizationTime(uint32_t nTime_us)	{ _nLedStablizationTime_us = nTime_us; }
	uint32_t		GetLedStablizationTime() const				{ return _nLedStablizationTime_us; }
	void			SetDetectorIdx(uint32_t n)						{ _nDetectorIdx = n; }
	uint32_t		GetDetectorIdx() const							{ return _nDetectorIdx; }
	void			SetDetectorIntegrationTime(uint32_t nTime_us)	{ _nDetectorIntegrationTime_us = nTime_us; }
	uint32_t		GetDetectorIntegrationTime() const				{ return _nDetectorIntegrationTime_us; }

	uint32_t        GetNumSegs() const              {return (uint32_t)_vSegments.size();}
    const Segment&  GetSegment(uint32_t idx) const  {return _vSegments[idx];}
    void            Clear()                         {_vSegments.clear();}

	void AddSegment(const Segment& seg)
	{
		_vSegments.push_back(seg);
	}

    virtual uint32_t        GetStreamSize() const
    {
        uint32_t nSize = StreamingObj::GetStreamSize();
		nSize += sizeof(_nDetectorType);
		nSize += sizeof(_nLedIdx);
		nSize += sizeof(_nLedIntensity);
		nSize += sizeof(_nLedStablizationTime_us);
		nSize += sizeof(_nDetectorIdx);
		nSize += sizeof(_nDetectorIntegrationTime_us);

		nSize += sizeof(uint32_t); //Number of segments.
		for (int i = 0; i < (int)_vSegments.size(); i++)
            nSize += _vSegments[i].GetStreamSize();
        
        return nSize;
    }

    virtual void    operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		uint32_t*   pSrc = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);

		_nDetectorType				= swap_uint32(*pSrc++);
		_nLedIdx					= swap_uint32(*pSrc++);
		_nLedIntensity				= swap_uint32(*pSrc++);
		_nLedStablizationTime_us	= swap_uint32(*pSrc++);
		_nDetectorIdx				= swap_uint32(*pSrc++);
		_nDetectorIntegrationTime_us= swap_uint32(*pSrc++);

		_vSegments.clear();
		int nNumSegs = swap_uint32(*pSrc++);
		_vSegments.resize(nNumSegs);
		for (int nSegIdx = 0; nSegIdx < nNumSegs; nSegIdx++)
        {
			_vSegments[nSegIdx] << (uint8_t*)pSrc;
			pSrc += _vSegments[nSegIdx].GetStreamSize() / sizeof(uint32_t);
        }
    }

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);

		*pDst++ = swap_uint32(_nDetectorType);
		*pDst++ = swap_uint32(_nLedIdx);
		*pDst++ = swap_uint32(_nLedIntensity);
		*pDst++ = swap_uint32(_nLedStablizationTime_us);
		*pDst++ = swap_uint32(_nDetectorIdx);
		*pDst++ = swap_uint32(_nDetectorIntegrationTime_us);

		*pDst++ = swap_uint32((uint32_t)_vSegments.size());
		for (int i = 0; i < (int)_vSegments.size(); i++)
        {
            _vSegments[i] >> (uint8_t*)pDst;
			pDst += _vSegments[i].GetStreamSize() / sizeof(uint32_t);
        }
    }

    PcrProtocol& operator=(const PcrProtocol& rhs)
    {
		_nDetectorType				= rhs._nDetectorType;
		_nLedIdx					= rhs._nLedIdx;
		_nLedIntensity				= rhs._nLedIntensity;
		_nLedStablizationTime_us	= rhs._nLedStablizationTime_us;
		_nDetectorIdx				= rhs._nDetectorIdx;
		_nDetectorIntegrationTime_us	= rhs._nDetectorIntegrationTime_us;

		Clear();
        _vSegments.resize(rhs.GetNumSegs());
        for (int nSegIdx = 0; nSegIdx < (int)rhs.GetNumSegs(); nSegIdx++)
            _vSegments[nSegIdx] = rhs.GetSegment(nSegIdx);

        return *this;
    }
    
protected:
  
private:
	uint32_t	_nDetectorType;
	uint32_t	_nLedIdx;
	uint32_t	_nLedIntensity;
	uint32_t	_nLedStablizationTime_us;
	uint32_t	_nDetectorIdx;
	uint32_t	_nDetectorIntegrationTime_us;

    std::vector<Segment> _vSegments;
};

#endif // __PcrProtocol_H
