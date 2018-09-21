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
    PidParams(float nKp = 0, float nKi = 0, float nKd = 0, float nS = 1000, float nY = 0)
        : StreamingObj(MakeObjId('P', 'i', 'd','P'))
        , _nKp(nKp)
        , _nKi(nKi)
        , _nKd(nKd)
        , _nSlope_m(nS)
        , _nYIntercept_m(nY)
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
    void         SetSlope(float n)        { _nSlope_m = n; }
	float        GetSlope() const         { return _nSlope_m; }
    void         SetYIntercept(float n)   { _nYIntercept_m = n; }
	float        GetYIntercept() const    { return _nYIntercept_m; }

    virtual uint32_t GetStreamSize() const
    {
        uint32_t nSize = StreamingObj::GetStreamSize();
        nSize += sizeof(_nKp);
        nSize += sizeof(_nKi);
        nSize += sizeof(_nKd);
        nSize += sizeof(_nSlope_m);
        nSize += sizeof(_nYIntercept_m);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        const uint8_t*   pSrc = &pData[StreamingObj::GetStreamSize()];

		pSrc = SwapFromStream(pSrc, &_nKp);
		pSrc = SwapFromStream(pSrc, &_nKi);
		pSrc = SwapFromStream(pSrc, &_nKd);
		pSrc = SwapFromStream(pSrc, &_nSlope_m);
		pSrc = SwapFromStream(pSrc, &_nYIntercept_m);
	}

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint8_t*   pDst = &pData[StreamingObj::GetStreamSize()];

        pDst = SwapToStream(pDst, _nKp);
		pDst = SwapToStream(pDst, _nKi);
		pDst = SwapToStream(pDst, _nKd);
		pDst = SwapToStream(pDst, _nSlope_m);
		pDst = SwapToStream(pDst, _nYIntercept_m);
    }

protected:

private:
    float        _nKp;
	float        _nKi;
	float        _nKd;
	float        _nSlope_m;      //y = mx + b.
	float        _nYIntercept_m; //y = mx + b.
};

#endif // __Common_H
