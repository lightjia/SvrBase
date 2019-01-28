#include "OpensslBase64.h"

COpensslBase64::COpensslBase64(){
}

COpensslBase64::~COpensslBase64(){
}

len_str COpensslBase64::Base64Encode(const char * pSrc, int iSrcLen, bool bNewLine) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(pSrc && iSrcLen > 0, lRet);
    BIO *b64 = BIO_new(BIO_f_base64());
    if (b64) {
        if (!bNewLine) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }

        BIO *bmem = BIO_new(BIO_s_mem());
        if (bmem) {
            b64 = BIO_push(b64, bmem);
            BIO_write(b64, pSrc, iSrcLen);
            BIO_flush(b64);
            BUF_MEM * bptr = NULL;
            BIO_get_mem_ptr(b64, &bptr);

            lRet.iLen = bptr->length;
            lRet.pStr = (char *)do_malloc((bptr->length + 1) * sizeof(char));
            memcpy(lRet.pStr, bptr->data, bptr->length);
            lRet.pStr[bptr->length] = '\0';
        }
       
        BIO_free_all(b64);
    }
   
    return lRet;
}

len_str COpensslBase64::Base64Decode(const char * pSrc, int iSrcLen, bool bNewLine) {
    len_str lRet;
    BZERO(lRet);
    ASSERT_RET_VALUE(pSrc && iSrcLen > 0, lRet);
    lRet.iLen = iSrcLen;
    lRet.pStr = (char *)do_malloc((iSrcLen + 1) * sizeof(char));
    BIO *b64 = BIO_new(BIO_f_base64());
    if (b64) {
        if (!bNewLine) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }

        BIO * bmem = BIO_new_mem_buf(pSrc, iSrcLen);
        if (bmem) {
            bmem = BIO_push(b64, bmem);
            BIO_read(bmem, lRet.pStr, iSrcLen);
        }

        BIO_free_all(b64);
    }
    
    return lRet;
}