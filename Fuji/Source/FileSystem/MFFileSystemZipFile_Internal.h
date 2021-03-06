#if !defined(_MFFILESYSTEM_ZIPFILE_INTERNAL_H)
#define _MFFILESYSTEM_ZIPFILE_INTERNAL_H

#include "FileSystem/MFFileSystemZipFile.h"

// internal functions
MFInitStatus MFFileSystemZipFile_InitModule();
void MFFileSystemZipFile_DeinitModule();

// filesystem callbacks
int MFFileSystemZipFile_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemZipFile_Dismount(MFMount *pMount);

MFFile* MFFileSystemZipFile_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileZipFile_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileZipFile_Close(MFFile* fileHandle);
int MFFileZipFile_Read(MFFile* fileHandle, void *pBuffer, int64 bytes);
int MFFileZipFile_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes);
int MFFileZipFile_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);
bool MFFileZipFile_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData);
bool MFFileZipFile_FindNext(MFFind *pFind, MFFindData *pFindData);
void MFFileZipFile_FindClose(MFFind *pFind);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileZipFile_GetSize(const char* pFilename);
bool MFFileZipFile_Exists(const char* pFilename);

// handle to the ZipFile filesystem
extern MFFileSystemHandle hZipFileSystem;

#endif
