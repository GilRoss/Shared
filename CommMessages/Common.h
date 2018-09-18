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
    PidParams(uint32_t nKp = 0, uint32_t nKi = 0, uint32_t nKd = 0, int32_t nS = 1000, int32_t nY = 0)
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

    void            SetKp(uint32_t n)           { _nKp = n; }
    uint32_t        GetKp() const               { return _nKp; }
    void            SetKi(uint32_t n)           { _nKi = n; }
    uint32_t        GetKi() const               { return _nKi; }
    void            SetKd(uint32_t n)           { _nKd = n; }
    uint32_t        GetKd() const               { return _nKd; }
    void            SetSlope(int32_t n)         { _nSlope_m = n; }
    int32_t         GetSlope() const            { return _nSlope_m; }
    void            SetYIntercept(int32_t n)    { _nYIntercept_m = n; }
    int32_t         GetYIntercept() const       { return _nYIntercept_m; }

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
        uint32_t*   pSrc = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);

        _nKp            = swap_uint32(*pSrc++);
        _nKi            = swap_uint32(*pSrc++);
        _nKd            = swap_uint32(*pSrc++);
        _nSlope_m       = swap_uint32(*pSrc++);
        _nYIntercept_m  = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);

        *pDst++ = swap_uint32(_nKp);
        *pDst++ = swap_uint32(_nKi);
        *pDst++ = swap_uint32(_nKd);
        *pDst++ = swap_uint32(_nSlope_m);
        *pDst++ = swap_uint32(_nYIntercept_m);
    }

protected:

private:
    uint32_t        _nKp;
    uint32_t        _nKi;
    uint32_t        _nKd;
    int32_t         _nSlope_m;      //y = mx + b.
    int32_t         _nYIntercept_m; //y = mx + b.
};


#endif // __Common_H
