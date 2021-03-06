#include "Fuji.h"

#if MF_FILESYSTEM == MF_DRIVER_GC

#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

MFInitStatus MFFileSystemNative_InitModulePlatformSpecific()
{
	return MFAIC_Succeeded;
}

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

int MFFileSystemNative_GetNumEntries(const char *pFindPattern, bool recursive, bool flatten, int *pStringLengths)
{
	return 0;
}

MFTOCEntry* MFFileSystemNative_BuildToc(const char *pFindPattern, MFTOCEntry *pToc, MFTOCEntry *pParent, char* &pStringCache, bool recursive, bool flatten)
{
	return NULL;
}

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFCALLSTACK;

	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataNative), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");
	MFMountDataNative *pMountNative = (MFMountDataNative*)pMountData;

	return 0;
}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	return 0;
}

int MFFileNative_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	return 0;
}

int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	return 0;
}

int MFFileNative_Tell(MFFile* fileHandle)
{
	MFCALLSTACK;

	return 0;
}

MFFileState MFFileNative_Query(MFFile* fileHandle)
{
	MFCALLSTACK;

	return MFFS_Unavailable;
}

int MFFileNative_GetSize(MFFile* fileHandle)
{
	MFCALLSTACK;

	return 0;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	return 0;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	return false;
}

#endif
