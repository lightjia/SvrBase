#include "UvFile.h"

CUvFile::CUvFile(){
    mpFsReq = NULL;
}

CUvFile::~CUvFile(){
    uv_fs_req_cleanup(mpFsReq);
}

int CUvFile::OnOpen(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnOpen");
    return 0;
}

int CUvFile::OnRead(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnRead");
    return 0;
}

int CUvFile::OnWrite(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnWrite");
    return 0;
}

int CUvFile::OnClose(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnClose");
    return 0;
}

int CUvFile::OnAccess(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnAccess");
    return 0;
}

int CUvFile::OnMkdir(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnMkdir");
    return 0;
}

int CUvFile::OnCustom(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnCustom");
    return 0;
}

int CUvFile::OnSendFile(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnSendFile");
    return 0;
}

int CUvFile::OnStat(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnStat");
    return 0;
}

int CUvFile::OnFstat(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFstat");
    return 0;
}

int CUvFile::OnTruncate(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnTruncate");
    return 0;
}

int CUvFile::OnUtime(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnUtime");
    return 0;
}

int CUvFile::OnFutime(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFutime");
    return 0;
}

int CUvFile::OnChmod(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnChmod");
    return 0;
}

int CUvFile::OnFchmod(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFchmod");
    return 0;
}

int CUvFile::OnFsync(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFsync");
    return 0;
}

int CUvFile::OnFdatasync(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFdatasync");
    return 0;
}

int CUvFile::OnUnlink(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnUnlink");
    return 0;
}

int CUvFile::OnRmdir(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnRmdir");
    return 0;
}

int CUvFile::OnMkdtemp(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnMkdtemp");
    return 0;
}

int CUvFile::OnRename(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnRename");
    return 0;
}

int CUvFile::OnScanDir(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnScanDir");
    //TODO uv_fs_scandir_next
    return 0;
}

int CUvFile::OnLink(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnLink");
    return 0;
}

int CUvFile::OnSymLink(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnSymLink");
    return 0;
}

int CUvFile::OnReadLink(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnReadLink");
    return 0;
}

int CUvFile::OnChown(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnChown");
    return 0;
}

int CUvFile::OnFchown(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnFchown");
    return 0;
}

int CUvFile::OnRealPath(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnRealPath");
    return 0;
}

int CUvFile::OnCopyFile(uv_fs_t* pFileReq) {
    LOG_INFO("Enter CUvFile::OnCopyFile");
    return 0;
}

void CUvFile::FsOperCb(uv_fs_t* pFileReq) {
    CUvFile* pUvFile = (CUvFile*)uv_handle_get_data((uv_handle_t*)pFileReq);
    if (NULL != pUvFile) {
        switch (uv_fs_get_type(pFileReq)){
        case UV_FS_CUSTOM:
            pUvFile->OnCustom(pFileReq);
            break;
        case UV_FS_OPEN:
            pUvFile->OnOpen(pFileReq);
            break;
        case UV_FS_CLOSE:
            pUvFile->OnClose(pFileReq);
            break;
        case UV_FS_READ:
            pUvFile->OnRead(pFileReq);
            break;
        case UV_FS_WRITE:
            pUvFile->OnWrite(pFileReq);
            break;
        case UV_FS_SENDFILE:
            pUvFile->OnSendFile(pFileReq);
            break;
        case UV_FS_STAT:
            pUvFile->OnStat(pFileReq);
            break;
        case UV_FS_FSTAT:
            pUvFile->OnFstat(pFileReq);
            break;
        case UV_FS_FTRUNCATE:
            pUvFile->OnTruncate(pFileReq);
            break;
        case UV_FS_UTIME:
            pUvFile->OnUtime(pFileReq);
            break;
        case UV_FS_FUTIME:
            pUvFile->OnFutime(pFileReq);
            break;
        case UV_FS_ACCESS:
            pUvFile->OnAccess(pFileReq);
            break;
        case UV_FS_CHMOD:
            pUvFile->OnChmod(pFileReq);
            break;
        case UV_FS_FCHMOD:
            pUvFile->OnFchmod(pFileReq);
            break;
        case UV_FS_FSYNC:
            pUvFile->OnFsync(pFileReq);
            break;
        case UV_FS_FDATASYNC:
            pUvFile->OnFdatasync(pFileReq);
            break;
        case UV_FS_UNLINK:
            pUvFile->OnUnlink(pFileReq);
            break;
        case UV_FS_RMDIR:
            pUvFile->OnRmdir(pFileReq);
            break;
        case UV_FS_MKDIR:
            pUvFile->OnMkdir(pFileReq);
            break;
        case UV_FS_MKDTEMP:
            pUvFile->OnMkdtemp(pFileReq);
            break;
        case UV_FS_RENAME:
            pUvFile->OnRename(pFileReq);
            break;
        case UV_FS_SCANDIR:
            pUvFile->OnScanDir(pFileReq);
            break;
        case UV_FS_LINK:
            pUvFile->OnLink(pFileReq);
            break;
        case UV_FS_SYMLINK:
            pUvFile->OnSymLink(pFileReq);
            break;
        case UV_FS_READLINK:
            pUvFile->OnReadLink(pFileReq);
            break;
        case UV_FS_CHOWN:
            pUvFile->OnChown(pFileReq);
            break;
        case UV_FS_FCHOWN:
            pUvFile->OnFchown(pFileReq);
            break;
        case UV_FS_REALPATH:
            pUvFile->OnRealPath(pFileReq);
            break;
        case UV_FS_COPYFILE:
            pUvFile->OnCopyFile(pFileReq);
            break;
        default:
            break;
        }
    }
}

