#pragma once

#include "UMath.h"
#include "UPrimitive.h"
#include "UZoneInfo.h"
#include "UProperty.h"

class UPointRegion : public UObject
{
public:
	UZoneInfo *Zone;
	int32 iLeaf;
	int8 ZoneNumber;
public:
	virtual bool SetVariable(const class UProperty& Prop);
public:
	UPointRegion();
};

// Flags associated with a Bsp node.
enum DLLEXPORT EBspNodeFlags
{
	// Flags.
	NF_NotCsg			= 0x01, // Node is not a Csg splitter, i.e. is a transparent poly.
	NF_NotVisBlocking   = 0x04, // Node does not block visibility, i.e. is an invisible collision hull.
	NF_BrightCorners	= 0x10, // Temporary.
	NF_IsNew 		 	= 0x20, // Editor: Node was newly-added.
	NF_IsFront     		= 0x40, // Filter operation bounding-sphere precomputed and guaranteed to be front.
	NF_IsBack      		= 0x80, // Guaranteed back.
};

class DLLEXPORT UBspNode
{
public:
	enum {MAX_NODE_VERTICES=16};	// Max vertices in a Bsp node, pre clipping.
	enum {MAX_FINAL_VERTICES=24};	// Max vertices in a Bsp node, post clipping.
	enum {MAX_ZONES=64};			// Max zones per level.

	// Persistent information.
	UPlane			Plane;			// 16 Plane the node falls INTo (X, Y, Z, W).
	uint64			ZoneMask;		// 8  Bit mask for all zones at or below this node (up to 64).
	int32				iVertPool;		// 4  Index of first vertex in vertex pool, =iTerrain if NumVertices==0 and NF_TerrainFront.
	int32				iSurf;			// 4  Index to surface information.

	// iBack:  4  Index to node in front (in direction of Normal).
	// iFront: 4  Index to node in back  (opposite direction as Normal).
	// iPlane: 4  Index to next coplanar poly in coplanar list.
	union { int32 iBack; int32 iChild[1]; };
	        int32 iFront;
			int32 iPlane;

	int32				iCollisionBound;// 4  Collision bound.
	int32				iRenderBound;	// 4  Rendering bound.
	uint8			iZone[2];		// 2  Visibility zone in 1=front, 0=back.
	uint8			NumVertices;	// 1  Number of vertices in node.
	uint8			NodeFlags;		// 1  Node flags.
	int32				iLeaf[2];		// 8  Leaf in back and front, INDEX_NONE=not a leaf.

	int32 BaseIndex;
	
	// Input - Serializer
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UBspNode);
};

class DLLEXPORT UBspSurf
{
public:

	// Persistent info.
	UMaterial*	Material;
//	UTexture*	Texture;		// 4 Texture map.
	uint32		PolyFlags;		// 4 Polygon flags.
	int32			pBase;			// 4 Polygon & texture base poINT index (where U,V==0,0).
	int32			vNormal;		// 4 Index to polygon normal.
	int32			vTextureU;		// 4 Texture U-vector index.
	int32			vTextureV;		// 4 Texture V-vector index.
	int32			iLightMap;		// 4 Light mesh.
	int32			iBrushPoly;		// 4 Editor brush polygon index.
	uint16		PanU;			// 2 U-Panning value.
	uint16		PanV;			// 2 V-Panning value.
	AActor /*ABrush**/		*Actor;			// 4 Brush actor owning this Bsp surface.
	//jfArray<UDecal, uint32>	Decals; // TArray // 12 Array decals on this surface
	jfArray2<int32, uint32>	Nodes; // TArray // 12 Nodes which make up this surface

	// UT2K3 Variables
	UPlane		Plane;

	virtual void setL2LibVars(L2LibVarsHolder *vars);

	// Input - Serializer
	UPackage& Serialize( UPackage& Ar );
	IMPLEMENT_USERIALIZER( UBspSurf );
public:
	UBspSurf()
	{
		Actor = 0;
	}
};

