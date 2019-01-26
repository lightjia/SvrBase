#ifndef __CTASK__H_
#define __CTASK__H_
#include "RcObject.h"
class CTask: public CRcObject
{
public:
    virtual int TaskInit() = 0;
    virtual int TaskExcute() = 0;
    virtual int TaskQuit() = 0;
};

#endif