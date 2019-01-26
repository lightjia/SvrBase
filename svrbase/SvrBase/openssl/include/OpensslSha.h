#ifndef __COPENSSLSHA__H_
#define __COPENSSLSHA__H_
#include "singleton.h"
class COpensslSha : public CSingleton<COpensslSha>{
    SINGLE_CLASS_INITIAL(COpensslSha);
public:
    ~COpensslSha();
};

#define sOpensslSha COpensslSha::Instance()
#endif