class DLLEXPORT ULeaf
{
public:
	// Variables.
	int32 iZone;          // The zone this convex volume is in.
	int32 iPermeating;    // Lights permeating this volume considering shadowing.
	int32 iVolumetric;    // Volumetric lights hitting this region, no shadowing.
	uint64 VisibleZones; // Bit mask of visible zones from this convex volume.

	// Functions.
	ULeaf( void ) { }
	ULeaf( int32 iInZone, int32 InPermeating, int32 InVolumetric, uint64 InVisibleZones )
	:	iZone(iInZone), iPermeating(InPermeating), iVolumetric(InVolumetric), VisibleZones(InVisibleZones) { }

/*	friend FArchive& operator<<( FArchive& Ar, FLeaf& L ) {
		guard(FLeaf<<);
		return Ar << AR_INDEX(L.iZone) << AR_INDEX(L.iPermeating) << AR_INDEX(L.iVolumetric) << L.VisibleZones;
		unguard;
	}*/

	// Input - Serializer
	UPackage& Serialize( UPackage& Ar )
	{
		return Ar >> UIndex( iZone ) >> UIndex( iPermeating ) >> UIndex( iVolumetric ) >> VisibleZones;
	}
	IMPLEMENT_USERIALIZER( ULeaf );
};

class DLLEXPORT UZoneProperties
{
public:
	// Variables.
	UObject /*AZoneInfo**/	*ZoneActor;		// Optional actor defining the zone's property.
	float		LastRenderTime;	// Most recent level TimeSeconds when rendered.
	uint64		Connectivity;	// (Connect[i]&(1<<j))==1 if zone i is adjacent to zone j.
	uint64		Visibility;		// (Connect[i]&(1<<j))==1 if zone i can see zone j.

	// Serializer.
/*	friend FArchive& operator<<( FArchive& Ar, FZoneProperties& P ) {
		guard(FZoneProperties<<);
		return Ar << *(UObject**)&P.ZoneActor << P.Connectivity << P.Visibility;
		Ar << P.LastRenderTime;
		unguard;
	}*/

	// Input - Serializer
	UPackage& Serialize( UPackage& Ar )
	{
		//Ar >> *(UObject**)&ZoneActor;
		int32 Index;
		Ar >> UIndex(Index);
		Ar >> Connectivity >> Visibility;
		if( Ar.Ver( ) >= UPACKAGE_UNREAL2_VERSION )
			Ar >> LastRenderTime;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UZoneProperties );

//	FZoneProperties( void ) { ZoneActor = 0; }
//	~FZoneProperties( void ) { delete ZoneActor; /*HACK!!*/ }
};

class DLLEXPORT ULightMapIndex
{
public:
	int32		DataOffset;
	int32		iLightActors;
	UVector	Pan;
	float	UScale, VScale;
	int32     UClamp, VClamp;
	uint8	UBits, VBits;

	// Input - Serializer
	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> DataOffset;
		Ar >> Pan;
		Ar >> UIndex( UClamp ) >> UIndex( VClamp );
		Ar >> UScale >> VScale;
		Ar >> iLightActors;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( ULightMapIndex );
};

class DLLEXPORT UVert
{
public:
	// Variables.
	int32 	pVertex;	// Index of vertex.
	int32		iSide;		// If shared, index of unique side. Otherwise INDEX_NONE.

	// Functions.
/*	friend FArchive& operator << ( FArchive &Ar, FVert &Vert ) {
		guard(FVert<<);
		return Ar >> AR_INDEX(Vert.pVertex) << AR_INDEX(Vert.iSide);
		unguard;
	}*/

	// Input - Serializer
	inline UPackage& Serialize( UPackage& Ar )
	{
		return Ar >> UIndex( pVertex ) >> UIndex( iSide );
	}
	IMPLEMENT_USERIALIZER( UVert );
};

