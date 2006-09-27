#include "Fuji.h"
#include "MFModel_Internal.h"
#include "MFView.h"
#include "MFRenderer.h"

#include <pspgu.h>

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&pModel->worldMatrix);
	sceGuSetMatrix(GU_PROJECTION, (ScePspFMatrix4*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFMatrix::identity);
	else
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFView_GetWorldToViewMatrix());

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)&pSubobjects[a].pMeshChunks[b];

				MFMaterial_SetMaterial(pMC->pMaterial);
				MFRenderer_Begin();

				sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, pMC->numVertices, 0, pMC->pVertexData);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	if(load)
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial + base);
		pMC->pVertexData += base;
	}
	else
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial - base);
		pMC->pVertexData -= base;
	}
}