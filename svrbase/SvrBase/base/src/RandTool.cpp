#include "RandTool.h"

#define	MATRIX_A	(0x9908b0dfUL)	
#define	UPPER_MASK	(0x80000000UL)	
#define	LOWER_MASK	(0x7fffffffUL)

#define XRAND_MAX (0xffffffff)

CRandTool::CRandTool(){
    mti = N + 1;
    m_dwRandSeed = 5489U;
}

CRandTool::~CRandTool(){
}

//设置随机种子
void CRandTool::SetSeed(uint32_t dwSeedVal){
    init_genrand(dwSeedVal);
    m_dwRandSeed = dwSeedVal;
}

uint32_t CRandTool::RandInt(uint32_t min, uint32_t max){
    if (min > max){
        return RandInt(max, min);
    }

    int rank = max - min + 1;
    uint32_t dwRandVal = genrand_int32();

    uint32_t offset = (uint32_t)((uint64_t)dwRandVal*(uint64_t)rank / ((uint64_t)(XRAND_MAX)+1));
    return min + offset;
}

float CRandTool::RandFloat(){
    uint32_t dwRandVal = genrand_int32();
    return (float)(dwRandVal*(1.0 / 4294967295.0f));
}

void CRandTool::init_genrand(uint32_t s){
    mt[0] = s & 0xffffffffUL;

    for (int i = 1; i < N; i++){
        mt[i] = (1812433253UL * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
        mt[i] &= 0xffffffffUL;
    }
}

uint32_t CRandTool::genrand_int32(){
    unsigned long y;
    static unsigned long mag01[2] = { 0x0UL, MATRIX_A };

    if (mti >= N){
        int kk;

        if (mti == N + 1)	
            init_genrand(5489UL);	

        for (kk = 0; kk < N - M; kk++){
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        for (; kk < N - 1; kk++){
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = (unsigned long)mt[(mti++) % N];

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}