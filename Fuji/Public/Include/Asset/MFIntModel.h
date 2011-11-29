#if !defined(_MFINTMODEL_H)
#define _MFINTMODEL_H

enum MFIntModelFormat
{
	MFIMF_Unknown = -1,

	MFIMF_F3D = 0,
	MFIMF_DAE,
	MFIMF_X,
	MFIMF_ASE,
	MFIMF_OBJ,
	MFIMF_MD2,
	MFIMF_MD3,
	MFIMF_MEMD2,

	MFIMF_Max,
	MFIMF_ForceInt = 0x7FFFFFFF
};

struct MFIntModel;

MFIntModel *MFIntModel_CreateFromFile(const char *pFilename);
MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, size_t size, MFIntModelFormat format, const char *pName);

void MFIntModel_Optimise(MFIntModel *pModel);

void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, size_t *pSize, MFPlatform platform);

void MFIntModel_Destroy(MFIntModel *pModel);

#endif
