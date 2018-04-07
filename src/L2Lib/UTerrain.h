#pragma once

#define MAP_SIZE_X (128 * 256)
#define MAP_SIZE_Z (128 * 256)

#include "UMath.h"
#include "AActor.h"
#include "UPrimitive.h"
#include "UTexture.h"
#include "UProperty.h"

struct URange
{
	float Min;
	float Max;
};
struct URangeVector
{
	URange X;
	URange Y;
	URange Z;
};

class UTerrainPrimitive : public UPrimitive
{
public:
	// ???
};


struct _NormalPair
{
	UVector Normal1;
	UVector Normal2;
};

enum ESortOrder
{
	SORT_NoSort,
	SORT_BackToFront,
	SORT_FrontToBack
};

class UTerrainLayer : public UObject
{
public:
	UMaterial*	Texture;
	UTexture*	AlphaMap;
	FLOAT		UScale;
	FLOAT		VScale;
	FLOAT		UPan;
	FLOAT		VPan;
	ETexMapAxis TextureMapAxis;
	FLOAT		TextureRotation;
	URotator	LayerRotation;
	UMatrix		TerrainMatrix;
	FLOAT		KFriction;
	FLOAT		KRestitution;
	UTexture*	LayerWeightMap;
	UVector		Scale;
public:
	bool SetVariable(const class UProperty& Prop);

	// serializer for Unreal2 (110) format
	/*UPackage& Serialize( UPackage& Ar )
	{
		//CHECK( Ar.Ver( ) > 117 );
		Ar >> *(UObject**)&Texture >> *(UObject**)&AlphaMap;
		Ar >> UScale >> VScale >> UPan >> VPan;
		(unsigned&)TextureMapAxis = 0;
		Ar >> (BYTE&)TextureMapAxis;
		Ar >> TextureRotation >> LayerRotation >> TerrainMatrix;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UTerrainLayer );*/
public:
	UTerrainLayer();
};

struct _DecorationLayer
{
	int32				ShowOnTerrain;
	UTexture*		ScaleMap;
	UTexture*		DensityMap;
	UTexture*		ColorMap;
	UStaticMesh*	StaticMesh;
	URangeVector	ScaleMultiplier;
	URange			FadeoutRadius;
	URange			DensityMultiplier;
	int32				MaxPerQuad;
	int32				Seed;
	int32				AlignToTerrain;
	ESortOrder		DrawOrder;
	int32				ShowOnInvisibleTerrain;
	int32				LitDirectional;
	int32				DisregardTerrainLighting;
	int32				RandomYaw;
};

struct _DecoInfo
{
	UVector		Location;
	URotator	Rotation;
	UVector		Scale;
	UVector		TempScale;
	UColor		Color;
	int32			Distance;
}; 

struct _DecoSectorInfo
{
	jfArray<_DecoInfo, uint32>	DecoInfo;
	UVector				Location;
	float				Radius;
};

struct _DecorationLayerData
{
	jfArray<_DecoSectorInfo, uint32> Sectors;
};

class UTerrainSector : public AActor
{
public:
	class UTerrainInfo *m_TerrainInfo;

	int32 SectorX, SectorZ;
	int32 OffsetX;
	int32 OffsetZ;
	UBox BoundingBox;
	uint32 CellNum;
	uint32 SectorWidth;

	// looks like lightmaps for different times of day
	jfArray2<uint8, uint32> UnkArr0;
	jfArray2<uint8, uint32> UnkArr1;
	jfArray2<uint8, uint32> UnkArr2;
	jfArray2<uint8, uint32> UnkArr3;
	jfArray2<uint8, uint32> UnkArr4;
	jfArray2<uint8, uint32> UnkArr5;
	jfArray2<uint8, uint32> UnkArr6;
	jfArray2<uint8, uint32> UnkArr7;

	jfArray2<uint16, uint32> TexInfo;

	//GLuint m_RenderList;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	GLuint *m_TexCoordBuffers;
	GLuint m_AlphaMapCoordBuffer;
	GLuint *m_TexInfoBuffers;
	GLuint m_LightValBuffer;

	float *m_VertexArray;
	GLuint *m_IndexArray;
	float **m_TexCoordArrays;
	float *m_AlphaMapCoordArray;
	float *m_TexInfo1Array;
	float *m_TexInfo2Array;
	float *m_TexInfo3Array;
	float *m_LightValArray;
public:
	bool SetVariable(const class UProperty& Prop);
	void OnLoaded() {};

	void Init(); // int32 InSectorX, int32 InSectorZ

	virtual void Tick();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UTerrainSector);
public:
	UTerrainSector();
};

class UTerrainInfo : public AActor
{
public:
	// Variables

	// unions and structs moved to UnTerrain.h

	int32		__VarBeg;	// Variable-Initialising-Hack

	GLuint AlphaMap1;
	GLuint AlphaMap2;
	GLuint AlphaMap3;

	int32						TerrainSectorSize;
	UTexture*				TerrainMap;
	UTexture*				VertexLightMap; // sjs
	UVector					TerrainScale;
	jfArray<UTerrainLayer*, uint32>			Layers;
	jfArray<_DecorationLayer, uint32> DecoLayers;
	float					DecoLayerOffset;
	bool					Inverted;

	// This option means use half the graphics res for Karma collision.
	// Note - Karma ignores per-quad info (eg. 'invisible' and 'edge-turned') with this set to true.
	bool					bKCollisionHalfRes;

	//
	// Internal data
	//
	int32						JustLoaded;
	jfArray<_DecorationLayerData, uint32>	DecoLayerData;
	jfArray<UTerrainSector*, uint32>	Sectors;
	jfArray<UVector, uint32>			Vertices;
	int32						HeightmapX;
	int32						HeightmapY;
	int32						SectorsX;
	int32						SectorsY;
	UTerrainPrimitive*		Primitive;
	jfArray<_NormalPair, uint32>		FaceNormals;
	UVector					ToWorld[4];
	UVector					ToHeightmap[4];
	jfArray<int32, uint32>				SelectedVertices;
	int32						ShowGrid;
	//jfArray<int32, uint32>				QuadVisibilityBitmap;
	int8				*QuadVisibilityBitmap;
	jfArray<int32, uint32>				EdgeTurnBitmap;
	jfArray<UMaterial*, uint32>		QuadDomMaterialBitmap;
	jfArray<int32, uint32>				RenderCombinations;
	jfArray<int32, uint32>				VertexStreams;
	jfArray<UColor, uint32>			VertexColors;
	jfArray<UColor, uint32>			PaintedColor;		// editor only
	UTexture*				CollapsedLayers;

	// OLD
	UTexture*				OldTerrainMap;
	jfArray<uint8, uint32>			OldHeightmap;

	int32*					myIndices; // rendering hack
	int32						myNumIndices;
	int32						myRenderIDVerts;

	int32		__VarEnd;	// Variable-Initialising-Hack

	// Heightmap manipulation functions
//	native final function PokeTerrain( UVector WorldLocation, int32 Radius, int32 MaxDepth );

	// Constructor
	UTerrainInfo( void );
	~UTerrainInfo( void ) { if( myIndices ) delete [] myIndices; }

	// UObject interface
	virtual bool SetVariable( const UProperty& Prop );
	// no need for special serializing
	// ... oder doch? es gibt die Funktion '?Serialize@UTerrainInfo@@UAEXAAVFArchive@@@Z' in der 'Engine.dll'

	virtual void Tick();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UTerrainInfo);
};

