/**
 * @file MFVertex.h
 * @brief Provides low level access to vertex and index buffers.
 * @author Manu Evans
 * @defgroup MFVertex Vertex data access
 * @{
 */

#if !defined(_MFVERTEX_H)
#define _MFVERTEX_H

/**
 * @struct MFVertexDeclaration
 * Represents the layout of a vertex buffer.
 */
struct MFVertexDeclaration;

/**
 * @struct MFVertexBuffer
 * Represents a Fuji vertex buffer.
 */
struct MFVertexBuffer;

/**
 * @struct MFIndexBuffer
 * Represents a Fuji index buffer.
 */
struct MFIndexBuffer;

enum MFVertexDataFormat
{
	MFVDF_Unknown = -1,

	MFVDF_Float4 = 0,
	MFVDF_Float3,
	MFVDF_Float2,
	MFVDF_Float1,
	MFVDF_UByte4_RGBA,
	MFVDF_UByte4N_RGBA,
	MFVDF_UByte4N_BGRA,
	MFVDF_SShort4,
	MFVDF_SShort2,
	MFVDF_SShort4N,
	MFVDF_SShort2N,
	MFVDF_UShort4,
	MFVDF_UShort2,
	MFVDF_UShort4N,
	MFVDF_UShort2N,
	MFVDF_Float16_4,
	MFVDF_Float16_2,

	MFVDF_Max,
	MFVDF_ForceInt = 0x7FFFFFFF
};

enum MFVertexBufferType
{
	MFVBType_Static,
	MFVBType_Dynamic,
	MFVBType_Scratch,

	MFVBType_Max,
	MFVBType_ForceInt = 0x7FFFFFFF
};

enum MFVertexElementType
{
	MFVET_Position,
	MFVET_Normal,
	MFVET_Colour,
	MFVET_TexCoord,
	MFVET_Binormal,
	MFVET_Tangent,
	MFVET_Indices,
	MFVET_Weights,

	MFVET_Max,
	MFVET_ForceInt = 0x7FFFFFFF
};

enum MFVertexPrimType
{
	MFVPT_Points,
	MFVPT_LineList,
	MFVPT_LineStrip,
	MFVPT_TriangleList,
	MFVPT_TriangleStrip,
	MFVPT_TriangleFan,

	MFVPT_Max,
	MFVPT_ForceInt = 0x7FFFFFFF
};

struct MFVertexElement
{
	int stream;
	MFVertexElementType elementType;
	int elementIndex;
	int componentCount;
};

MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount);
MF_API void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration);

MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory = NULL);
MF_API void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer);
MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer);
MF_API void MFVertex_CopyVertexData(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const void *pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices);
MF_API void MFVertex_SetVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, const MFVector &data);
MF_API void MFVertex_ReadVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector *pData);
MF_API void MFVertex_SetVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 data);
MF_API void MFVertex_ReadVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 *pData);
MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer);

MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory = NULL);
MF_API void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer);
MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices);
MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer);

MF_API void MFVertex_SetVertexDeclaration(MFVertexDeclaration *pVertexDeclaration);
MF_API void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer);
MF_API void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices);
MF_API void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer);

#endif

/** @} */
