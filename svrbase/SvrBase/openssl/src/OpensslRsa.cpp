#include "OpensslRsa.h"

COpensslRsa::COpensslRsa(){
    BZERO(mlPubKey); 
    BZERO(mlPriKey);
}

COpensslRsa::~COpensslRsa(){
    DOFREE(mlPubKey.pStr);
    DOFREE(mlPriKey.pStr);
}

void COpensslRsa::SetPubKey(const char* pKey, int iLen) {
    ASSERT_RET(pKey && iLen > 0);
    DOFREE(mlPubKey.pStr);
    mlPubKey.pStr = (char*)do_malloc(sizeof(char)*iLen);
    mlPubKey.iLen = iLen;
    memcpy(mlPubKey.pStr, pKey, iLen);
}

void COpensslRsa::SetPriKey(const char* pKey, int iLen) {
    ASSERT_RET(pKey && iLen > 0);
    DOFREE(mlPriKey.pStr);
    mlPriKey.pStr = (char*)do_malloc(sizeof(char)*iLen);
    mlPriKey.iLen = iLen;
    memcpy(mlPriKey.pStr, pKey, iLen);
}

len_str COpensslRsa::RsaEncode(const char* psrc, size_t iLen) {
    len_str lRet;
    BZERO(lRet);

    ASSERT_RET_VALUE(psrc && iLen > 0 && mlPubKey.pStr && mlPubKey.iLen > 0, lRet);
    BIO *keybio = BIO_new_mem_buf((unsigned char *)mlPubKey.pStr, (int)mlPubKey.iLen);
    ASSERT_RET_VALUE(keybio, lRet);
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    RSA* pRSAPublicKey = RSA_new();
    RSA *rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
        lRet.pStr = (char *)do_malloc((len + 1)*sizeof(char));

        // 加密函数  
        int ret = RSA_public_encrypt((int)iLen, (const unsigned char*)psrc, (unsigned char*)lRet.pStr, rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
            lRet.iLen = ret;
        } else {
            LOG_ERR("RSA_public_encrypt error");
            DOFREE(lRet.pStr);
        }
    }
    
    // 释放内存  
    BIO_free_all(keybio);
    RSA_free(rsa);

    return lRet;
}

len_str COpensslRsa::RsaDecode(const char* psrc, size_t iLen) {
    len_str lRet;
    BZERO(lRet);

    ASSERT_RET_VALUE(psrc && iLen > 0 && mlPriKey.pStr && mlPriKey.iLen > 0, lRet);
    BIO *keybio = BIO_new_mem_buf((unsigned char *)mlPriKey.pStr, (int)mlPriKey.iLen);
    ASSERT_RET_VALUE(keybio, lRet);
    RSA *rsa = RSA_new();

    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
        lRet.pStr = (char *)do_malloc((len + 1)*sizeof(char));

        // 解密函数  
        int ret = RSA_private_decrypt(iLen, (const unsigned char*)psrc, (unsigned char*)lRet.pStr, rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
            lRet.iLen = ret;
        } else {
            LOG_ERR("RSA_private_decrypt error");
            DOFREE(lRet.pStr);
        }
    }

    // 释放内存  
    BIO_free_all(keybio);
    RSA_free(rsa);
    return lRet;
}