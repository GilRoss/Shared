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
        , _bMelt(false)
        , _nRampRate_mCPerSec(0)
    {
    }
    
    void        SetRampRate(int32_t nRampRate_mCPerSec) {_nRampRate_mCPerSec = nRampRate_mCPerSec;}
    int32_t     GetRampRate() const                     {return _nRampRate_mCPerSec;}
    void        SetTargetTemp(int32_t nTargetTemp_mC)   {_nTargetTemp_mC = nTargetTemp_mC;}
    int32_t     GetTargetTemp() const                   {return _nTargetTemp_mC;}
    void        SetHoldTimer(uint32_t nHoldTime_ms)     {_nHoldTimer_ms = nHoldTime_ms;}
    uint32_t    GetHoldTimer() const                    {return _nHoldTimer_ms;}
	void        SetMeltFlg(bool b)						{ _bMelt = b; }
	bool        GetMeltFlg() const						{ return _bMelt; }
	uint32_t    GetNumOpticsChans()					    { return sizeof(_arReadChanFlgs); }
	void        SetReadChanFlg(uint32_t nChanIdx, bool b) { _arReadChanFlgs[nChanIdx] = b; }
	bool        GetReadChanFlg(uint32_t nChanIdx) const { return _arReadChanFlgs[nChanIdx]; }

    void        SetTargetTemp(int32_t nTargetTemp_mC, uint32_t nHoldTime_ms,
                              bool bMeasureFluorescence = false, int32_t nRampRate_mCPerSec = 0)
                {
                    _nTargetTemp_mC     = nTargetTemp_mC;
                    _nHoldTimer_ms      = nHoldTime_ms;
					_bMelt				= bMeasureFluorescence;
                    _nRampRate_mCPerSec = nRampRate_mCPerSec;
                }
    
	virtual uint32_t GetStreamSize() const
	{
		return StreamingObj::GetStreamSize() + ((4 + kNumOpticChans) * sizeof(uint32_t));
	}

    virtual void    operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		uint32_t*   pSrc = (uint32_t*)(pData + StreamingObj::GetStreamSize());
		_nTargetTemp_mC		= swap_uint32(*pSrc++);
        _nHoldTimer_ms      = swap_uint32(*pSrc++);
		_bMelt				= swap_uint32(*pSrc++) != 0;
        _nRampRate_mCPerSec = swap_uint32(*pSrc++);
		
		for (int i = 0; i < kNumOpticChans; i++)
			_arReadChanFlgs[i] = swap_uint32(*pSrc++) != 0;
	}

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		*pDst++ = swap_uint32(_nTargetTemp_mC);
		*pDst++ = swap_uint32(_nHoldTimer_ms);
		*pDst++ = swap_uint32(_bMelt ? 1 : 0);
		*pDst++ = swap_uint32(_nRampRate_mCPerSec);

		for (int i = 0; i < kNumOpticChans; i++)
			*pDst++ = swap_uint32(_arReadChanFlgs[i] ? 1 : 0);
	}

protected:
  
private:
    int32_t     _nTargetTemp_mC;
    uint32_t    _nHoldTimer_ms;
	bool        _arReadChanFlgs[kNumOpticChans];
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
        uint32_t nSize = StreamingObj::GetStreamSize() + sizeof(_nNumCycles) + sizeof(uint32_t);
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
		*pDst++ = swap_uint32(_vSteps.size());

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
    PcrProtocol()
        :StreamingObj(MakeObjId('P','r','o','t'))
    {
    }

    virtual ~PcrProtocol()
    {
        Clear();
    }

    uint32_t        GetNumSegs() const              {return (uint32_t)_vSegments.size();}
    const Segment&  GetSegment(uint32_t idx) const  {return _vSegments[idx];}
    void            Clear()                         {_vSegments.clear();}

	void AddSegment(const Segment& seg)
	{
		_vSegments.push_back(seg);
	}

    virtual uint32_t        GetStreamSize() const
    {
        uint32_t nSize = StreamingObj::GetStreamSize() + sizeof(uint32_t);
        for (int i = 0; i < (int)_vSegments.size(); i++)
            nSize += _vSegments[i].GetStreamSize();
        
        return nSize;
    }

    virtual void    operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		uint32_t*   pSrc = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);

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

		*pDst++ = swap_uint32((uint32_t)_vSegments.size());
		for (int i = 0; i < (int)_vSegments.size(); i++)
        {
            _vSegments[i] >> (uint8_t*)pDst;
			pDst += _vSegments[i].GetStreamSize() / sizeof(uint32_t);
        }
    }

    PcrProtocol& operator=(const PcrProtocol& rhs)
    {
        Clear();
        _vSegments.resize(rhs.GetNumSegs());
        for (int nSegIdx = 0; nSegIdx < (int)rhs.GetNumSegs(); nSegIdx++)
            _vSegments[nSegIdx] = rhs.GetSegment(nSegIdx);

        return *this;
    }
    
protected:
  
private:
    std::vector<Segment> _vSegments;
};

#endif // __PcrProtocol_H
