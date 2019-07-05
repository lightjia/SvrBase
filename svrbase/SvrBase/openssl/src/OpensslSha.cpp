#include "OpensslSha.h"

COpensslSha::COpensslSha(){
}

COpensslSha::~COpensslSha(){
}

std::string COpensslSha::Sha1(const unsigned char* psrc, int iSrcLen) {
    ASSERT_RET_VALUE(psrc && iSrcLen > 0, "");
	char szDst[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)psrc, iSrcLen, (unsigned char*)szDst);
    return std::string(szDst, SHA_DIGEST_LENGTH);
}

std::string COpensslSha::Sha224(const unsigned char* psrc, int iSrcLen) {
    ASSERT_RET_VALUE(psrc && iSrcLen > 0, "");
	char szDst[SHA224_DIGEST_LENGTH];
    SHA224((const unsigned char*)psrc, iSrcLen, (unsigned char*)szDst);
    return std::string(szDst, SHA224_DIGEST_LENGTH);
}

std::string COpensslSha::Sha256(const unsigned char* psrc, int iSrcLen) {
    ASSERT_RET_VALUE(psrc && iSrcLen > 0, "");
	char szDst[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)psrc, iSrcLen, (unsigned char*)szDst);
    return std::string(szDst, SHA256_DIGEST_LENGTH);
}

std::string COpensslSha::Sha384(const unsigned char* psrc, int iSrcLen) {
    ASSERT_RET_VALUE(psrc && iSrcLen > 0, "");
	char szDst[SHA384_DIGEST_LENGTH];
    SHA384((const unsigned char*)psrc, iSrcLen, (unsigned char*)szDst);
    return std::string(szDst, SHA384_DIGEST_LENGTH);
}

std::string COpensslSha::Sha512(const unsigned char* psrc, int iSrcLen) {
    ASSERT_RET_VALUE(psrc && iSrcLen > 0, "");
	char szDst[SHA512_DIGEST_LENGTH];
    SHA512((const unsigned char*)psrc, iSrcLen, (unsigned char*)szDst);
    return std::string(szDst, SHA512_DIGEST_LENGTH);
}