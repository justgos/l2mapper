#include "UBase.h"
#include "UPackage.h"

#include "UObj.h"
#include "UPackageManager.h"

UPointRegion::UPointRegion()
	: UObject()
{
	iLeaf = -1;
}

bool UPointRegion::SetVariable(const class UProperty& Prop)
{
	if(UObject::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Zone") == 0)
	{
		Zone = *(UZoneInfo**)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "iLeaf") == 0)
	{
		iLeaf = *(int32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "ZoneNumber") == 0)
	{
		ZoneNumber = *(int8*)Prop.Data;
		return true;
	}

	return false;
}

UPackage& UBspNode::Serialize(UPackage& Ar)
{
	// ich bin mir hier nicht ganz sicher!
	Ar >> Plane;
	Ar >> ZoneMask;
	Ar >> NodeFlags; // ??
	Ar >> UIndex( iVertPool );
	Ar >> UIndex( iSurf );

	Ar >> UIndex( iBack );
	Ar >> UIndex( iFront );
	Ar >> UIndex( iPlane );

	Ar >> UIndex( iCollisionBound );
	Ar >> UIndex( iRenderBound );

	if( Ar.Ver( ) >= UPACKAGE_UNREAL2_VERSION )
	{
		UVector			vUnknownPoint;
		int32				UnkownID; // meistens "K35C" - vielleicht irgendwie collision??
		uint64			PrblyConnZones;
		uint64			PrblyVisZones;
		Ar >> vUnknownPoint;
		Ar >> UnkownID;
		//if( Ar.Ver( ) <= 121 ) // übler ut2004 Hack
			Ar >> PrblyConnZones >> PrblyVisZones;
	}

	int32 temp;
	Ar >> UIndex( temp ); iZone[0] = temp;
	Ar >> UIndex( temp ); iZone[1] = temp;
	Ar >> NumVertices;

//	Ar >> FIndex( iLeaf[0] );
//	Ar >> FIndex( iLeaf[1] );
	Ar >> iLeaf[0] >> iLeaf[1];

	// übler ut2003/ut2004 hack
	if( Ar.Ver( ) >= UPACKAGE_UNREAL2_VERSION )
	{
		Ar.Seek(0xC);
	}

	return Ar;
}

void UBspSurf::setL2LibVars(L2LibVarsHolder *vars)
{
	if(Material)
	{
		Material->setL2LibVars(vars);
	}
}

UPackage& UBspSurf::Serialize( UPackage& Ar )
{
	Ar >> *(UObject**)&Material;
	Ar >> PolyFlags;
	Ar >> UIndex( pBase );
	Ar >> UIndex( vNormal );
	Ar >> UIndex( vTextureU );
	Ar >> UIndex( vTextureV );

	iLightMap = -1; // surface will be referenced by 'FLightMap' (iIndex)
	Ar >> UIndex( iBrushPoly );
	Ar >> *(UObject**)&Actor;
	//int32 Index;
	//Ar >> UIndex(Index);
	Ar >> Plane;
	float LightMapScale;
	Ar >> LightMapScale;
	uint32 SomeDWORD;
	Ar >> SomeDWORD; // ?? - sicher nicht PanU und PanV!
	PanU = PanV = 0; // das scheint jetzt alles von 'pBase'-Punkt geregelt zu werden

	return Ar;
}

UPolys::UPolys(void)
	: UObject()
{
}

bool UPolys::SetVariable(const class UProperty& Prop)
{
	if(UObject::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Format") == 0)
	{
		//Format = *(uint8*)Prop.Data;
	}

	return false;
}

void UPolys::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;

	//if(Data)
	//	delete [] Data;
}

UPackage& UPolys::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	int32 DbNum, DbMax;

	Ar >> DbNum >> DbMax;

	//Ar >> UIndex(DbMax);

	Elements.alloc(DbMax);

	for(uint32 i = 0; i < Elements.Size(); i++)
		Ar >> Elements[i];

	return Ar;
}

void UPolys::finishLoad()
{
	if(_UIsInitialized)
		return;

	m_RenderLists = glGenLists(Elements.Size());

	uint32 offset = 0;

	for(uint32 surf = 0; surf < Elements.Size(); surf++)
	{
		UPoly &ts = Elements[surf];
		glNewList(m_RenderLists + surf, GL_COMPILE);

		uint32 pf = Elements[surf].PolyFlags;
		if(Elements[surf].PolyFlags & EPolyFlags::PF_Invisible
			|| Elements[surf].PolyFlags & EPolyFlags::PF_NotSolid
			|| Elements[surf].PolyFlags & 0x00000080
			|| Elements[surf].PolyFlags & 0x00002000
			//|| Elements[surf].PolyFlags & 0x00008000
			|| UTIL_strcmpi(Elements[surf].ItemName, "Sheet") == 0
			//|| (UTIL_strcmpi(Elements[surf].ItemName, "None") == 0
			//|| Elements[surf].iLink == 0xffffffff
			//|| (Elements[surf].Texture && UTIL_strcmpi(((UObject*)Elements[surf].Texture)->ObjectName, "godad_dragoncave_t.field_B_cs") == 0) // TexCoordSource
			)
		{
			glEndList();
			continue;
		}

		if((Elements[surf].Texture && UTIL_strcmpi(((UObject*)Elements[surf].Texture)->ObjectName, "godad_dragoncave_t.field_B_cs") == 0))
		{
			UPoly *pt = &(Elements[surf]);
			int gg = 0;
		}

		glPushMatrix();
			glBegin(GL_TRIANGLE_FAN);
				for(uint32 f = 0; f < Elements[surf].NumVertices; f++)
				{
					float tu = ((ts.Vertex[f] - ts.Base) | ts.TextureU) / 128.;
					float tv = ((ts.Vertex[f] - ts.Base) | ts.TextureV) / 128.;
					glTexCoord2d(tu, tv);
					glNormal3f(ts.Normal.X, ts.Normal.Z, ts.Normal.Y);
					glVertex3f(ts.Vertex[f].X, ts.Vertex[f].Z, ts.Vertex[f].Y);
				}
			glEnd();
		glPopMatrix();

		glEndList();
	}

	//m_RenderLists = m_RenderLists;
	OnLoaded();

	_UIsInitialized = true;
}