enum DLLEXPORT EPolyFlags
{
	// Regular in-game flags.
	PF_Invisible		= 0x00000001,	// Poly is invisible.
	PF_NotSolid			= 0x00000008,	// Poly is not solid, doesn't block.
	PF_Semisolid	  	= 0x00000020,	// Poly is semi-solid = collision solid, Csg nonsolid.
	PF_GeomMarked	  	= 0x00000040,	// Geometry mode sometimes needs to mark polys for processing later.
	PF_TwoSided			= 0x00000100,	// Poly is visible from both sides.
	PF_Unk1				= 0x00002000,	// pf on bounding bsp, invisible
	PF_Unk2				= 0x00008000,	// pf on bounding bsp
	PF_Unk3				= 0x00400000,	// pf on bounding bsp
	PF_Portal			= 0x04000000,	// Portal between iZones.
	PF_Unk4				= 0x08000000,	// pf on Ant Nest's invisible bsp
	PF_Mirrored         = 0x20000000,   // Mirrored BSP surface.

	// Editor flags.
	PF_Memorized     	= 0x01000000,	// Editor: Poly is remembered.
	PF_Selected      	= 0x02000000,	// Editor: Poly is selected.

	// Internal.
	PF_EdProcessed 		= 0x40000000,	// FPoly was already processed in editorBuildFPolys.
	PF_EdCut       		= 0x80000000,	// FPoly has been split by SplitPolyWithPlane.

	// Combinations of flags.
	PF_NoEdit			= PF_Memorized | PF_Selected | PF_EdProcessed | PF_EdCut,
	PF_NoImport			= PF_NoEdit | PF_Memorized | PF_Selected | PF_EdProcessed | PF_EdCut,
	PF_AddLast			= PF_Semisolid | PF_NotSolid,
	PF_NoAddToBSP		= PF_EdCut | PF_EdProcessed | PF_Selected | PF_Memorized
};

class DLLEXPORT UPoly
{
public:
	enum {MAX_VERTICES=32/*16*/}; // Maximum vertices an FPoly may have.
	enum {VERTEX_THRESHOLD=MAX_VERTICES-2}; // Threshold for splitting INTo two.

	UVector		Base;        	// Base poINT of polygon.
	UVector		Normal;			// Normal of polygon.
	UVector		TextureU;		// Texture U FVector.
	UVector		TextureV;		// Texture V FVector.
	UVector		Vertex[MAX_VERTICES]; // Actual vertices.
	uint32		PolyFlags;		// FPoly & Bsp poly bit flags (PF_).
	UObject /*ABrush**/		*Actor;			// Brush where this originated, or 0.
	UMaterial *Texture;		// Texture map.
	char		*ItemName;		// Item name.
	int32			NumVertices;	// Number of vertices.
	int32			iLink;			// iBspSurf, or brush FPoly index of first identical polygon, or MAXWORD.
	int32			iBrushPoly;		// Index of editor solid's polygon this originated from.
	int16		PanU,PanV;		// Texture panning values.
public:
	inline UPackage& Serialize(UPackage& Ar)
	{
		int32 Index;

		Ar >> UIndex(NumVertices);
		//CHECK( NumVertices < 3 || NumVertices > MAX_VERTICES );
		Ar >> Base >> Normal >> TextureU >> TextureV;
		for( INT i = 0 ; i < NumVertices; i++ )
			Ar >> Vertex[i];
		Ar >> PolyFlags;
		Ar >> *(UObject**)&Actor;
		Ar >> *(UObject**)&Texture;
		Ar >> UIndex(Index);
		ItemName = Ar.Names[Index].Name;
		Ar >> UIndex(iLink) >> UIndex(iBrushPoly);
		Ar >> PanU >> PanV;
		Ar.Seek(4);
		return Ar;
	}
	IMPLEMENT_USERIALIZER(UPoly);
};

/*class UDecal
{
public:
	// Variables.
	UVector			Vertices[4];	// Vertices of decal on surface (offset from the surface base).
	ADecal*			Actor;			// ADecal actor associated with this decal.
	jfArray2<int32>	Nodes; // TArray // The FBspNode indices on which this decal lies.

	// Input - Serializer
	inline UPackage& Serialize( UPackage& Ar )
	{
		if( false )
			Ar >> *(UObject**)&Actor;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UDecal );
};*/

class DLLEXPORT UPolys : public UObject
{
public:
	jfArray<UPoly, uint32> Elements;

	GLuint m_RenderLists;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UPolys);
public:
	UPolys(void);
};
