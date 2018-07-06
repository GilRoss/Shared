#ifndef __HostMessages_H
#define __HostMessages_H

#include <cstdint>
#include "StreamingObj.h"
#include "SysStatus.h"
#include "PcrProtocol.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ErrCode: uint32_t
{
    kNoError = 0,
	kDeviceCommErr,
	kRunInProgressErr,
	kInvalidCmdParams,
	kMemoryMappingErr,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum PidType : uint32_t
{
    kTemperature,
    kCurrent
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HostMsg : public StreamingObj
{
public:
    enum {kMaxRequestSize = 1 * 1024, kMaxResponseSize = 1 * 1024};
    
    HostMsg(uint32_t nMsgId = 0)
        : StreamingObj(nMsgId)
		, _nSize(0)
		, _nTransId(0)
		, _nError(ErrCode::kNoError)
    {
    }
 
    virtual ~HostMsg()
    {
    }

	void            SetMsgSize(uint32_t nSize)	    { _nSize = nSize; }
	uint32_t        GetMsgSize() const				{ return _nSize; }
	void            SetTransId(uint32_t nTransId)   {_nTransId = nTransId;}
    uint32_t        GetTransId() const              {return _nTransId;}
    void            SetError(ErrCode nError)        {_nError = nError;}
    ErrCode         GetError() const				{ return _nError; }

    void    SetResponseHeader(HostMsg& requestMsg, ErrCode nErrCode = ErrCode::kNoError)
    {
        SetObjId(requestMsg.GetObjId());
        SetMsgSize(GetStreamSize());
        SetTransId(requestMsg.GetTransId());
        SetError(nErrCode);
    }
	
	virtual uint32_t GetStreamSize() const 
	{
		return StreamingObj::GetStreamSize() + sizeof(_nSize) + sizeof(_nTransId) + sizeof(_nError);
	}
	
	virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		_nSize      = swap_uint32(*pSrc++);
		_nTransId   = swap_uint32(*pSrc++);
		_nError     = (ErrCode)swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(&pData[StreamingObj::GetStreamSize()]);
		*pDst++ = swap_uint32(_nSize);
		*pDst++ = swap_uint32(_nTransId);
		*pDst++ = swap_uint32(_nError);
    }
        
protected:
  
private:
	uint32_t    _nSize;
	uint32_t    _nTransId;
	ErrCode     _nError;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetStatusRes : public HostMsg
{
public:
    GetStatusRes()
        :HostMsg(MakeObjId('G', 'S', 't', 't'))
    {
    }

    virtual ~GetStatusRes()
    {
    }

    void        SetSysStatus(const SysStatus& ss)	{ _sysStatus = ss;}
    SysStatus*  GetSysStatusPtr()					{return &_sysStatus;}

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = HostMsg::GetStreamSize();
		nSize += _sysStatus.GetStreamSize();
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		_sysStatus << &pData[HostMsg::GetStreamSize()];
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        _sysStatus >> &pData[HostMsg::GetStreamSize()];
    }

protected:
  
private:
    SysStatus	_sysStatus;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetOpticsRecsReq : public HostMsg
{
public:
	GetOpticsRecsReq()
		:HostMsg(MakeObjId('G', 'O', 'p', 't'))
	{
	}

	virtual ~GetOpticsRecsReq()
	{
	}

	void        SetSiteIdx(uint32_t n)          { _nSiteIdx = n; }
	uint32_t	GetSiteIdx() const  	        { return _nSiteIdx; }
	void    	SetFirstRecToReadIdx(uint32_t n){ _nFirstRecToReadIdx = n; }
	uint32_t	GetFirstRecToReadIdx() const  	{ return _nFirstRecToReadIdx; }
	void    	SetNumRecsToRead(uint32_t n)    { _nNumRecsToRead = n; }
	uint32_t	GetNumRecsToRead() const  	    { return _nNumRecsToRead; }

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nSiteIdx);
		nSize += sizeof(_nFirstRecToReadIdx);
		nSize += sizeof(_nNumRecsToRead);
		return nSize;
	}

	virtual void     operator<<(const uint8_t* pData)
	{
		HostMsg::operator<<(pData);
		uint32_t*	pSrc = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);
        _nSiteIdx		        = swap_uint32(*pSrc++);
        _nFirstRecToReadIdx	    = swap_uint32(*pSrc++);
        _nNumRecsToRead		    = swap_uint32(*pSrc++);
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);
        *pDst++ = swap_uint32(_nSiteIdx);
        *pDst++ = swap_uint32(_nFirstRecToReadIdx);
        *pDst++ = swap_uint32(_nNumRecsToRead);
	}

