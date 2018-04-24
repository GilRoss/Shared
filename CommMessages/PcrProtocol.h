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
        , _nHoldTime_ms(0)
        , _bMelt(false)
        , _nRampRate_mCPerSec(0)
    {
    }
    
    void        SetRampRate(int32_t nRampRate_mCPerSec) {_nRampRate_mCPerSec = nRampRate_mCPerSec;}
    int32_t     GetRampRate() const                     {return _nRampRate_mCPerSec;}
    void        SetTargetTemp(int32_t nTargetTemp_mC)   {_nTargetTemp_mC = nTargetTemp_mC;}
    int32_t     GetTargetTemp() const                   {return _nTargetTemp_mC;}
    void        SetHoldTime(uint32_t nHoldTime_ms)      {_nHoldTime_ms = nHoldTime_ms;}
    uint32_t    GetHoldTime() const                     {return _nHoldTime_ms;}
	void        SetMeltFlg(bool b)						{ _bMelt = b; }
	bool        GetMeltFlg() const						{ return _bMelt; }
	uint32_t    GetNumOpticsChans()					    { return sizeof(_arReadChanFlgs); }
	void        SetReadChanFlg(uint32_t nChanIdx, bool b) { _arReadChanFlgs[nChanIdx] = b; }
	bool        GetReadChanFlg(uint32_t nChanIdx) const { return _arReadChanFlgs[nChanIdx]; }

    void        SetTargetTemp(int32_t nTargetTemp_mC, uint32_t nHoldTime_ms,
                              bool bMeasureFluorescence = false, int32_t nRampRate_mCPerSec = 0)
                {
                    _nTargetTemp_mC     = nTargetTemp_mC;
                    _nHoldTime_ms       = nHoldTime_ms;
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
        _nTargetTemp_mC     = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 0]);
        _nHoldTime_ms       = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 4]);
		_bMelt				= *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 8]) == 1;
        _nRampRate_mCPerSec = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 12]);
		
		int nOffset = StreamingObj::GetStreamSize() + (4 * sizeof(uint32_t));
		for (int i = 0; i < kNumOpticChans; i++)
		{
			_arReadChanFlgs[i] = *((uint32_t*)&pData[nOffset]) == 1;
			nOffset += sizeof(uint32_t);
		}
	}

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 0])  = _nTargetTemp_mC;
        *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 4])  = _nHoldTime_ms;
        *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 8])  = _bMelt ? 1 : 0;
		*((uint32_t*)&pData[StreamingObj::GetStreamSize() + 12]) = _nRampRate_mCPerSec;

		int nOffset = StreamingObj::GetStreamSize() + (4 * sizeof(uint32_t));
		for (int i = 0; i < kNumOpticChans; i++)
		{
			*((uint32_t*)&pData[nOffset]) = _arReadChanFlgs[i] ? 1 : 0;
			nOffset += sizeof(uint32_t);
		}
	}

protected:
  
private:
    int32_t     _nTargetTemp_mC;
    uint32_t    _nHoldTime_ms;
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

    void        SetNumCycles(uint32_t nNumCycs) {_nNumCycles = nNumCycs;}
    uint32_t    GetNumCycles() const            {return _nNumCycles;}
    uint32_t    GetNumSteps() const             {return (uint32_t)_vSteps.size();}
    Step        GetStep(uint32_t stpIdx) const  {return _vSteps[stpIdx];}
    void        PushStep(const Step& stp)       {_vSteps.push_back(stp);}
    void        Clear()                         {_vSteps.clear();}
    
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
		_nNumCycles = *((uint32_t*)&pData[StreamingObj::GetStreamSize()]);
		int nNumSteps = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + sizeof(uint32_t)]);
		_vSteps.clear();

		uint32_t nOffset = StreamingObj::GetStreamSize() + sizeof(_nNumCycles) + sizeof(uint32_t);
		for (int i = 0; i < nNumSteps; i++)
        {
			Step step;
			step << &pData[nOffset];
			_vSteps.push_back(step);
            nOffset += step.GetStreamSize();
        }
    }

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		*((uint32_t*)&pData[StreamingObj::GetStreamSize()]) = _nNumCycles;
		*((uint32_t*)&pData[StreamingObj::GetStreamSize() + sizeof(uint32_t)]) = (uint32_t)_vSteps.size();

		uint32_t nOffset = StreamingObj::GetStreamSize() + sizeof(_nNumCycles) + sizeof(uint32_t);
		for (int i = 0; i < (int)_vSteps.size(); i++)
        {
            _vSteps[i] >> &pData[nOffset];
            nOffset += _vSteps[i].GetStreamSize();
        }
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

    uint32_t    GetNumSegs() const              {return (uint32_t)_vSegments.size();}
    Segment     GetSegment(uint32_t idx) const  {return _vSegments[idx];}
    void        AddSegment(const Segment& seg)  {_vSegments.push_back(seg);}
    void        Clear()                         {_vSegments.clear();}
    
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
        _vSegments.clear();

		int nNumSegs = *((uint32_t*)&pData[StreamingObj::GetStreamSize()]);
		uint32_t nOffset = StreamingObj::GetStreamSize() + sizeof(uint32_t);
		for (int i = 0; i < nNumSegs; i++)
        {
			Segment seg;
			seg << &pData[nOffset];
            _vSegments.push_back(seg);
            nOffset += seg.GetStreamSize();
        }
    }

    virtual void    operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		*((uint32_t*)&pData[StreamingObj::GetStreamSize()]) = (uint32_t)_vSegments.size();
		uint32_t nOffset = StreamingObj::GetStreamSize() + sizeof(uint32_t);
		for (int i = 0; i < (int)_vSegments.size(); i++)
        {
            _vSegments[i] >> &pData[nOffset];
            nOffset += _vSegments[i].GetStreamSize();
        }
    }

    PcrProtocol& operator=(const PcrProtocol& rhs)
    {
        Clear();
        for (int nSegIdx = 0; nSegIdx < (int)rhs.GetNumSegs(); nSegIdx++)
            AddSegment(rhs.GetSegment(nSegIdx));

        return *this;
    }
    
protected:
  
private:
    std::vector<Segment> _vSegments;
};

#endif // __PcrProtocol_H
