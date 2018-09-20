#ifndef __StreamingObj_H
#define __StreamingObj_H

#include        <cstdint>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StreamingObj
{
public:
    StreamingObj(uint32_t nObjId = 0, uint32_t nVersion = 1)
        : _nObjId(nObjId)
        , _nVersion(nVersion)
    {
    }

    void            SetObjId(uint32_t nMsgId)       {_nObjId = nMsgId;}
    uint32_t        GetObjId() const                {return _nObjId;}
    void            SetVersion(uint32_t nVersion)   {_nVersion = nVersion;}
	uint32_t        GetVersion() const				{ return _nVersion; }

#ifdef LITTLE_ENDIAN
	//! Byte swap unsigned int
	uint32_t swap_uint32(uint32_t val)
	{
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
		return (val << 16) | (val >> 16);
	}

	//! Byte swap float
	const uint8_t*	SwapFromStream(const uint8_t* pSrc, float* pVal)
	{
		uint8_t* pTemp = (uint8_t*)pVal;
		pTemp[0] = pSrc[3];
		pTemp[1] = pSrc[2];
		pTemp[2] = pSrc[1];
		pTemp[3] = pSrc[0];
		return pSrc + sizeof(float);
	}

	//! Byte swap float
	uint8_t*	SwapToStream(uint8_t* pDst, float nVal)
	{
		uint8_t* pTemp = (uint8_t*)&nVal;
		pDst[0] = pTemp[3];
		pDst[1] = pTemp[2];
		pDst[2] = pTemp[1];
		pDst[3] = pTemp[0];
		return pDst + sizeof(float);
}

#elif BIG_ENDIAN
	uint32_t	swap_uint32(uint32_t val) { return val; }

	const uint8_t*	SwapFromStream(const uint8_t* pSrc, float* pVal)
	{
		*pVal = *((float*)pSrc);
		return pSrc + sizeof(float);
	}

	uint8_t*	SwapToStream(uint8_t* pDst, float nVal)
	{
		*((float*)pDst) = nVal;
		return pDst + sizeof(float);
	}
#endif


	virtual uint32_t GetStreamSize() const
	{
	    return sizeof(_nObjId) + sizeof(_nVersion);
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        uint32_t*    pSrc = (uint32_t*)pData;
        _nObjId      = swap_uint32(*pSrc++);
        _nVersion    = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        uint32_t*    pDst = (uint32_t*)pData;
        *pDst++      = swap_uint32(_nObjId);
        *pDst++      = swap_uint32(_nVersion);
    }
  
    static uint32_t MakeObjId(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
                {return (((uint32_t)c1) << 24) + (((uint32_t)c2) << 16) + (((uint32_t)c3) << 8) + c4;}

protected:
  
private:
    uint32_t    _nObjId;
    uint32_t    _nVersion;
};

#endif // __StreamingObj_H
