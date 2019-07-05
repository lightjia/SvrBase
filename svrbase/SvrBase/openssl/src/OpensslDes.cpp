#include "OpensslDes.h"

#define OPENSSL_DES_ALIGN_LEN   8
COpensslDes::COpensslDes(){
	miAlign = OPENSSL_DES_ALIGN_LEN;
}

COpensslDes::~COpensslDes(){
}

void  COpensslDes::SetAligin(int iAlign) {
	if (iAlign > 0 && iAlign <= OPENSSL_DES_ALIGN_LEN) {
		miAlign = iAlign;
	}
}

void COpensslDes::SetKey(const char* pKey, unsigned int iLen) {
    ASSERT_RET(pKey && iLen > 0);
	mcKey.Append(pKey, iLen);
}

CMemBuffer* COpensslDes::DesEncode(const char* psrc, size_t iLen) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(psrc && iLen > 0 && mcKey.GetBuffer() && mcKey.GetBuffLen() > 0, pRet);
    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, miAlign);
    // 构造补齐后的密钥    
    if (mcKey.GetBuffLen() <= miAlign) {
        memcpy(keyEncrypt, mcKey.GetBuffer(), mcKey.GetBuffLen());
    } else {
        memcpy(keyEncrypt, mcKey.GetBuffer(), miAlign);
    }
        

    // 密钥置换    
    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    // 循环加密，每miAlign字节一次    
    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<unsigned char> vecCiphertext;
    unsigned char tmp[OPENSSL_DES_ALIGN_LEN];

    for (int i = 0; i < iLen / miAlign; i++){
        memcpy(inputText, psrc + i * miAlign, miAlign);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, miAlign);

        for (int j = 0; j < miAlign; j++) {
            vecCiphertext.push_back(tmp[j]);
        }
    }

    if (iLen % miAlign != 0){
        int tmp1 = (int)((iLen / miAlign) * miAlign);
        int tmp2 = (int)(iLen - tmp1);
        memset(inputText, 0, miAlign);
        memcpy(inputText, psrc + tmp1, tmp2);
        // 加密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, miAlign);

        for (int j = 0; j < miAlign; j++) {
            vecCiphertext.push_back(tmp[j]);
        }
            
    }

    if (vecCiphertext.size() > 0) {
		pRet = new CMemBuffer();
		pRet->SetBuffLen(vecCiphertext.size());
		pRet->AllocBuffer(pRet->GetBuffLen());
		char* pDst = (char*)pRet->GetBuffer();
        for (size_t i = 0; i < pRet->GetBuffLen(); ++i) {
			pDst[i] = (char)vecCiphertext[i];
        }
    }

    return pRet;
}

CMemBuffer* COpensslDes::DesDecode(const char* psrc, size_t iLen) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(psrc && iLen > 0 && mcKey.GetBuffer() && mcKey.GetBuffLen() > 0, pRet);

    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, miAlign);
	if (mcKey.GetBuffLen() <= miAlign) {
		memcpy(keyEncrypt, mcKey.GetBuffer(), mcKey.GetBuffLen());
	} else {
		memcpy(keyEncrypt, mcKey.GetBuffer(), miAlign);
	}

    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<unsigned char> vecCleartext;
    unsigned char tmp[OPENSSL_DES_ALIGN_LEN];

    for (int i = 0; i < iLen / miAlign; i++){
        memcpy(inputText, psrc + i * miAlign, miAlign);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, miAlign);

        for (int j = 0; j < miAlign; j++) {
            vecCleartext.push_back(tmp[j]);
        }
    }

    if (iLen % miAlign != 0){
        int tmp1 = (int)((iLen / miAlign) * miAlign);
        int tmp2 = (int)(iLen - tmp1);
        memset(inputText, 0, miAlign);
        memcpy(inputText, psrc + tmp1, tmp2);
        // 解密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, miAlign);

        for (int j = 0; j < miAlign; j++){
            vecCleartext.push_back(tmp[j]);
        }
    }

    if (vecCleartext.size() > 0) {
		pRet = new CMemBuffer();
		pRet->SetBuffLen(vecCleartext.size());
		pRet->AllocBuffer(pRet->GetBuffLen());
		char* pDst = (char*)pRet->GetBuffer();
        for (size_t i = 0; i < pRet->GetBuffLen(); ++i) {
			pDst[i] = (char)vecCleartext[i];
        }
    }

    return pRet;
}