#include "OpensslSha.h"

COpensslSha::COpensslSha(){
}

COpensslSha::~COpensslSha(){
}

len_str COpensslSha::Sha1(const char* psrc, int iSrcLen, char* pDst) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && pDst && iSrcLen > 0, lRet);
    lRet.pStr = (char*)do_malloc(sizeof(char)*SHA_DIGEST_LENGTH);
    lRet.iLen = SHA_DIGEST_LENGTH;
    SHA1((const unsigned char*)psrc, iSrcLen, (unsigned char*)lRet.pStr);
    return lRet;
}

len_str COpensslSha::Sha224(const char* psrc, int iSrcLen, char* pDst) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && pDst && iSrcLen > 0, lRet);
    lRet.pStr = (char*)do_malloc(sizeof(char)*SHA224_DIGEST_LENGTH);
    lRet.iLen = SHA224_DIGEST_LENGTH;
    SHA224((const unsigned char*)psrc, iSrcLen, (unsigned char*)lRet.pStr);
    return lRet;
}

len_str COpensslSha::Sha256(const char* psrc, int iSrcLen, char* pDst) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && pDst && iSrcLen > 0, lRet);
    lRet.pStr = (char*)do_malloc(sizeof(char)*SHA256_DIGEST_LENGTH);
    lRet.iLen = SHA256_DIGEST_LENGTH;
    SHA256((const unsigned char*)psrc, iSrcLen, (unsigned char*)lRet.pStr);
    return lRet;
}

len_str COpensslSha::Sha384(const char* psrc, int iSrcLen, char* pDst) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && pDst && iSrcLen > 0, lRet);
    lRet.pStr = (char*)do_malloc(sizeof(char)*SHA384_DIGEST_LENGTH);
    lRet.iLen = SHA384_DIGEST_LENGTH;
    SHA384((const unsigned char*)psrc, iSrcLen, (unsigned char*)lRet.pStr);
    return lRet;
}

len_str COpensslSha::Sha512(const char* psrc, int iSrcLen, char* pDst) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && pDst && iSrcLen > 0, lRet);
    lRet.pStr = (char*)do_malloc(sizeof(char)*SHA512_DIGEST_LENGTH);
    lRet.iLen = SHA512_DIGEST_LENGTH;
    SHA512((const unsigned char*)psrc, iSrcLen, (unsigned char*)lRet.pStr);
    return lRet;
}