protected:

private:
	uint32_t    _nSiteIdx;
	uint32_t    _nFirstRecToReadIdx;
	uint32_t    _nNumRecsToRead;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetOpticsRecsRes : public HostMsg
{
public:
	GetOpticsRecsRes()
		:HostMsg(MakeObjId('G', 'O', 'p', 't'))
	{
	}

	virtual ~GetOpticsRecsRes()
	{
	}

	uint32_t			GetNumOpticsRecs()						{ return (uint32_t)_arOpticsRecs.size(); }
	const OpticsRec&	GetOpticsRec(uint32_t nIdx)			{ return _arOpticsRecs[nIdx]; }
	void				AddOpticsRec(const OpticsRec& rec)	{ return _arOpticsRecs.push_back(rec); }
	void				ClearAllOpticsRecs()					{ _arOpticsRecs.clear(); }

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = HostMsg::GetStreamSize();
		nSize += sizeof(uint32_t);
		nSize += (uint32_t)_arOpticsRecs.size() * sizeof(OpticsRec);
		return nSize;
	}

	virtual void     operator<<(const uint8_t* pData)
	{
		HostMsg::operator<<(pData);
		uint32_t*	pSrc = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		_arOpticsRecs.resize(swap_uint32(*pSrc++));
		for (int i = 0; i < (int)_arOpticsRecs.size(); i++)
		{
			_arOpticsRecs[i]._nTimeTag_ms		= swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nCycleIdx	        = swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nDarkRead		    = swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nIlluminatedRead	= swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nShuttleTemp_mC	= swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nRefIlluminatedRead = swap_uint32(*pSrc++);
			_arOpticsRecs[i]._nRefDarkRead      = swap_uint32(*pSrc++);
		}
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		*pDst++ = swap_uint32((uint32_t)_arOpticsRecs.size());
		for (int i = 0; i < (int)_arOpticsRecs.size(); i++)
		{
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nTimeTag_ms);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nCycleIdx);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nDarkRead);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nIlluminatedRead);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nShuttleTemp_mC);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nRefIlluminatedRead);
			*pDst++ = swap_uint32(_arOpticsRecs[i]._nRefDarkRead);
		}
	}

protected:

private:
	std::vector<OpticsRec>	_arOpticsRecs;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetPidParamsRes : public HostMsg
{
public:
    GetPidParamsRes()
        :HostMsg(MakeObjId('G', 'P', 'i', 'd'))
        , _nPidType(kTemperature)
        , _nPGain(0)
        , _nIGain(0)
        , _nDGain(0)
    {
    }

    virtual ~GetPidParamsRes()
    {
    }

    void            SetType(PidType n)      { _nPidType = n; }
    PidType         GetType()               { return _nPidType; }
    void            SetPGain(uint32_t n)    { _nPGain = n; }
    uint32_t        GetPGain()              { return _nPGain; }
    void            SetIGain(uint32_t n)    { _nIGain = n; }
    uint32_t        GetIGain()              { return _nIGain; }
    void            SetDGain(uint32_t n)    { _nDGain = n; }
    uint32_t        GetDGain()              { return _nDGain; }

    virtual uint32_t GetStreamSize() const
    {
        uint32_t nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_nPidType);
        nSize += sizeof(_nPGain);
        nSize += sizeof(_nIGain);
        nSize += sizeof(_nDGain);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

        _nPidType   = (PidType)swap_uint32(*pSrc++);
        _nPGain     = swap_uint32(*pSrc++);
        _nIGain     = swap_uint32(*pSrc++);
        _nDGain     = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

        *pDst++ = swap_uint32(_nPidType);
        *pDst++ = swap_uint32(_nPGain);
        *pDst++ = swap_uint32(_nIGain);
        *pDst++ = swap_uint32(_nDGain);
    }

