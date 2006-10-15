#include "Fuji.h"
#include "F3D.h"

enum KeyType
{
	KT_Quat = 0,
	KT_Scale,
	KT_Translation,
	KT_Matrix
};

extern F3DFile *pModel;

char gTokenBuffer[2048];

struct XMeshChunk
{
	static XMeshChunk Create(const MFMatrix &_mat, const char *_pMesh) { XMeshChunk mc; mc.mat = _mat; mc.pMesh = _pMesh; return mc; }
	MFMatrix mat;
	const char *pMesh;
};

MFArray<XMeshChunk> gMeshChunks;
MFArray<const char *> gAnimSets;

const char *GetNextToken(const char *pText, const char **ppTokenEnd = NULL, char *pBuffer = gTokenBuffer)
{
	char *pT = pBuffer;

	while(MFIsWhite(*pText) || MFIsNewline(*pText))
		++pText;

	if(!*pText)
		return NULL;

	// skip comments
	while(*pText == '/' && pText[1] == '/')
	{
		while(*pText && !MFIsNewline(*pText))
			++pText;

		while(MFIsWhite(*pText) || MFIsNewline(*pText))
			++pText;

		if(!*pText)
			return NULL;
	}

	while(*pText && !MFIsWhite(*pText) && !MFIsNewline(*pText) && *pText != ',' && *pText != ';' && *pText != '{' && *pText != '}')
	{
		*pT++ = *pText++;
	}

	if(pT == pBuffer)
		*pT++ = *pText++;

	*pT = 0;

	if(ppTokenEnd)
		*ppTokenEnd = pText;

	return pBuffer;
}

void SkipToken(const char *&pText, const char *pToken)
{
	const char *pTok = GetNextToken(pText, &pText);
	MFDebug_Assert(!MFString_Compare(pTok, pToken), MFStr("Expected '%s'\n", pToken));
}

void SkipSection(const char *&pText)
{
	while(MFString_Compare(GetNextToken(pText, &pText), "{")) { }

	int depth = 1;

	while(depth)
	{
		const char *pTok = GetNextToken(pText, &pText);

		if(!MFString_Compare(pTok, "{"))
			++depth;
		if(!MFString_Compare(pTok, "}"))
			--depth;
	}
}

const char *FindSectionInScope(const char *pText, const char *pSection)
{
	const char *pT;
	int depth = 1;

	while(depth)
	{
		const char *pTok = GetNextToken(pText, &pT);

		if(depth == 1)
			if(!MFString_Compare(pTok, pSection))
				return pText;
		pText = pT;

		if(!MFString_Compare(pTok, "{"))
			++depth;
		if(!MFString_Compare(pTok, "}"))
			--depth;
	}

	return NULL;
}

const char *GetValue(const char *pText, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	const char *pVal = GetNextToken(pText, &pText);
	const char *pSemi = GetNextToken(pText, ppTokenEnd, temp);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");

	return pVal;
}

float GetFloat(const char *pText, const char **ppTokenEnd = NULL)
{
	return (float)atof(GetValue(pText, ppTokenEnd));
}

int GetInt(const char *pText, const char **ppTokenEnd = NULL)
{
	return atoi(GetValue(pText, ppTokenEnd));
}

const char *GetString(const char *pText, const char **ppTokenEnd = NULL)
{
	char gStringBuffer[2048];

	char *pString = (char*)GetNextToken(pText, &pText, gStringBuffer);

	MFDebug_Assert(*pString == '"', "Not a string.");

	int len = MFString_Length(pString);

	if(len > 1 && pString[len - 1] == '"')
	{
		pString[len - 1] = 0;
	}
	else
	{
		while(*pText && *pText != '"')
		{
			MFDebug_Assert(!MFIsNewline(*pText), "Unterminated string found...");

			pString[len] = *pText;
			++len;
			++pText;
		}

		MFDebug_Assert(*pText, "Unterminated string found...");

		pString[len] = 0;
		++pText;
	}

	SkipToken(pText, ";");

	if(ppTokenEnd)
		*ppTokenEnd = pText;

	return &pString[1];
}

void GetIntArray(const char *pText, int *pOutput, int arrayLength, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	for(int a=0; a<arrayLength; a++)
	{
		const char *pVal = GetNextToken(pText, &pText);

		if(a < arrayLength-1)
		{
			const char *pComa = GetNextToken(pText, &pText, temp);
			MFDebug_Assert(!MFString_Compare(pComa, ","), "Array values are separated with a coma.\n");
		}

		pOutput[a] = atoi(pVal);
	}

	const char *pSemi = GetNextToken(pText, ppTokenEnd);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");
}

