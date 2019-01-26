#include "OpensslMd5.h"

COpensslMd5::COpensslMd5(){
}

COpensslMd5::~COpensslMd5(){
}

std::string COpensslMd5::Md5(const char* pSrc, int iSrcLen) {
    std::string strRet;
    ASSERT_RET_VALUE(pSrc && iSrcLen > 0, strRet);
    // ����md5��ϣ    
    unsigned char mdStr[MD5_DIGEST_LENGTH] = { 0 };
    MD5((const unsigned char *)pSrc, iSrcLen, mdStr);

    // ��ϣ���ַ��� תʮ�����ƴ� 32�ֽ�    
    char buf[MD5_CBLOCK] = { 0 };
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(buf + i * 2, "%02x", mdStr[i]);
    }

    strRet = std::string(buf, MD5_CBLOCK);
    return strRet;
}