int CUvFile::Open(std::string strFileName, int iFlags, int iMode) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_open(mpUvLoop, mpFsReq, strFileName.c_str(), iFlags, iMode, CUvFile::FsOperCb);
}

int CUvFile::Read(uv_file file, char* pData, unsigned int iDataLen, int64_t iOffset) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq && pData != NULL && iDataLen > 0, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    uv_buf_t stBuf = uv_buf_init(pData, iDataLen);

    return uv_fs_read(mpUvLoop, mpFsReq, file, &stBuf, 1, iOffset, CUvFile::FsOperCb);
}

int CUvFile::Write(uv_file file, char* pData, unsigned int iDataLen, int64_t iOffset) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq && pData != NULL && iDataLen > 0, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    uv_buf_t stBuf = uv_buf_init(pData, iDataLen);

    return uv_fs_write(mpUvLoop, mpFsReq, file, &stBuf, 1, iOffset, CUvFile::FsOperCb);
}

int CUvFile::Close(uv_file file) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_close(mpUvLoop, mpFsReq, file, CUvFile::FsOperCb);
}

int CUvFile::Unlink(std::string strFilePath) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_unlink(mpUvLoop, mpFsReq, strFilePath.c_str(), CUvFile::FsOperCb);
}

int CUvFile::CopyFile(std::string strNewFileName, std::string strOldFileName, int iFlags) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_copyfile(mpUvLoop, mpFsReq, strOldFileName.c_str(), strNewFileName.c_str(), iFlags, CUvFile::FsOperCb);
}

int CUvFile::Mkdir(std::string strFilePath, int iMode) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_mkdir(mpUvLoop, mpFsReq, strFilePath.c_str(), iMode, CUvFile::FsOperCb);
}

int CUvFile::Mkdtemp(std::string strFilePath) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_mkdtemp(mpUvLoop, mpFsReq, strFilePath.c_str(), CUvFile::FsOperCb);
}

int CUvFile::RmDir(std::string strFilePath) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_rmdir(mpUvLoop, mpFsReq, strFilePath.c_str(), CUvFile::FsOperCb);
}

int CUvFile::ScanDir(std::string strFilePath, int iFlags) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_scandir(mpUvLoop, mpFsReq, strFilePath.c_str(), iFlags, CUvFile::FsOperCb);
}

int CUvFile::Stat(std::string strFilePath) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_stat(mpUvLoop, mpFsReq, strFilePath.c_str(), CUvFile::FsOperCb);
}

int CUvFile::Fstat(uv_file file) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_fstat(mpUvLoop, mpFsReq, file, CUvFile::FsOperCb);
}

int CUvFile::Rename(std::string strNewFileName, std::string strOldFileName) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_rename(mpUvLoop, mpFsReq, strOldFileName.c_str(), strNewFileName.c_str(), CUvFile::FsOperCb);
}

int CUvFile::Fsync(uv_file file) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_fsync(mpUvLoop, mpFsReq, file, CUvFile::FsOperCb);
}

int CUvFile::FdataSync(uv_file file) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_fdatasync(mpUvLoop, mpFsReq, file, CUvFile::FsOperCb);
}

int CUvFile::Ftruncate(uv_file file, int64_t iOffset) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_ftruncate(mpUvLoop, mpFsReq, file, iOffset, CUvFile::FsOperCb);
}

int CUvFile::SendFile(uv_file out_fd, uv_file in_fd, int64_t in_offset, size_t length) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_sendfile(mpUvLoop, mpFsReq, out_fd, in_fd, in_offset, length, CUvFile::FsOperCb);
}

int CUvFile::Access(std::string strFilePath, int iMode) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_access(mpUvLoop, mpFsReq, strFilePath.c_str(), iMode, CUvFile::FsOperCb);
}

int CUvFile::Chmod(std::string strFilePath, int iMode) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_chmod(mpUvLoop, mpFsReq, strFilePath.c_str(), iMode, CUvFile::FsOperCb);
}

int CUvFile::Utime(std::string strFilePath, double atime, double mtime) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_utime(mpUvLoop, mpFsReq, strFilePath.c_str(), atime, mtime, CUvFile::FsOperCb);
}

int CUvFile::FuTime(uv_file file, double atime, double mtime) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_futime(mpUvLoop, mpFsReq, file, atime, mtime, CUvFile::FsOperCb);
}

int CUvFile::Lstat(std::string strFilePath) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_lstat(mpUvLoop, mpFsReq, strFilePath.c_str(), CUvFile::FsOperCb);
}

int CUvFile::Link(std::string strNewFileName, std::string strOldFileName) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpFsReq, 1);
    uv_fs_req_cleanup(mpFsReq);
    uv_handle_set_data((uv_handle_t*)mpFsReq, (void*)this);
    return uv_fs_link(mpUvLoop, mpFsReq, strOldFileName.c_str(), strNewFileName.c_str(), CUvFile::FsOperCb);
}