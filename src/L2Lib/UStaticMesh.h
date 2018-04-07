#pragma once

#include "UMath.h"
#include "UPrimitive.h"
#include "UMaterial.h"
#include "UProperty.h"

struct UStaticMeshSurf
{
	int32	UnknownDW; // immer 0
	int32	OffsetSize; // ??
	uint16	VertexMax; // ??
	uint16	TrisCount, TrisMax;

	UPackage& Serialize(UPackage& Ar)
	{
		Ar >> UnknownDW;
		//CHECK( UnknownDW != 0 );
		return Ar >> OffsetSize >> VertexMax >> TrisCount >> TrisMax;
	}
	IMPLEMENT_USERIALIZER(UStaticMeshSurf);
};

struct USMeshVertex
{
	UVector	Point;
	UVector Normal; // ? sicher?

	UPackage& Serialize(UPackage& Ar)
	{
		return Ar >> Point >> Normal;
	}
	IMPLEMENT_USERIALIZER(USMeshVertex);
};

struct USMeshCoord
{
	FLOAT U;
	FLOAT V;

	UPackage& Serialize(UPackage& Ar)
	{
		return Ar >> U >> V;
	}
	IMPLEMENT_USERIALIZER(USMeshCoord);
};

struct USMeshCoords
{
	jfArray<USMeshCoord, uint32> Elements;
	uint32 UnkownID;
	uint32 UnkownDW;

	UPackage& Serialize(UPackage& Ar) { return Ar >> Elements >> UnkownID >> UnkownDW; }
	IMPLEMENT_USERIALIZER(USMeshCoords);
};

class DLLEXPORT UStaticMesh : public UPrimitive
{
public:
	jfArray<UStaticMeshSurf, uint32> Surfs;
	UBox BBox;
	jfArray<USMeshVertex, uint32> Vertices;
	jfArray<UColor, uint32> VertColors;
	jfArray<UColor, uint32> VertColors2; // ??
	jfArray<USMeshCoords, uint32> TexCoords;
	jfArray2<int16, uint32> VIndices;
	jfArray2<int16, uint32> VIndices2; // ??

	//
	int32 MaterialCount;
	jfArray<UMaterialContainer*, uint32> Materials;

	GLuint m_RenderLists;
	uint32 m_NumSurfs;
	GLuint *m_Textures;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UStaticMesh);
public:
	UStaticMesh(void);
	~UStaticMesh(void);
};

