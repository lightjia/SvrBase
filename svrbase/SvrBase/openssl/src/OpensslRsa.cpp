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
    // �˴������ַ���  
    // 1, ��ȡ�ڴ������ɵ���Կ�ԣ��ٴ��ڴ�����rsa  
    // 2, ��ȡ���������ɵ���Կ���ı��ļ����ڴ��ڴ�����rsa  
    // 3��ֱ�ӴӶ�ȡ�ļ�ָ������rsa  
    RSA* pRSAPublicKey = RSA_new();
    RSA *rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
		pRet = new CMemBuffer();
		pRet->AllocBuffer(len);
		pRet->SetBuffLen(len);
        // ���ܺ���  
        int ret = RSA_public_encrypt((int)iLen, (const unsigned char*)psrc, (unsigned char*)pRet->GetBuffer(), rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
			pRet->SetBuffLen(ret);
        } else {
            LOG_ERR("RSA_public_encrypt error");
			DODELETE(pRet);
        }
    }
    
    // �ͷ��ڴ�  
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

    // �˴������ַ���  
    // 1, ��ȡ�ڴ������ɵ���Կ�ԣ��ٴ��ڴ�����rsa  
    // 2, ��ȡ���������ɵ���Կ���ı��ļ����ڴ��ڴ�����rsa  
    // 3��ֱ�ӴӶ�ȡ�ļ�ָ������rsa  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    int len = RSA_size(rsa);
    if (len > 0) {
		pRet = new CMemBuffer();
		pRet->SetBuffLen(len);
		pRet->AllocBuffer(iLen);
        // ���ܺ���  
        int ret = RSA_private_decrypt((int)iLen, (const unsigned char*)psrc, (unsigned char*)pRet->GetBuffer(), rsa, RSA_PKCS1_PADDING);
        if (ret > 0) {
			pRet->SetBuffLen(ret);
        } else {
            LOG_ERR("RSA_private_decrypt error");
			DODELETE(pRet);
        }
    }

    // �ͷ��ڴ�  
    BIO_free_all(keybio);
    RSA_free(rsa);
    return pRet;
}