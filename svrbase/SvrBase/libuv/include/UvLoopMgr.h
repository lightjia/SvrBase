#ifndef __CUvLoopMgr__H_
#define __CUvLoopMgr__H_
#include "singleton.h"
#include "UvLoop.h"
#include <vector>
class CUvLoopMgr : public CSingleton<CUvLoopMgr>{
    SINGLE_CLASS_INITIAL(CUvLoopMgr);

public:
    ~CUvLoopMgr();

public:
    int AddUvBase(CUvBase* pUvBase);

private:
    std::vector<CUvLoop*> mvecUvLoop;
    CUvMutex mcVecUvLoopMuex;
};

#define sUvLoopMgr CUvLoopMgr::Instance()
#endif