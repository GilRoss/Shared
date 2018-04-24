#ifndef __HostMsgFactory_H
#define __HostMsgFactory_H

#include        <cstdint>
#include        "HostMessages.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HostMsgFactory
{
public:
    HostMsgFactory()
    {
    }
    
    //use GetHostMsg method to get object of type HostMessage 
    static HostMsg* GetHostMsg(uint8_t* pMsgBuf)
    {
        HostMsg* pHostMsg = NULL;

        //Message ID is in the first 4 bytes of the message.
        uint32_t nMsgId = *((uint32_t*)pMsgBuf);

        if (nMsgId == HostMsg::MakeMsgId('G', 'S', 't', 't'))
            pHostMsg = new GetStatusReq;

        if (nMsgId == HostMsg::MakeMsgId('S', 'R', 'u', 'n'))
            pHostMsg = new StartRunReq;

        return pHostMsg;
    }
        
protected:
  
private:
};

#endif // __HostMsgFactory_H
