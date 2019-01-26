#include "OpensslDes.h"

#define OPENSSL_DES_ALIGN_LEN   8
COpensslDes::COpensslDes(){
    BZERO(mlKey);
}

COpensslDes::~COpensslDes(){
    DOFREE(mlKey.pStr);
}

void COpensslDes::SetKey(const char* pKey, int iLen) {
    ASSERT_RET(pKey && iLen > 0);
    DOFREE(mlKey.pStr);
    mlKey.pStr = (char*)do_malloc(sizeof(char)*iLen);
    mlKey.iLen = iLen;
    memcpy(mlKey.pStr, pKey, iLen);
}

len_str COpensslDes::DesEncode(const char* psrc, size_t iLen) {
    len_str lRet;
    BZERO(lRet);

    ASSERT_RET_VALUE(psrc && iLen > 0 && mlKey.pStr && mlKey.iLen > 0, lRet);
    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, OPENSSL_DES_ALIGN_LEN);
    // 构造补齐后的密钥    
    if (mlKey.iLen <= OPENSSL_DES_ALIGN_LEN) {
        memcpy(keyEncrypt, mlKey.pStr, mlKey.iLen);
    } else {
        memcpy(keyEncrypt, mlKey.pStr, OPENSSL_DES_ALIGN_LEN);
    }
        

    // 密钥置换    
    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    // 循环加密，每OPENSSL_DES_ALIGN_LEN字节一次    
    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<unsigned char> vecCiphertext;
    unsigned char tmp[OPENSSL_DES_ALIGN_LEN];

    for (int i = 0; i < iLen / OPENSSL_DES_ALIGN_LEN; i++){
        memcpy(inputText, psrc + i * OPENSSL_DES_ALIGN_LEN, OPENSSL_DES_ALIGN_LEN);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, OPENSSL_DES_ALIGN_LEN);

        for (int j = 0; j < OPENSSL_DES_ALIGN_LEN; j++) {
            vecCiphertext.push_back(tmp[j]);
        }
    }

    if (iLen % OPENSSL_DES_ALIGN_LEN != 0){
        int tmp1 = (int)((iLen / OPENSSL_DES_ALIGN_LEN) * OPENSSL_DES_ALIGN_LEN);
        int tmp2 = (int)(iLen - tmp1);
        memset(inputText, 0, OPENSSL_DES_ALIGN_LEN);
        memcpy(inputText, psrc + tmp1, tmp2);
        // 加密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, OPENSSL_DES_ALIGN_LEN);

        for (int j = 0; j < OPENSSL_DES_ALIGN_LEN; j++) {
            vecCiphertext.push_back(tmp[j]);
        }
            
    }

    if (vecCiphertext.size() > 0) {
        lRet.iLen = vecCiphertext.size();
        lRet.pStr = (char*)do_malloc(sizeof(char) * (lRet.iLen + 1));
        lRet.pStr[lRet.iLen] = '\0';
        for (size_t i = 0; i < lRet.iLen; ++i) {
            lRet.pStr[i] = (char)vecCiphertext[i];
        }
    }

    return lRet;
}

len_str COpensslDes::DesDecode(const char* psrc, size_t iLen) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(psrc && iLen > 0 && mlKey.pStr && mlKey.iLen > 0, lRet);

    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, OPENSSL_DES_ALIGN_LEN);

    if (mlKey.iLen <= OPENSSL_DES_ALIGN_LEN) {
        memcpy(keyEncrypt, mlKey.pStr, mlKey.iLen);
    } else {
        memcpy(keyEncrypt, mlKey.pStr, OPENSSL_DES_ALIGN_LEN);
    }

    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<unsigned char> vecCleartext;
    unsigned char tmp[OPENSSL_DES_ALIGN_LEN];

    for (int i = 0; i < iLen / OPENSSL_DES_ALIGN_LEN; i++){
        memcpy(inputText, psrc + i * OPENSSL_DES_ALIGN_LEN, OPENSSL_DES_ALIGN_LEN);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, OPENSSL_DES_ALIGN_LEN);

        for (int j = 0; j < OPENSSL_DES_ALIGN_LEN; j++) {
            vecCleartext.push_back(tmp[j]);
        }
    }

    if (iLen % OPENSSL_DES_ALIGN_LEN != 0){
        int tmp1 = (int)((iLen / OPENSSL_DES_ALIGN_LEN) * OPENSSL_DES_ALIGN_LEN);
        int tmp2 = (int)(iLen - tmp1);
        memset(inputText, 0, OPENSSL_DES_ALIGN_LEN);
        memcpy(inputText, psrc + tmp1, tmp2);
        // 解密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, OPENSSL_DES_ALIGN_LEN);

        for (int j = 0; j < OPENSSL_DES_ALIGN_LEN; j++){
            vecCleartext.push_back(tmp[j]);
        }
    }

    if (vecCleartext.size() > 0) {
        lRet.iLen = vecCleartext.size();
        lRet.pStr = (char*)do_malloc(sizeof(char) * (lRet.iLen + 1));
        lRet.pStr[lRet.iLen] = '\0';
        for (size_t i = 0; i < lRet.iLen; ++i) {
            lRet.pStr[i] = (char)vecCleartext[i];
        }
    }
    return lRet;
}