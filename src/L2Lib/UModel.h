#pragma once

#include "UMath.h"
#include "UPrimitive.h"
#include "UObj.h"
#include "UProperty.h"

// vielleicht irgendwas von wegen Collision oder Octree?
struct DLLEXPORT UMUnkownStruct01
{
public:
	struct UnknownSubStruct
	{
		UVector	Point;
		UPlane	Plane;
		UVector	Vector;
	};

	jfArray2<UnknownSubStruct, uint32> SubStructs;
	int32			Int1;
	UMaterial *Material1;
	int32			Int2;
	int32			Int3;
	int32 		Int4;

	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> SubStructs;
		Ar >> Int1;
#if 1	// don't load textures/material that we don't need / not able to process - avoid
		// texture-loading overhead
		int32 MaterialIndex;
		Ar >> UIndex( MaterialIndex );
#else
		Ar >> Material1;
#endif
		Ar >> Int2 >> Int3 >> Int4;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UMUnkownStruct01 );
};

// serious guessings based on extreme hex analysis and dll disassembling & debugging
class DLLEXPORT ULightMap
{
public:

	struct UnknownSubStruct {
		UObject /*class ALight**/	*Light;
		jfArray2<uint8, uint32>	SomeBytes;
		int32				Size[2];
		int32				Unknown[3];
		int32				Size2[2]; // normalerweise (Size[0]-1,Size[1]-1)

		UPackage& Serialize( UPackage& Ar )
		{
			Ar >> *(UObject**)&Light;
			Ar >> SomeBytes;
			Ar >> Size[0] >> Size[1];
			Ar.Read( Unknown, sizeof( Unknown ) );
			Ar >> Size2[0];
			Ar >> Size2[1];
			return Ar;
		}
		IMPLEMENT_USERIALIZER( UnknownSubStruct );
	};

	int32				iLightMapTexture; // ??
	int32				iSurface;
	int32				iUnknown;
	int32				OffsetX, OffsetY;
	int32				SizeX, SizeY;
	//UMatrix			SomeMatrix;
	FLOAT			MysteriousField[9]; // [16+9] ohne Matrix
	jfArray<UnknownSubStruct, uint32> SubStructs;
	UObject /*class ULevel**/	*Level;
	DWORD			SomeIndex;

	// UObject interface - tja durch dasm hab ich auch gleich Ar.Ver() Konflikte beseitigt ;-)
	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> UIndex( iLightMapTexture );
		Ar >> UIndex( iSurface );
		Ar >> UIndex( iUnknown ); // usually 1
		Ar >> UIndex( OffsetX ) >> UIndex( OffsetY );
		Ar >> UIndex( SizeX ) >> UIndex( SizeY );
		//Ar >> SomeMatrix;
		Ar.Seek(16 * sizeof(float));
		Ar.Read( MysteriousField, sizeof( MysteriousField ) );
		if( Ar.Ver( ) >= 107 )
		{
			Ar >> SubStructs;
			if( Ar.Ver( ) < 108 )
			{
				int32 Dummy;
				Ar >> Dummy;
			}
		}
		if( Ar.Ver( ) >= 110 )
		{
			Ar >> *(UObject**)&Level;
			Ar >> SomeIndex;
		}
		return Ar;
	}
	IMPLEMENT_USERIALIZER( ULightMap );
};

class DLLEXPORT UStaticLightMapTexture
{
public:
	jfArray<uint8, uint32>	Array1;
	jfArray<uint8, uint32>	Array2;
	uint8	Format;		// 3: DXT1; 7: DXT3; 8: DXT5
	int32					SizeX;
	int32					SizeY;
	int32					Unknown; // irgend eine Maske oder Bit-Anzahl (z.B. 1023 [512+512-1])

	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> Array1 >> Array2;
		Format = 0;
		Ar >> Format >> SizeX >> SizeY >> Unknown;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UStaticLightMapTexture );
};

class DLLEXPORT ULightMapTexture
{
public:
	class ULevel*	Level;
	jfArray<int32, uint32>	SomeInts;
	int32				iSomeIndex;
	uint64			SomeQWORD;
	int32				SomeINT;
	UStaticLightMapTexture	LTexture;

	// Serializeer
	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> *(UObject**)&Level;
		Ar >> SomeInts;
		Ar >> SomeQWORD;
		Ar >> SomeINT;
		if( Ar.Ver( ) < 116 )
			return Ar;
		Ar >> LTexture;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( ULightMapTexture );
};

class DLLEXPORT UModel : public UPrimitive
{
public:
	enum {MAX_NODES  = 65536};
	enum {MAX_POINTS = 128000};

	// Arrays and subobjects.
	UPolys*					Polys;
	jfArray<UBspNode, uint32>	Nodes;
	jfArray<UVert, uint32>		Verts;
	jfArray<UVector, uint32>	Vectors;
	jfArray<UVector, uint32>	Points;
	jfArray<UBspSurf, uint32>	Surfs;
	jfArray<ULightMapIndex, uint32>	LightMap;
	jfArray2<uint8, uint32>		LightBits;
	jfArray<UBox, uint32>			Bounds;
	jfArray2<int32, uint32>		LeafHulls;
	jfArray<ULeaf, uint32>			Leaves;
	jfArray<UObject*/*AActor**/, uint32>			Lights;

	// Other variables.
	bool					RootOutside;
	bool					Linked;
	int32					MoverLink;
	int32					NumSharedSides;
	int32					NumZones;
	UZoneProperties			Zones[UBspNode::MAX_ZONES];

	// Unreal2-tech
	jfArray<UMUnkownStruct01, uint32>	UnkownData;

	// Unreal2-tech-lighting
	jfArray<ULightMap, uint32>			LightMaps;
	jfArray<ULightMapTexture, uint32>	LightMapTextures;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UModel);
public:
	UModel(void);
};