protected:

private:
    PidType         _nPidType;
    uint32_t        _nPGain;
    uint32_t        _nIGain;
    uint32_t        _nDGain;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetPidParamsReq : public HostMsg
{
public:
    GetPidParamsReq()
        :HostMsg(MakeObjId('G', 'P', 'i', 'd'))
        , _nPidType(kTemperature)
    {
    }

    virtual ~GetPidParamsReq()
    {
    }

    void            SetType(PidType n)      { _nPidType = n; }
    PidType         GetType()               { return _nPidType; }

    virtual uint32_t GetStreamSize() const
    {
        uint32_t nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_nPidType);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t*   pSrc = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

        _nPidType   = (PidType)swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t*   pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

        *pDst++ = swap_uint32(_nPidType);
    }

protected:

private:
    PidType         _nPidType;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetThermalRecsRes : public HostMsg
{
public:
	GetThermalRecsRes()
		:HostMsg(MakeObjId('G', 'T', 'h', 'm'))
	{
	}

	virtual ~GetThermalRecsRes()
	{
	}

	uint32_t			GetNumThermalRecs()						{ return (uint32_t)_arThermalRecs.size(); }
	const ThermalRec&	GetThermalRec(uint32_t nIdx)			{ return _arThermalRecs[nIdx]; }
	void				AddThermalRec(const ThermalRec& rec)	{ return _arThermalRecs.push_back(rec); }
	void				ClearAllThermalRecs()					{ _arThermalRecs.clear(); }

	virtual uint32_t GetStreamSize() const
	{
		uint32_t nSize = HostMsg::GetStreamSize();
		nSize += sizeof(uint32_t);
		nSize += (uint32_t)_arThermalRecs.size() * sizeof(ThermalRec);
		return nSize;
	}

	virtual void     operator<<(const uint8_t* pData)
	{
		HostMsg::operator<<(pData);
		uint32_t*	pSrc = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		_arThermalRecs.resize(swap_uint32(*pSrc++));
		for (int i = 0; i < (int)_arThermalRecs.size(); i++)
		{
			_arThermalRecs[i]._nTimeTag_ms	= swap_uint32(*pSrc++);
			_arThermalRecs[i]._nChan1_mC	= swap_uint32(*pSrc++);
			_arThermalRecs[i]._nChan2_mC	= swap_uint32(*pSrc++);
			_arThermalRecs[i]._nChan3_mC	= swap_uint32(*pSrc++);
			_arThermalRecs[i]._nChan4_mC	= swap_uint32(*pSrc++);
			_arThermalRecs[i]._nCurrent_mA	= swap_uint32(*pSrc++);
		}
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		*pDst++ = swap_uint32((uint32_t)_arThermalRecs.size());
		for (int i = 0; i < (int)_arThermalRecs.size(); i++)
		{
			*pDst++ = swap_uint32(_arThermalRecs[i]._nTimeTag_ms);
			*pDst++ = swap_uint32(_arThermalRecs[i]._nChan1_mC);
			*pDst++ = swap_uint32(_arThermalRecs[i]._nChan2_mC);
			*pDst++ = swap_uint32(_arThermalRecs[i]._nChan3_mC);
			*pDst++ = swap_uint32(_arThermalRecs[i]._nChan4_mC);
			*pDst++ = swap_uint32(_arThermalRecs[i]._nCurrent_mA);
		}
	}

protected:

private:
	std::vector<ThermalRec>	_arThermalRecs;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoadPcrProtocolReq : public HostMsg
{
public:
    LoadPcrProtocolReq()
        :HostMsg(MakeObjId('L', 'd', 'P', 'r'))
        ,_nSiteIdx(0)
    {
    }

    virtual ~LoadPcrProtocolReq()
    {
    }
    
    void                SetSiteIdx(uint32_t nSiteIdx)		{_nSiteIdx = nSiteIdx;}
    uint32_t            GetSiteIdx() const					{return _nSiteIdx;}
    void                SetPcrProtocol(const PcrProtocol& p){_pcrProtocol = p;}
    const PcrProtocol&  GetPcrProtocol()                    {return _pcrProtocol;}

	virtual uint32_t GetStreamSize() const
	{
		return HostMsg::GetStreamSize() + sizeof(_nSiteIdx) + _pcrProtocol.GetStreamSize();
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        _pcrProtocol << &pData[HostMsg::GetStreamSize() + 0];
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        _pcrProtocol >> &pData[HostMsg::GetStreamSize() + 0];
    }

protected:
  
private:
    uint32_t        _nSiteIdx;
    PcrProtocol     _pcrProtocol;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetOpticsLedReq : public HostMsg
{
public:
    SetOpticsLedReq()
        :HostMsg(MakeObjId('S', 'O', 'L', 'd'))
        ,_nSiteIdx(0)
        ,_nChanIdx(0)
        ,_nIntensity(0)
        ,_nDuration_us(0)
    {
    }

    virtual ~SetOpticsLedReq()
    {
    }

    void        SetSiteIdx(uint32_t nSiteIdx)       {_nSiteIdx = nSiteIdx;}
    uint32_t    GetSiteIdx() const                  {return _nSiteIdx;}
    void        SetChanIdx(uint32_t nChanIdx)       {_nChanIdx = nChanIdx;}
    uint32_t    GetChanIdx() const                  {return _nChanIdx;}    
    void        SetIntensity(uint32_t nIntensity)   {_nIntensity = nIntensity;}
    uint32_t    GetIntensity() const                {return _nIntensity;}    
    void        SetDuration(uint32_t nDur_us)		{_nDuration_us = nDur_us;}
    uint32_t    GetDuration() const                 {return _nDuration_us;}    

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nSiteIdx);
		nSize += sizeof(_nChanIdx);
		nSize += sizeof(_nIntensity);
		nSize += sizeof(_nDuration_us);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nSiteIdx	    = swap_uint32(*pSrc++);
		_nChanIdx	    = swap_uint32(*pSrc++);
		_nIntensity	    = swap_uint32(*pSrc++);
		_nDuration_us	= swap_uint32(*pSrc++);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = swap_uint32(_nSiteIdx);
		*pDst++ = swap_uint32(_nChanIdx);
		*pDst++ = swap_uint32(_nIntensity);
		*pDst++ = swap_uint32(_nDuration_us);
	}

protected:
  
private:
    uint32_t    _nSiteIdx;
    uint32_t    _nChanIdx;
    uint32_t    _nIntensity;
    uint32_t    _nDuration_us;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetOpticsDiodeReq : public HostMsg
{
public:
    GetOpticsDiodeReq()
        :HostMsg(MakeObjId('G', 'O', 'D', 'i'))
        ,_nDiodeIdx(0)
    {
    }

    virtual ~GetOpticsDiodeReq()
    {
    }

    void        SetDiodeIdx(uint32_t nDiodeIdx)       {_nDiodeIdx = nDiodeIdx;}
    uint32_t    GetDiodeIdx() const                  {return _nDiodeIdx;}

    virtual uint32_t GetStreamSize() const
    {
        int nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_nDiodeIdx);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
        _nDiodeIdx       = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
        *pDst++ = swap_uint32(_nDiodeIdx);
    }

protected:

private:
    uint32_t    _nDiodeIdx;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetOpticsDiodeRes : public HostMsg
{
public:
    GetOpticsDiodeRes()
        :HostMsg(MakeObjId('G', 'O', 'D', 'i'))
        ,_diodeValue(0)
    {
    }

    virtual ~GetOpticsDiodeRes()
    {
    }

    void        SetDiodeValue(uint32_t ndiodeValue)       {_diodeValue = ndiodeValue;}
    uint32_t    GetDiodeValue() const                  {return _diodeValue;}

    virtual uint32_t GetStreamSize() const
    {
        int nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_diodeValue);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
        _diodeValue       = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
        *pDst++ = swap_uint32(_diodeValue);
    }

protected:

private:
    uint32_t    _diodeValue;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ReadOpticsReq : public HostMsg
{
public:
    ReadOpticsReq()
        :HostMsg(MakeObjId('R', 'O', 'p', 't'))
        ,_nLedIdx(0)
        ,_nDiodeIdx(0)
        ,_nLedIntensity(0)
        ,_nIntegrationTime_us(0)
    {
    }

    virtual ~ReadOpticsReq()
    {
    }

    void        SetLedIdx(uint32_t nLedIdx)       {_nLedIdx = nLedIdx;}
    uint32_t    GetLedIdx() const                  {return _nLedIdx;}
    void        SetDiodeIdx(uint32_t nDiodeIdx)       {_nDiodeIdx = nDiodeIdx;}
    uint32_t    GetDiodeIdx() const                  {return _nDiodeIdx;}
    void        SetLedIntensity(uint32_t nLedIntensity)       {_nLedIntensity = nLedIntensity;}
    uint32_t    GetLedIntensity() const                  {return _nLedIntensity;}
    void        SetIntegrationTime(uint32_t nIntegrationTime_us)       {_nIntegrationTime_us = nIntegrationTime_us;}
    uint32_t    GetIntegrationTime() const                  {return _nIntegrationTime_us;}

    virtual uint32_t GetStreamSize() const
    {
        int nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_nLedIdx);
        nSize += sizeof(_nDiodeIdx);
        nSize += sizeof(_nLedIntensity);
        nSize += sizeof(_nIntegrationTime_us);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
        _nLedIdx                = swap_uint32(*pSrc++);
        _nDiodeIdx              = swap_uint32(*pSrc++);
        _nLedIntensity          = swap_uint32(*pSrc++);
        _nIntegrationTime_us    = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
        *pDst++ = swap_uint32(_nLedIdx);
        *pDst++ = swap_uint32(_nDiodeIdx);
        *pDst++ = swap_uint32(_nLedIntensity);
        *pDst++ = swap_uint32(_nIntegrationTime_us);
    }

protected:

private:
    uint32_t    _nLedIdx;
    uint32_t    _nDiodeIdx;
    uint32_t    _nLedIntensity;
    uint32_t    _nIntegrationTime_us;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ReadOpticsRes : public HostMsg
{
public:
    ReadOpticsRes()
        :HostMsg(MakeObjId('R', 'O', 'p', 't'))
        ,_diodeValue(0)
    {
    }

    virtual ~ReadOpticsRes()
    {
    }

    void        SetDiodeValue(uint32_t ndiodeValue)    {_diodeValue = ndiodeValue;}
    uint32_t    GetDiodeValue() const                  {return _diodeValue;}

    virtual uint32_t GetStreamSize() const
    {
        int nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_diodeValue);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
        _diodeValue       = swap_uint32(*pSrc++);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
        *pDst++ = swap_uint32(_diodeValue);
    }

protected:

private:
    uint32_t    _diodeValue;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetManControlSetpointReq : public HostMsg
{
public:
    SetManControlSetpointReq()
        :HostMsg(MakeObjId('M', 'c', 'S', 'p'))
        ,_nSiteIdx(0)
        ,_nSetpoint_mC(0)
    {
    }

    virtual ~SetManControlSetpointReq()
    {
    }
    
    void        SetSiteIdx(uint32_t nSiteIdx)   {_nSiteIdx = nSiteIdx;}
    uint32_t    GetSiteIdx() const              {return _nSiteIdx;}    
    void        SetSetpoint(int32_t n)          {_nSetpoint_mC = n;}
    int32_t     GetSetpoint() const             {return _nSetpoint_mC;}

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nSiteIdx);
		nSize += sizeof(_nSetpoint_mC);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nSiteIdx	    = swap_uint32(*pSrc++);
		_nSetpoint_mC	= swap_uint32(*pSrc++);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = swap_uint32(_nSiteIdx);
		*pDst++ = swap_uint32(_nSetpoint_mC);
	}

