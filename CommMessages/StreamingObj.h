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

	virtual uint32_t GetStreamSize() const			{ return 2 * sizeof(uint32_t); }

    virtual void     operator<<(const uint8_t* pData)
    {
        _nObjId      = *((uint32_t*)&pData[0]);
        _nVersion    = *((uint32_t*)&pData[4]);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        *((uint32_t*)&pData[0])      = _nObjId;
        *((uint32_t*)&pData[4])      = _nVersion;
    }
  
    static uint32_t MakeObjId(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
                {return (((uint32_t)c4) << 24) + (((uint32_t)c3) << 16) + (((uint32_t)c2) << 8) + c1;}
    
protected:
  
private:
    uint32_t    _nObjId;
    uint32_t    _nVersion;
};

#endif // __StreamingObj_H
