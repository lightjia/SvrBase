#ifndef __CUVFILE__H_
#define __CUVFILE__H_
#include "UvBase.h"
class CUvFile : public CUvBase
{
public:
    CUvFile();
    virtual ~CUvFile();

public:
    static void FsOperCb(uv_fs_t* pFileReq);

public:
    int Open(std::string strFileName, int iFlags, int iMode);
    int Read(uv_file file, char* pData, unsigned int iDataLen, int64_t iOffset = 0);
    int Write(uv_file file, char* pData, unsigned int iDataLen, int64_t iOffset = 0);
    int Close(uv_file file);
    int Unlink(std::string strFilePath);
    int CopyFile(std::string strNewFileName, std::string strOldFileName, int iFlags);
    int Mkdir(std::string strFilePath, int iMode);
    int Mkdtemp(std::string strFilePath);
    int RmDir(std::string strFilePath);
    int ScanDir(std::string strFilePath, int iFlags);
    int Stat(std::string strFilePath);
    int Fstat(uv_file file);
    int Rename(std::string strNewFileName, std::string strOldFileName);
    int Fsync(uv_file file);
    int FdataSync(uv_file file);
    int Ftruncate(uv_file file, int64_t iOffset);
    int SendFile(uv_file out_fd, uv_file in_fd, int64_t in_offset, size_t length);
    int Access(std::string strFilePath, int iMode);
    int Chmod(std::string strFilePath, int iMode);
    int Utime(std::string strFilePath, double atime, double mtime);
    int FuTime(uv_file file, double atime, double mtime);
    int Lstat(std::string strFilePath);
    int Link(std::string strNewFileName, std::string strOldFileName);

protected:
    virtual int OnOpen(uv_fs_t* pFileReq);
    virtual int OnRead(uv_fs_t* pFileReq);
    virtual int OnWrite(uv_fs_t* pFileReq);
    virtual int OnClose(uv_fs_t* pFileReq);
    virtual int OnAccess(uv_fs_t* pFileReq);
    virtual int OnMkdir(uv_fs_t* pFileReq);
    virtual int OnCustom(uv_fs_t* pFileReq);
    virtual int OnSendFile(uv_fs_t* pFileReq);
    virtual int OnStat(uv_fs_t* pFileReq);
    virtual int OnFstat(uv_fs_t* pFileReq);
    virtual int OnTruncate(uv_fs_t* pFileReq);
    virtual int OnUtime(uv_fs_t* pFileReq);
    virtual int OnFutime(uv_fs_t* pFileReq);
    virtual int OnChmod(uv_fs_t* pFileReq);
    virtual int OnFchmod(uv_fs_t* pFileReq);
    virtual int OnFsync(uv_fs_t* pFileReq);
    virtual int OnFdatasync(uv_fs_t* pFileReq);
    virtual int OnUnlink(uv_fs_t* pFileReq);
    virtual int OnRmdir(uv_fs_t* pFileReq);
    virtual int OnMkdtemp(uv_fs_t* pFileReq);
    virtual int OnRename(uv_fs_t* pFileReq);
    virtual int OnScanDir(uv_fs_t* pFileReq);
    virtual int OnLink(uv_fs_t* pFileReq);
    virtual int OnSymLink(uv_fs_t* pFileReq);
    virtual int OnReadLink(uv_fs_t* pFileReq);
    virtual int OnChown(uv_fs_t* pFileReq);
    virtual int OnFchown(uv_fs_t* pFileReq);
    virtual int OnRealPath(uv_fs_t* pFileReq);
    virtual int OnCopyFile(uv_fs_t* pFileReq);

protected:
    uv_fs_t* mpFsReq;
};

#endif