protected:
  
private:
    uint32_t    _nSiteIdx;
    int32_t     _nSetpoint_mC;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetPidParamsReq : public HostMsg
{
public:
    SetPidParamsReq()
        :HostMsg(MakeObjId('S', 'P', 'i', 'd'))
		, _nPidType(PidType::kTemperature)
		, _nKp(0)
		, _nKi(0)
        , _nKd(0)
    {
    }

    virtual ~SetPidParamsReq()
    {
    }
    
	void        SetType(PidType n)      { _nPidType = n; }
	PidType	    GetType()               { return _nPidType; }
	void        SetPGain(uint32_t n)    { _nKp = n; }
	uint32_t    GetPGain()              { return _nKp; }
	void        SetIGain(uint32_t n)    { _nKi = n; }
	uint32_t    GetIGain()              { return _nKi; }
	void        SetDGain(uint32_t n)    { _nKd = n; }
	uint32_t    GetDGain()              { return _nKd; }

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nPidType);
		nSize += sizeof(_nKp);
		nSize += sizeof(_nKi);
		nSize += sizeof(_nKd);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nPidType	= (PidType)swap_uint32(*pSrc++);
		_nKp		= swap_uint32(*pSrc++);
		_nKi		= swap_uint32(*pSrc++);
		_nKd		= swap_uint32(*pSrc++);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = swap_uint32(_nPidType);
		*pDst++ = swap_uint32(_nKp);
		*pDst++ = swap_uint32(_nKi);
		*pDst++ = swap_uint32(_nKd);
	}

