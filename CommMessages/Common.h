#ifndef  __Common_H
#define  __Common_H

#include <cstdint>
#include "StreamingObj.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum FluorDetectorType : uint32_t
{
    kPhotoDiode,
    kCamera,
    kTypes
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct OpticsRec
{
public:
    uint32_t    _nTimeTag_ms;
    uint32_t    _nCycleNum;
    uint32_t    _nLedIdx;
    uint32_t    _nDetectorIdx;
    uint32_t    _nDarkRead;
    uint32_t    _nIlluminatedRead;
    uint32_t    _nRefDarkRead;
    uint32_t    _nRefIlluminatedRead;
    int32_t     _nShuttleTemp_mC;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ThermalRec
{
public:
    uint32_t    _nTimeTag_ms;
    int32_t     _nChan1_mC;
    int32_t     _nChan2_mC;
    int32_t     _nChan3_mC;
    int32_t     _nChan4_mC;
    int32_t     _nCurrent_mA;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum PidType : uint32_t
{
    kTemperature,
    kCurrent,
    kNumPidTypes
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PidParams : public StreamingObj
{
public:
    PidParams(float nKp = 0, float nKi = 0, float nKd = 0, float nS = 1000, float nY = 0, float nStabilizationTolerance_C = 0, float nStabilizationTime_s = 0)
        : StreamingObj(MakeObjId('P', 'i', 'd','P'), 2)
        , _nKp(nKp)
        , _nKi(nKi)
        , _nKd(nKd)
        , _nSlope(nS)
        , _nYIntercept(nY)
		, _nStabilizationTolerance_C(nStabilizationTolerance_C)
		, _nStabilizationTime_s(nStabilizationTime_s)

    {
    }

    virtual ~PidParams()
    {
    }

    void         SetKp(float n)           { _nKp = n; }
	float        GetKp() const            { return _nKp; }
    void         SetKi(float n)           { _nKi = n; }
	float        GetKi() const            { return _nKi; }
    void         SetKd(float n)           { _nKd = n; }
	float        GetKd() const            { return _nKd; }
    void         SetSlope(float n)        { _nSlope = n; }
	float        GetSlope() const         { return _nSlope; }
	void         SetYIntercept(float n)				{ _nYIntercept = n; }
	float        GetYIntercept() const				{ return _nYIntercept; }
	void         SetStabilizationTolerance(float n) { _nStabilizationTolerance_C = n; }
	float        GetStabilizationTolerance() const	{ return _nStabilizationTolerance_C; }
	void         SetStabilizationTime(float n)		{ _nStabilizationTime_s = n; }
	float        GetStabilizationTime() const		{ return _nStabilizationTime_s; }

	virtual uint32_t GetStreamSize() const
    {
        uint32_t nSize = StreamingObj::GetStreamSize();
        nSize += sizeof(_nKp);
        nSize += sizeof(_nKi);
        nSize += sizeof(_nKd);
        nSize += sizeof(_nSlope);
		nSize += sizeof(_nYIntercept);
        nSize += sizeof(_nStabilizationTolerance_C);
        nSize += sizeof(_nStabilizationTime_s);
		return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        const uint8_t*   pSrc = &pData[StreamingObj::GetStreamSize()];

		pSrc = SwapFromStream(pSrc, &_nKp);
		pSrc = SwapFromStream(pSrc, &_nKi);
		pSrc = SwapFromStream(pSrc, &_nKd);
		pSrc = SwapFromStream(pSrc, &_nSlope);
		pSrc = SwapFromStream(pSrc, &_nYIntercept);

        //If version 2 or above, overwrite default.
		if (GetVersion() >= 2)
		{
            pSrc = SwapFromStream(pSrc, &_nStabilizationTolerance_C);
            pSrc = SwapFromStream(pSrc, &_nStabilizationTime_s);
		}
	}

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint8_t*   pDst = &pData[StreamingObj::GetStreamSize()];

        pDst = SwapToStream(pDst, _nKp);
		pDst = SwapToStream(pDst, _nKi);
		pDst = SwapToStream(pDst, _nKd);
		pDst = SwapToStream(pDst, _nSlope);
		pDst = SwapToStream(pDst, _nYIntercept);
		pDst = SwapToStream(pDst, _nStabilizationTolerance_C);
		pDst = SwapToStream(pDst, _nStabilizationTime_s);
	}

protected:

private:
    float _nKp;
	float _nKi;
	float _nKd;
	float _nSlope;      //y = mx + b.
	float _nYIntercept; //y = mx + b.
	float _nStabilizationTolerance_C;
	float _nStabilizationTime_s;
};

#endif // __Common_H
