#ifndef __CRANDTOOL__H_
#define __CRANDTOOL__H_
#include "singleton.h"
class CRandTool : public CSingleton<CRandTool>{
    SINGLE_CLASS_INITIAL(CRandTool);
public:
    ~CRandTool();

    //设置随机种子
    void SetSeed(uint32_t dwSeedVal);
    uint32_t GetSeed() { return m_dwRandSeed; }

    uint32_t RandInt(uint32_t min, uint32_t max);

    float RandFloat();//产生[0, 1f]闭区间的随机数

private:
    void init_genrand(uint32_t s);	
    uint32_t genrand_int32(void);

private:
    static const int N = 624;
    static const int M = 397;

    uint64_t	mt[N];
    int			mti;

    uint32_t m_dwRandSeed;
};
#define sRandTool CRandTool::Instance()
#endif