protected:
  
private:
    PidType     _nPidType;
    uint32_t    _nKp;
    uint32_t    _nKi;
    uint32_t    _nKd;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StartRunReq : public HostMsg
{
public:
    StartRunReq()
        :HostMsg(MakeObjId('S', 'R', 'u', 'n'))
        ,_nSiteIdx(0)
        ,_bMeerstetterPid(true)
    {
    }

    virtual ~StartRunReq()
    {
    }
    
    void        SetSiteIdx(uint32_t nSiteIdx)   {_nSiteIdx = nSiteIdx;}
    uint32_t    GetSiteIdx() const              {return _nSiteIdx;}    
    void        SetMeerstetterPidFlg(bool b)    {_bMeerstetterPid = b;}
    bool        GetMeerstetterPidFlg() const    {return _bMeerstetterPid;}    

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nSiteIdx);
		nSize += sizeof(uint32_t);	//_bMeerstetterPid
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nSiteIdx			= swap_uint32(*pSrc++);
		_bMeerstetterPid	= (swap_uint32(*pSrc++) != 0);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = swap_uint32(_nSiteIdx);
		*pDst++ = swap_uint32(_bMeerstetterPid ? 1 : 0);
	}

protected:
  
private:
    uint32_t    _nSiteIdx;
    uint32_t    _bMeerstetterPid;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StopRunReq : public HostMsg
{
public:
    StopRunReq()
        :HostMsg(MakeObjId('S', 't', 'o', 'p'))
        ,_nSiteIdx(0)
    {
    }

    virtual ~StopRunReq()
    {
    }
    
    void        SetSiteIdx(uint32_t nSiteIdx)   {_nSiteIdx = nSiteIdx;}
    uint32_t    GetSiteIdx() const              {return _nSiteIdx;}

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nSiteIdx);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nSiteIdx			= swap_uint32(*pSrc++);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = swap_uint32(_nSiteIdx);
	}

