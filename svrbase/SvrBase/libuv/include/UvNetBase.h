#ifndef __CUVNETBASE__H_
#define __CUVNETBASE__H_

#include "UvBase.h"
#include "UvMutex.h"
#include <string>

class CUvNetBase : public CUvBase{
public:
    CUvNetBase();
    virtual ~CUvNetBase();

public:
    int SetNetParam(const char* pIp, unsigned short iPort);
    unsigned short GetPort() { return musPort; }
    std::string& GetIp() { return mstrIp; }

protected:
    unsigned short musPort;
    std::string mstrIp;
    struct sockaddr_in mstLocalAddr;
};

#endif