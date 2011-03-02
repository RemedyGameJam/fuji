#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_D3D11
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_D3D11
	#define MFVertex_CreateVertexDeclaration MFVertex_CreateVertexDeclaration_D3D11
	#define MFVertex_DestroyVertexDeclaration MFVertex_DestroyVertexDeclaration_D3D11
	#define MFVertex_CreateVertexBuffer MFVertex_CreateVertexBuffer_D3D11
	#define MFVertex_DestroyVertexBuffer MFVertex_DestroyVertexBuffer_D3D11
	#define MFVertex_LockVertexBuffer MFVertex_LockVertexBuffer_D3D11
	#define MFVertex_UnlockVertexBuffer MFVertex_UnlockVertexBuffer_D3D11
	#define MFVertex_CreateIndexBuffer MFVertex_CreateIndexBuffer_D3D11
	#define MFVertex_DestroyIndexBuffer MFVertex_DestroyIndexBuffer_D3D11
	#define MFVertex_LockIndexBuffer MFVertex_LockIndexBuffer_D3D11
	#define MFVertex_UnlockIndexBuffer MFVertex_UnlockIndexBuffer_D3D11
	#define MFVertex_SetVertexDeclaration MFVertex_SetVertexDeclaration_D3D11
	#define MFVertex_SetVertexStreamSource MFVertex_SetVertexStreamSource_D3D11
	#define MFVertex_RenderVertices MFVertex_RenderVertices_D3D11
	#define MFVertex_RenderIndexedVertices MFVertex_RenderIndexedVertices_D3D11
#endif

#include "MFVector.h"
#include "MFVertex_Internal.h"

MFVertexDataFormat MFVertexD3D11_ChoooseDataType(MFVertexElementType elementType, int components)
{
	return MFVDF_Unknown;
}

void MFVertex_InitModulePlatformSpecific()
{
}

void MFVertex_DeinitModulePlatformSpecific()
{
}

MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{
	return NULL;
}

void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{
}

MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory)
{
	return NULL;
}

void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{

}

MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory)
{
	return NULL;
}

void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
}

void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
}

void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
}

void MFVertex_SetVertexDeclaration(MFVertexDeclaration *pVertexDeclaration)
{
}

void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer)
{
}

static int MFVertex_GetNumPrims(MFVertexPrimType primType, int numVertices)
{

	return 0;
}

void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices)
{
}

void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer)
{
}

#endif // MF_RENDERER