protected:
  
private:
    uint32_t    _nSiteIdx;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PauseRunReq : public HostMsg
{
public:
    PauseRunReq()
        :HostMsg(MakeObjId('P', 'a', 'u', 's'))
        ,_nSiteIdx(0)
    {
    }

    virtual ~PauseRunReq()
    {
    }

    void        SetSiteIdx(uint32_t nSiteIdx)  {_nSiteIdx = nSiteIdx;}
    uint32_t    GetSiteIdx() const             {return _nSiteIdx;}
    void        SetPausedFlg(bool b)            {_bPaused = b;}
    bool        GetPausedFlg() const            {return _bPaused;}

    virtual uint32_t GetStreamSize() const
    {
        int nSize = HostMsg::GetStreamSize();
        nSize += sizeof(_nSiteIdx);
        nSize += sizeof(_bPaused);
        return nSize;
    }

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
        uint32_t* pSrc = (uint32_t*)(pData + HostMsg::GetStreamSize());
        _nSiteIdx           = swap_uint32(*pSrc++);
        _bPaused            = (swap_uint32(*pSrc++) != 0);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        HostMsg::operator>>(pData);
        uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
        *pDst++ = swap_uint32(_nSiteIdx);
        *pDst++ = swap_uint32(_bPaused ? 1 : 0);

    }

protected:

private:
    uint32_t    _nSiteIdx;
    uint32_t    _bPaused;
};

#endif // __HostMessages_H
