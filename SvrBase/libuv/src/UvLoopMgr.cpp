#include "UvLoopMgr.h"

CUvLoopMgr::CUvLoopMgr(){
}

CUvLoopMgr::~CUvLoopMgr(){
}

int CUvLoopMgr::AddUvBase(CUvBase* pUvBase) {
    ASSERT_RET_VALUE(nullptr != pUvBase, 1);
    bool bFlag = false;
    mcVecUvLoopMuex.Lock();
    for (std::vector<CUvLoop*>::iterator iter = mvecUvLoop.begin(); iter != mvecUvLoop.end(); ++iter) {
        CUvLoop* pUvLoop = *iter;
        if (nullptr != pUvLoop && pUvLoop->PushUvBase(pUvBase) == 0) {
            bFlag = true;
            break;
        }
    }
    mcVecUvLoopMuex.UnLock();

    if (!bFlag) {
        CUvLoop* pUvLoop = new CUvLoop();
        if (nullptr != pUvLoop) {
            if (0 == pUvLoop->Init()) {
                pUvLoop->PushUvBase(pUvBase);

                mcVecUvLoopMuex.Lock();
                mvecUvLoop.push_back(pUvLoop);
                mcVecUvLoopMuex.UnLock();
            }else {
                DODELETE(pUvLoop);
                LOG_ERR("CUvLoop Init Error");
                return 1;
            }
        }else {
            LOG_ERR("New UvLoop Error");
            return 1;
        }
    }

    return 0;
}