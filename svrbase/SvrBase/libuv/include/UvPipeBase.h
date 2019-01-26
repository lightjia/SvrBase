#ifndef __CUVPIPEBASE__H_
#define __CUVPIPEBASE__H_
#include "UvBase.h"
#include <string>
class CUvPipeBase :
    public CUvBase
{
public:
    CUvPipeBase();
    virtual ~CUvPipeBase();

public:
    void SetPipeParam(std::string strPipeName, int iIpc);

protected:
    std::string mstrPipeName;
    int miIpc;//ÊÇ·ñ¿ä½ø³Ì
};

#endif