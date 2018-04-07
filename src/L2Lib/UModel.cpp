#include "UBase.h"
#include "UPackage.h"

#include "UModel.h"

UModel::UModel(void)
	: UPrimitive()
{
}

bool UModel::SetVariable(const class UProperty& Prop)
{
	if(UPrimitive::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Format") == 0)
	{
		//Format = *(uint8*)Prop.Data;
	}

	return false;
}

void UModel::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;

	//if(Data)
	//	delete [] Data;
}

UPackage& UModel::Serialize(UPackage& Ar)
{
	UPrimitive::Serialize( Ar );

	Ar >> Vectors;
	Ar >> Points;
	Ar >> Nodes;
	Ar >> Surfs;
	Ar >> Verts;

	Ar >> NumSharedSides;

	Ar >> NumZones;
	//CHECK( NumZones > FBspNode::MAX_ZONES );
	for( int i = 0; i < NumZones; i++ )
		Ar >> Zones[i];

	Ar >> *(UObject**)&Polys;

	// wie sich das hier weiter verhällt müsste erst noch untersucht werden!
	//if( Ar.Ver( ) <= 61 )
		return Ar;

	// LightMaps werden hier anders gespeichert!
	if( Ar.Ver( ) < UPACKAGE_UNREAL2_VERSION )
	{
		Ar >> LightMap;
		Ar >> LightBits;
	}

	Ar >> Bounds;
	Ar >> LeafHulls;	// ?
	Ar >> Leaves;		// ?
	Ar >> Lights;

	// Other variables.
	int8 t;
	Ar >> t;
	RootOutside = t;
	Ar >> t; // MoverLink; ??
	Linked = t;

	// the following doesn't exist on unreal1-tech levels
	if( Ar.Ver( ) < UPACKAGE_UNREAL2_VERSION )
		return Ar;

	//Ar >> UnkownData;

	// Unreal2-tech-lighting
	//Ar >> LightMaps;
	//Ar >> LightMapTextures;

	return Ar;
}

void UModel::finishLoad()
{
	if(_UIsInitialized)
		return;

	for(uint32 i = 0; i < Nodes.Size(); i++)
	{
		UBspNode &Node = Nodes[i];
		UBspSurf &Surf = Surfs[Node.iSurf];

		if(Surf.Material)
			Surf.Material->finishLoad();
	}

	Polys->finishLoad();

	_UIsInitialized = true;
}
