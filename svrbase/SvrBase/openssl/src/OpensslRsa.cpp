#include "OpensslRsa.h"

COpensslRsa::COpensslRsa(){
}

COpensslRsa::~COpensslRsa(){
}

void COpensslRsa::SetPubKey(const char* pKey, unsigned int iLen) {
    ASSERT_RET(pKey && iLen > 0);
	mcPubKey.Append(pKey, iLen);
}

void COpensslRsa::SetPriKey(const char* pKey, unsigned int iLen) {
    ASSERT_RET(pKey && iLen > 0);
	mcPriKey.Append(pKey, iLen);
}

CMemBuffer* COpensslRsa::RsaEncode(const char* psrc, size_t iLen) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(psrc && iLen > 0 && mcPubKey.GetBuffer() && mcPubKey.GetBuffLen() > 0, pRet);
    BIO *keybio = BIO_new_mem_buf((unsigned char *)mcPubKey.GetBuffer(), (int)mcPubKey.GetBuffLen());
    ASSERT_RET_VALUE(keybio, pRet);
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    RSA* pRSAPublicKey = RSA_new();
    RSA *rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
		pRet = new CMemBuffer();
		pRet->AllocBuffer(len);
		pRet->SetBuffLen(len);
        // 加密函数  
        int ret = RSA_public_encrypt((int)iLen, (const unsigned char*)psrc, (unsigned char*)pRet->GetBuffer(), rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
			pRet->SetBuffLen(ret);
        } else {
            LOG_ERR("RSA_public_encrypt error");
			DODELETE(pRet);
        }
    }
    
    // 释放内存  
    BIO_free_all(keybio);
    RSA_free(rsa);

    return pRet;
}

CMemBuffer* COpensslRsa::RsaDecode(const char* psrc, size_t iLen) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(psrc && iLen > 0 && mcPriKey.GetBuffer() && mcPriKey.GetBuffLen() > 0, pRet);
    BIO *keybio = BIO_new_mem_buf((unsigned char *)mcPriKey.GetBuffer(), (int)mcPriKey.GetBuffLen());
    ASSERT_RET_VALUE(keybio, pRet);
    RSA *rsa = RSA_new();

    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
		pRet = new CMemBuffer();
		pRet->SetBuffLen(len);
		pRet->AllocBuffer(iLen);
        // 解密函数  
        int ret = RSA_private_decrypt((int)iLen, (const unsigned char*)psrc, (unsigned char*)pRet->GetBuffer(), rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
			pRet->SetBuffLen(ret);
        } else {
            LOG_ERR("RSA_private_decrypt error");
			DODELETE(pRet);
        }
    }

    // 释放内存  
    BIO_free_all(keybio);
    RSA_free(rsa);
    return pRet;
}