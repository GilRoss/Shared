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
		, _nError(0)
    {
    }
 
    virtual ~HostMsg()
    {
    }

	void            SetMsgSize(uint32_t nSize)	    { _nSize = nSize; }
	uint32_t        GetMsgSize() const				{ return _nSize; }
	void            SetTransId(uint32_t nTransId)   {_nTransId = nTransId;}
    uint32_t        GetTransId() const              {return _nTransId;}
    void            SetError(uint32_t nError)       {_nError = nError;}
	uint32_t        GetError() const				{ return _nError; }
	
	virtual uint32_t GetStreamSize() const 
	{
		return StreamingObj::GetStreamSize() + (3 * sizeof(uint32_t));
	}
	
	virtual void     operator<<(const uint8_t* pData)
    {
        StreamingObj::operator<<(pData);
		_nSize      = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 0]);
		_nTransId   = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 4]);
		_nError     = *((uint32_t*)&pData[StreamingObj::GetStreamSize() + 8]);
    }

    virtual void     operator>>(uint8_t* pData)
    {
        StreamingObj::operator>>(pData);
		*((uint32_t*)&pData[StreamingObj::GetStreamSize() + 0]) = _nSize;
		*((uint32_t*)&pData[StreamingObj::GetStreamSize() + 4]) = _nTransId;
		*((uint32_t*)&pData[StreamingObj::GetStreamSize() + 8]) = _nError;
    }
        
protected:
  
private:
	uint32_t    _nSize;
	uint32_t    _nTransId;
    uint32_t    _nError;
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
        _nSiteIdx		        = *pSrc++;
        _nFirstRecToReadIdx	    = *pSrc++;
        _nNumRecsToRead		    = *pSrc++;
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);
        *pDst++ = _nSiteIdx;
        *pDst++ = _nFirstRecToReadIdx;
        *pDst++ = _nNumRecsToRead;
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

		_arOpticsRecs.resize(*pSrc++);
		for (int i = 0; i < (int)_arOpticsRecs.size(); i++)
		{
			_arOpticsRecs[i]._nTimeTag_ms		= *pSrc++;
			_arOpticsRecs[i]._nCycleIdx	        = *pSrc++;
			_arOpticsRecs[i]._nDarkRead		    = *pSrc++;
			_arOpticsRecs[i]._nIlluminatedRead	= *pSrc++;
			_arOpticsRecs[i]._nShuttleTemp_mC	= *pSrc++;
		}
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		*pDst++ = (uint32_t)_arOpticsRecs.size();
		for (int i = 0; i < (int)_arOpticsRecs.size(); i++)
		{
			*pDst++ = _arOpticsRecs[i]._nTimeTag_ms;
			*pDst++ = _arOpticsRecs[i]._nCycleIdx;
			*pDst++ = _arOpticsRecs[i]._nDarkRead;
			*pDst++ = _arOpticsRecs[i]._nIlluminatedRead;
			*pDst++ = _arOpticsRecs[i]._nShuttleTemp_mC;
		}
	}

protected:

private:
	std::vector<OpticsRec>	_arOpticsRecs;
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

		_arThermalRecs.resize(*pSrc++);
		for (int i = 0; i < (int)_arThermalRecs.size(); i++)
		{
			_arThermalRecs[i]._nTimeTag_ms		= *pSrc++;
			_arThermalRecs[i]._nBlockTemp_mC	= *pSrc++;
			_arThermalRecs[i]._nSinkTemp_mC		= *pSrc++;
			_arThermalRecs[i]._nSampleTemp_mC	= *pSrc++;
			_arThermalRecs[i]._nCurrent_mA		= *pSrc++;
		}
	}

	virtual void     operator>>(uint8_t* pData)
	{
		HostMsg::operator>>(pData);
		uint32_t*	pDst = (uint32_t*)(&pData[HostMsg::GetStreamSize()]);

		*pDst++ = (uint32_t)_arThermalRecs.size();
		for (int i = 0; i < (int)_arThermalRecs.size(); i++)
		{
			*pDst++ = _arThermalRecs[i]._nTimeTag_ms;
			*pDst++ = _arThermalRecs[i]._nBlockTemp_mC;
			*pDst++ = _arThermalRecs[i]._nSinkTemp_mC;
			*pDst++ = _arThermalRecs[i]._nSampleTemp_mC;
			*pDst++ = _arThermalRecs[i]._nCurrent_mA;
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
		_nSiteIdx	    = *pSrc++;
		_nChanIdx	    = *pSrc++;
		_nIntensity	    = *pSrc++;
		_nDuration_us	= *pSrc++;
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = _nSiteIdx;
		*pDst++ = _nChanIdx;
		*pDst++ = _nIntensity;
		*pDst++ = _nDuration_us;
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
		_nSiteIdx	    = *pSrc++;
		_nSetpoint_mC	= *pSrc++;
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = _nSiteIdx;
		*pDst++ = _nSetpoint_mC;
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
        ,_nKp(0)
        ,_nKi(0)
        ,_nKd(0)
    {
    }

    virtual ~SetPidParamsReq()
    {
    }
    
    void        SetKp(uint32_t nKp)         {_nKp = nKp;}
    uint32_t    GetKp() const               {return _nKp;}    
    void        SetKi(uint32_t nKi)         {_nKi = nKi;}
    uint32_t    GetKi() const               {return _nKi;}    
    void        SetKd(uint32_t nKd)         {_nKd = nKd;}
    uint32_t    GetKd() const               {return _nKd;}    

	virtual uint32_t GetStreamSize() const
	{
		int nSize = HostMsg::GetStreamSize();
		nSize += sizeof(_nKp);
		nSize += sizeof(_nKi);
		nSize += sizeof(_nKd);
		return nSize;
	}

    virtual void     operator<<(const uint8_t* pData)
    {
        HostMsg::operator<<(pData);
		uint32_t* pSrc  = (uint32_t*)(pData + HostMsg::GetStreamSize());
		_nKp	= *pSrc++;
		_nKi	= *pSrc++;
		_nKd	= *pSrc++;
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = _nKp;
		*pDst++ = _nKi;
		*pDst++ = _nKd;
	}

protected:
  
private:
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
		_nSiteIdx			= *pSrc++;
		_bMeerstetterPid	= (*pSrc++ != 0);
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = _nSiteIdx;
		*pDst++ = _bMeerstetterPid ? 1 : 0;
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
		_nSiteIdx			= *pSrc++;
	}

    virtual void     operator>>(uint8_t* pData)
    {
		HostMsg::operator>>(pData);
		uint32_t* pDst = (uint32_t*)(pData + HostMsg::GetStreamSize());
		*pDst++ = _nSiteIdx;
	}

protected:
  
private:
    uint32_t    _nSiteIdx;
};

#endif // __HostMessages_H