void GetFloatArray(const char *pText, float *pOutput, int arrayLength, const char **ppTokenEnd = NULL)
{
	char temp[1024];

	for(int a=0; a<arrayLength; a++)
	{
		const char *pVal = GetNextToken(pText, &pText);

		if(a < arrayLength-1)
		{
			const char *pComa = GetNextToken(pText, &pText, temp);
			MFDebug_Assert(!MFString_Compare(pComa, ","), "Array values are separated with a coma.\n");
		}

		pOutput[a] = (float)atof(pVal);
	}

	const char *pSemi = GetNextToken(pText, ppTokenEnd);

	MFDebug_Assert(!MFString_Compare(pSemi, ";"), "Value is not terminated with a semicolon.\n");
}

int* ParseMaterialList(const char *pText, F3DSubObject &sub, int numFaces)
{
	int *pMatFaces = NULL;

	const char *pMatList = FindSectionInScope(pText, "MeshMaterialList");

	if(pMatList)
	{
		SkipToken(pMatList, "MeshMaterialList");
		SkipToken(pMatList, "{");

		int numMats = GetInt(pMatList, &pMatList);
		MFDebug_Assert(numMats < 16, "Exceeded maximum 16 materials per subobject...");
		int numMatFaces = GetInt(pMatList, &pMatList);
		MFDebug_Assert(numMatFaces == numFaces, "Number of material faces does not match number of mesh faces");

		pMatFaces = (int*)MFHeap_Alloc(sizeof(int)*numMatFaces);
		GetIntArray(pMatList, pMatFaces, numMatFaces, &pMatList);

		// process materials...
		const char *pToken = GetNextToken(pMatList, &pMatList);

		while(MFString_Compare(pToken, "}"))
		{
			if(!MFString_Compare(pToken, "Material"))
			{
				const char *pMatName = GetNextToken(pMatList, &pToken);

				if(MFString_Compare(pMatName, "{"))
					pMatList = pToken;
				else
					pMatName = "Unknown Material";

				int matID = pModel->GetMaterialChunk()->GetMaterialIndexByName(pMatName);

				F3DMaterialSubobject &matSub = sub.matSubobjects.push();

				if(matID != -1)
				{
					matSub.materialIndex = matID;
					SkipSection(pMatList);
				}
				else
				{
					matSub.materialIndex = pModel->GetMaterialChunk()->materials.size();
					F3DMaterial &mat = pModel->GetMaterialChunk()->materials.push();
					MFString_CopyN(mat.name, pMatName, 64);

					SkipToken(pMatList, "{");

					mat.diffuse.x = GetFloat(pMatList, &pMatList);
					mat.diffuse.y = GetFloat(pMatList, &pMatList);
					mat.diffuse.z = GetFloat(pMatList, &pMatList);
					mat.diffuse.w = GetFloat(pMatList, &pMatList);
					SkipToken(pMatList, ";");

					mat.specularLevel = GetFloat(pMatList, &pMatList);

					mat.specular.x = GetFloat(pMatList, &pMatList);
					mat.specular.y = GetFloat(pMatList, &pMatList);
					mat.specular.z = GetFloat(pMatList, &pMatList);
					mat.diffuse.w = 0.0f;
					SkipToken(pMatList, ";");

					mat.emissive.x = GetFloat(pMatList, &pMatList);
					mat.emissive.y = GetFloat(pMatList, &pMatList);
					mat.emissive.z = GetFloat(pMatList, &pMatList);
					mat.emissive.w = 0.0f;
					SkipToken(pMatList, ";");

					// any textures...
					pToken = GetNextToken(pMatList, &pMatList);

					while(MFString_Compare(pToken, "}"))
					{
						if(!MFString_Compare(pToken, "TextureFilename"))
						{
							SkipToken(pMatList, "{");
							MFString_Copy(mat.maps[0], GetString(pMatList, &pMatList));
							SkipToken(pMatList, "}");
						}
						else
						{
							printf("Unknown token '%s'\n", pToken);
						}

						pToken = GetNextToken(pMatList, &pMatList);
					}
				}
			}
			else
			{
				printf("Unknown token '%s'\n", pToken);
			}

			pToken = GetNextToken(pMatList, &pMatList);
		}
	}

	return pMatFaces;
}

