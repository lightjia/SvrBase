#include "OpensslBase64.h"
#include "MemMgr.h"

COpensslBase64::COpensslBase64(){
}

COpensslBase64::~COpensslBase64(){
}

CMemBuffer* COpensslBase64::Base64Encode(const char * pSrc, int iSrcLen, bool bNewLine) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(pSrc && iSrcLen > 0, pRet);
	pRet = new CMemBuffer();
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

			pRet->Append(bptr->data, bptr->length);
			pRet->AppendNul();
        }
       
        BIO_free_all(b64);
    }
   
    return pRet;
}

CMemBuffer* COpensslBase64::Base64Decode(const char * pSrc, int iSrcLen, bool bNewLine) {
	CMemBuffer* pRet = NULL;
    ASSERT_RET_VALUE(pSrc && iSrcLen > 0, pRet);
	pRet->SetBuffLen((iSrcLen + 1) * sizeof(char));
	pRet->AllocBuffer(pRet->GetBuffLen());
    BIO *b64 = BIO_new(BIO_f_base64());
    if (b64) {
        if (!bNewLine) {
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        }

        BIO * bmem = BIO_new_mem_buf(pSrc, iSrcLen);
        if (bmem) {
            bmem = BIO_push(b64, bmem);
            BIO_read(bmem, pRet->GetBuffer(), iSrcLen);
        }

        BIO_free_all(b64);
    }
    
    return pRet;
}