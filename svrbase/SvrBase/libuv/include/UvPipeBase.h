#ifndef __CUVPIPEBASE__H_
#define __CUVPIPEBASE__H_
#include "UvBase.h"
#include <string>
class CUvPipeBase : public CUvBase{
public:
    CUvPipeBase();
    virtual ~CUvPipeBase();

public:
    void SetPipeParam(std::string strPipeName, int iIpc);
	const std::string& GetPipeId() { return mstrPipeName; }

protected:
    std::string mstrPipeName;
    int miIpc;//�Ƿ�����

private:
	std::string strPipeId;
};

#endif