const char *ParseNormals(const char *pText, F3DSubObject &sub, const MFMatrix &mat, int numFaces, int *pMatFaces)
{
	SkipToken(pText, "{");

	// get num positions
	int numNormals = GetInt(pText, &pText);

	sub.normals.resize(numNormals);

	// read positions
	for(int a=0; a<numNormals; a++)
	{
		sub.normals[a].x = GetFloat(pText, &pText);
		sub.normals[a].y = GetFloat(pText, &pText);
		sub.normals[a].z = GetFloat(pText, &pText);

		sub.normals[a] = ApplyMatrix3(sub.normals[a], mat);
		sub.normals[a].Normalise3();

		if(a < numNormals-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	// get num faces
	int numNormalFaces = GetInt(pText, &pText);
	MFDebug_Assert(numNormalFaces == numFaces, "Number of normal faces does not match the number of faces in the mesh.");

	// read faces
	int face[16], numVerts[16];
	MFZeroMemory(numVerts, sizeof(numVerts));
	for(int a=0; a<numNormalFaces; a++)
	{
		int matSub = pMatFaces ? pMatFaces[a] : 0;

		int numPoints = GetInt(pText, &pText);
		GetIntArray(pText, face, numPoints, &pText);

		for(int b=0; b<numPoints; b++)
			sub.matSubobjects[matSub].vertices[numVerts[matSub]+b].normal = face[b];

		numVerts[matSub] += numPoints;

		if(a < numNormalFaces-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");
	SkipToken(pText, "}");

	return pText;
}

const char *ParseTexCoords(const char *pText, F3DSubObject &sub, int numPositions)
{
	SkipToken(pText, "{");

	// get num positions
	int numUV = GetInt(pText, &pText);
	MFDebug_Assert(numUV == numPositions, "Number of UV's does not match the number of verts in the mesh.");

	sub.uvs.resize(numUV);

	// read positions
	for(int a=0; a<numUV; a++)
	{
		sub.uvs[a].x = GetFloat(pText, &pText);
		sub.uvs[a].y = GetFloat(pText, &pText);

		if(a < numUV-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	// map to faces
	for(int m=0; m<sub.matSubobjects.size(); m++)
	{
		int totalVerts = sub.matSubobjects[m].vertices.size();
		for(int a=0; a<totalVerts; a++)
		{
			sub.matSubobjects[m].vertices[a].uv1 = sub.matSubobjects[m].vertices[a].position;
		}
	}

	SkipToken(pText, "}");

	return pText;
}

const char *ParseColours(const char *pText, F3DSubObject &sub, int numPositions)
{
	SkipToken(pText, "{");

	// get num positions
	int numColours = GetInt(pText, &pText);
	MFDebug_Assert(numColours == numPositions, "Number of colours's does not match the number of verts in the mesh.");

	sub.colours.resize(numColours);

	// read positions
	for(int a=0; a<numColours; a++)
	{
		int colVert = GetInt(pText, &pText);

		sub.colours[colVert].x = GetFloat(pText, &pText);
		sub.colours[colVert].y = GetFloat(pText, &pText);
		sub.colours[colVert].z = GetFloat(pText, &pText);
		sub.colours[colVert].w = GetFloat(pText, &pText);
		SkipToken(pText, ";");

		if(a < numColours-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	// map to faces
	for(int m=0; m<sub.matSubobjects.size(); m++)
	{
		int totalVerts = sub.matSubobjects[m].vertices.size();
		for(int a=0; a<totalVerts; a++)
		{
			sub.matSubobjects[m].vertices[a].colour = sub.matSubobjects[m].vertices[a].position;
		}
	}

	SkipToken(pText, "}");

	return pText;
}

const char *ParseSkinWeights(const char *pText, F3DSubObject &sub, int numPositions)
{
	SkipToken(pText, "{");

	const char *pName = GetString(pText, &pText);

	// get num weights
	int numWeights = GetInt(pText, &pText);
	MFDebug_Assert(numWeights == numPositions, "Number of weights's does not match the number of verts in the mesh.");

	int *pIndices = (int*)MFHeap_Alloc(sizeof(int) * numWeights);
	GetIntArray(pText, pIndices, numWeights, &pText);

	float *pWeights = (float*)MFHeap_Alloc(sizeof(float) * numWeights);
	GetFloatArray(pText, pWeights, numWeights, &pText);

	MFMatrix matrixOffset;
	GetFloatArray(pText, (float*)&matrixOffset, 16, &pText);
	SkipToken(pText, ";");

	SkipToken(pText, "}");

	// now we want to do something with all this data...
	F3DSkeletonChunk *pSC = pModel->GetSkeletonChunk();

	// check weights are sequential
	for(int a=0; a<numWeights; a++)
		MFDebug_Assert(a == pIndices[a], "Weight array is not sequential!");

	// map to faces
	for(int m=0; m<sub.matSubobjects.size(); m++)
	{
		int totalVerts = sub.matSubobjects[m].vertices.size();
		for(int a=0; a<totalVerts; a++)
		{
			F3DVertex &v = sub.matSubobjects[m].vertices[a];

			int i = v.position;
			const int numBones = sizeof(v.bone)/sizeof(v.bone[0]);

			if(pWeights[i] != 0.0f)
			{
				for(int b=0; b<numBones; b++)
				{
					if(v.bone[b] == -1)
					{
						v.weight[b] = pWeights[i];

						int bone = pSC->FindBone(pName);
						if(bone != -1)
						{
							v.bone[b] = bone;
							pSC->bones[bone].bIsSkinned = true;
							sub.matSubobjects[m].maxWeights = MFMax(sub.matSubobjects[m].maxWeights, b+1);
						}

						break;
					}
				}
			}
		}
	}

	return pText;
}

const char *ParseMesh(const char *pText, const MFMatrix &mat)
{
	F3DMeshChunk *pMesh = pModel->GetMeshChunk();
	F3DSubObject &sub = pMesh->subObjects.push();

	// read name
	GetNextToken(pText, &pText, sub.name);

	if(!MFString_Compare(sub.name, "{"))
		MFString_Copy(sub.name, "Unnamed Subobject");
	else
		SkipToken(pText, "{");

	// get num positions
	int numPositions = GetInt(pText, &pText);

	sub.positions.resize(numPositions);

	// read positions
	for(int a=0; a<numPositions; a++)
	{
		sub.positions[a].x = GetFloat(pText, &pText);
		sub.positions[a].y = GetFloat(pText, &pText);
		sub.positions[a].z = GetFloat(pText, &pText);

		sub.positions[a] = ApplyMatrixH(sub.positions[a], mat);

		if(a < numPositions-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	// get num faces
	int numFaces = GetInt(pText, &pText);

	// see if we have a material face mapping
	int *pMatFaces = ParseMaterialList(pText, sub, numFaces);

	// read faces
	int face[16], numVerts[16], numTris[16];
	MFZeroMemory(numVerts, sizeof(numVerts));
	MFZeroMemory(numTris, sizeof(numTris));
	for(int a=0; a<numFaces; a++)
	{
		int matSub = pMatFaces ? pMatFaces[a] : 0;

		int numPoints = GetInt(pText, &pText);
		MFDebug_Assert(numPoints < 16, "Exceeded maximum 16 points per face...");
		GetIntArray(pText, face, numPoints, &pText);

		for(int b=0; b<numPoints; b++)
			sub.matSubobjects[matSub].vertices[numVerts[matSub]+b].position = face[b];

		for(int b=0; b<numPoints-2; b++)
		{
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[0] = numVerts[matSub]+0;
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[1] = numVerts[matSub]+b+1;
			sub.matSubobjects[matSub].triangles[numTris[matSub]+b].v[2] = numVerts[matSub]+b+2;
		}

		numVerts[matSub] += numPoints;
		numTris[matSub] += numPoints-2;

		if(a < numFaces-1)
			SkipToken(pText, ",");
	}
	SkipToken(pText, ";");

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "MeshMaterialList"))
		{
			SkipSection(pText);
		}
		else if(!MFString_Compare(pTok, "MeshNormals"))
		{
			pText = ParseNormals(pText, sub, mat, numFaces, pMatFaces);
		}
		else if(!MFString_Compare(pTok, "MeshTextureCoords"))
		{
			pText = ParseTexCoords(pText, sub, numPositions);
		}
		else if(!MFString_Compare(pTok, "MeshVertexColors"))
		{
			pText = ParseColours(pText, sub, numPositions);
		}
		else if(!MFString_Compare(pTok, "XSkinMeshHeader"))
		{
			SkipToken(pText, "{");

			// get num positions
			int nMaxSkinWeightsPerVertex = GetInt(pText, &pText);
			int nMaxSkinWeightsPerFace = GetInt(pText, &pText);
			int nBones = GetInt(pText, &pText);

			// not yet sure how this helps...
			for(int m=0; m<sub.matSubobjects.size(); m++)
			{
				sub.matSubobjects[m].numBones = nBones;
			}

			SkipToken(pText, "}");
		}
		else if(!MFString_Compare(pTok, "SkinWeights"))
		{
			pText = ParseSkinWeights(pText, sub, numPositions);
		}
		else if(!MFString_Compare(pTok, "DeclData"))
		{
			SkipSection(pText);
		}
		else
		{
			printf("Unknown token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	// we should order the bone weights into the most to least weighting.

	return pText;
}

const char *ParseAnimation(const char *pText)
{
	SkipToken(pText, "{");

	char bone[64];
	int numFrames = 0;

	MFVector *pQuats = NULL;
	MFVector *pScale = NULL;
	MFVector *pTrans = NULL;
	MFMatrix *pMats = NULL;

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "AnimationKey"))
		{
			SkipToken(pText, "{");

			int type = GetInt(pText, &pText);
			numFrames = GetInt(pText, &pText);

			switch(type)
			{
				case KT_Quat:
					pQuats = (MFVector*)MFHeap_Alloc(sizeof(MFVector)*numFrames);
					break;
				case KT_Scale:
					pScale = (MFVector*)MFHeap_Alloc(sizeof(MFVector)*numFrames);
					break;
				case KT_Translation:
					pTrans = (MFVector*)MFHeap_Alloc(sizeof(MFVector)*numFrames);
					break;
				case KT_Matrix:
					pMats = (MFMatrix*)MFHeap_Alloc(sizeof(MFMatrix)*numFrames);
					break;
			}

			// read data
			for(int a=0; a<numFrames; a++)
			{
				int frame = GetInt(pText, &pText);
				MFDebug_Assert(frame == a, "Frame sequence is not sequential.");

				int numComponents = GetInt(pText, &pText);

				switch(type)
				{
					case KT_Quat:
						MFDebug_Assert(numComponents == 4, "Required 4 components for a quaternion.");
						GetFloatArray(pText, (float*)&pQuats[a], numComponents, &pText);
						break;
					case KT_Scale:
						MFDebug_Assert(numComponents == 3, "Required 3 components for a scale.");
						GetFloatArray(pText, (float*)&pScale[a], numComponents, &pText);
						break;
					case KT_Translation:
						MFDebug_Assert(numComponents == 3, "Required 3 components for a translation.");
						GetFloatArray(pText, (float*)&pTrans[a], numComponents, &pText);
						break;
					case KT_Matrix:
						MFDebug_Assert(numComponents == 16, "Required 16 components for a matrix.");
						GetFloatArray(pText, (float*)&pMats[a], numComponents, &pText);
						break;
				}
				SkipToken(pText, ";");

				if(a < numFrames-1)
					SkipToken(pText, ",");
			}
			SkipToken(pText, ";");

			SkipToken(pText, "}");
		}
		else if(!MFString_Compare(pTok, "{"))
		{
			GetNextToken(pText, &pText, bone);
			SkipToken(pText, "}");
		}
		else
		{
			printf("Unexpected token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	// copy data to bone
	F3DAnimation &anim = pModel->GetAnimationChunk()->anims.push();

	anim.boneID = pModel->GetSkeletonChunk()->FindBone(bone);
	if(anim.boneID == -1)
	{
		printf("Bone '%s' not found..\n", bone);
		return pText;
	}

	anim.minTime = 0.0f;
	anim.maxTime = (float)(numFrames-1);

	for(int a=0; a<numFrames; a++)
	{
		anim.keyframes[a].time = (float)a;

		if(pMats)
		{
			anim.keyframes[a].scale = MakeVector(pMats[a].GetXAxis().Magnitude3(), pMats[a].GetYAxis().Magnitude3(), pMats[a].GetZAxis().Magnitude3(), 1.0f);
			anim.keyframes[a].translation = pMats[a].GetTrans();

			// not sure how to do this yet...
			anim.keyframes[a].rotation = MFVector::identity;
		}
		else
		{
			anim.keyframes[a].rotation = pQuats ? pQuats[a] : MFVector::identity;
			anim.keyframes[a].scale = pScale ? pScale[a] : MFVector::one;
			anim.keyframes[a].translation = pTrans ? pTrans[a] : MFVector::identity;
		}
	}

	return pText;
}

const char *ParseAnimationSet(const char *pText)
{
	const char *pName = GetNextToken(pText, &pText);

	if(MFString_Compare(pName, "{"))
		SkipToken(pText, "{");

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "Animation"))
		{
			pText = ParseAnimation(pText);
		}
		else
		{
			printf("Unexpected token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	return pText;
}

const char *ParseFrame(const char *pText, const MFMatrix &mat, int parentID)
{
	int boneID = pModel->GetSkeletonChunk()->bones.size();
	F3DBone &bone = pModel->GetSkeletonChunk()->bones[boneID];

	F3DBone *pParent = parentID == -1 ? NULL : &pModel->GetSkeletonChunk()->bones[parentID];

	bone.worldMatrix = mat;

	GetNextToken(pText, &pText, bone.name);
	MFString_Copy(bone.parentName, pParent ? pParent->name : "");

	SkipToken(pText, "{");

	const char *pTok = GetNextToken(pText, &pText);

	while(MFString_Compare(pTok, "}"))
	{
		if(!MFString_Compare(pTok, "Frame"))
		{
			pText = ParseFrame(pText, bone.worldMatrix, boneID);
		}
		else if(!MFString_Compare(pTok, "FrameTransformMatrix"))
		{
			SkipToken(pText, "{");

			GetFloatArray(pText, (float*)&bone.boneMatrix, 16, &pText);

			bone.worldMatrix.Multiply(bone.boneMatrix);

			SkipToken(pText, ";");
			SkipToken(pText, "}");
		}
		else if(!MFString_Compare(pTok, "Mesh"))
		{
			gMeshChunks.push(XMeshChunk::Create(bone.worldMatrix, pText));
			SkipSection(pText);
		}
		else
		{
			printf("Unexpected token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	return pText;
}

void LoadTextXFile(const char *pText)
{
	const char *pTok = GetNextToken(pText, &pText);

	while(pTok)
	{
		if(!MFString_Compare(pTok, "Header"))
		{
			SkipToken(pText, "{");

			printf("XFile V%d.%d, 0x%X\n", GetInt(pText, &pText), GetInt(pText, &pText), GetInt(pText, &pText));

			pTok = GetNextToken(pText, &pText);
			while(MFString_Compare(pTok, "}"))
			{
				pTok = GetNextToken(pText, &pText);
			}
		}
		else if(!MFString_Compare(pTok, "Frame"))
		{
			pText = ParseFrame(pText, MFMatrix::identity, -1);
		}
		else if(!MFString_Compare(pTok, "Mesh"))
		{
			gMeshChunks.push(XMeshChunk::Create(MFMatrix::identity, pText));
			SkipSection(pText);
		}
		else if(!MFString_Compare(pTok, "AnimationSet"))
		{
			gAnimSets.push(pText);
			SkipSection(pText);
		}
		else if(!MFString_Compare(pTok, "template"))
		{
//			const char *pName = GetNextToken(pText, &pText);
			SkipSection(pText);
		}
		else
		{
			printf("Unknown token '%s'\n", pTok);
			SkipSection(pText);
		}

		pTok = GetNextToken(pText, &pText);
	}

	int a;
	for(a=0; a<gMeshChunks.size(); a++)
	{
		ParseMesh(gMeshChunks[a].pMesh, gMeshChunks[a].mat);
	}

	for(a=0; a<gAnimSets.size(); a++)
	{
		ParseAnimationSet(gAnimSets[a]);
	}

	
}

void ParseXFile(char *pFilePtr)
{
	if(MFString_CaseCmpN(pFilePtr, "xof ", 4))
	{
		printf("File is not an .x file\n");
		return;
	}

	if(!MFString_CaseCmpN(&pFilePtr[8], "txt ", 4))
	{
		pFilePtr += 16;

		LoadTextXFile(pFilePtr);
	}
	else if(!MFString_CaseCmpN(&pFilePtr[8], "bin ", 4))
	{
		printf("Binary .x files not yet supported...\n");
	}
	else
	{
		printf("Not a valid .x file...\n");
	}

	pModel->GetSkeletonChunk()->BuildHierarchy();
	pModel->GetSkeletonChunk()->FlagReferenced();
}

int F3DFile::ReadX(char *pFilename)
{
	FILE *infile;
	char *file = NULL;
	uint32 filesize = 0;

	infile = fopen(pFilename, "rb");

	if(!infile)
	{
		printf("Failed to open X file %s\n", pFilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	file = (char*)malloc(filesize+1);
	fread(file, 1, filesize, infile);
	file[filesize] = 0;

	fclose(infile);

	ParseXFile(file);

	free(file);

	return 0;
}
