#include "UBase.h"
#include "UPackage.h"
#include "math.h"

#include "UTerrain.h"

#include "ULevel.h"

#define TERRAIN_ZSCALE			(1.0f/128.0f)

UTerrainLayer::UTerrainLayer()
	: UObject()
{
}

bool UTerrainLayer::SetVariable(const class UProperty& Prop)
{
	if(UObject::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Texture") == 0)
	{
		Texture = *(UTexture**)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "AlphaMap") == 0)
	{
		AlphaMap = *(UTexture**)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "UScale") == 0)
	{
		UScale = *(float*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "VScale") == 0)
	{
		VScale = *(float*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "UPan") == 0)
	{
		UPan = *(float*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "VPan") == 0)
	{
		VPan = *(float*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "TextureRotation") == 0)
	{
		TextureRotation = *(float*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "LayerRotation") == 0)
	{
		LayerRotation = *(URotator*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Scale") == 0)
	{
		Scale = *(UVector*)Prop.Data;
		return true;
	}

	return false;
}

UTerrainSector::UTerrainSector()
	: AActor()
{
	m_TerrainInfo = 0;
}

bool UTerrainSector::SetVariable(const UProperty& Prop)
{
	if(AActor::SetVariable(Prop))
		return true;

	return false;
}

void UTerrainSector::Init() // int32 InSectorX, int32 InSectorZ
{
	if(!m_TerrainInfo)
		return;

	if(!m_TerrainInfo->TerrainMap)
		return;

	SectorX = OffsetX / 2 / 2048;
	SectorZ = OffsetZ / 2 / 2048;

	//m_RenderList = glGenLists(1);

	//glNewList(m_RenderList, GL_COMPILE);

	float Scale = 128;
	float Restit = 9567.05;
	//float HeightScale = 76;
	float TextureScale = 1;

	m_VertexArray = new float[17 * 17 * 3];

	for(uint16 iy = 0; iy < 17; iy++)
	{
		for(uint16 ix = 0; ix < 17; ix++)
		{
			uint16 hmx, hmy;

			hmx = ix + SectorX * 16;
			hmy = iy + SectorZ * 16;

			m_VertexArray[(iy * 17 + ix) * 3 + 0] = hmx * Scale;
			//m_VertexArray[(iy * 17 + ix) * 3 + 1] = *(uint16*)(m_TerrainInfo->TerrainMap->Data + (min(hmy, 255) * m_TerrainInfo->TerrainMap->Width + min(hmx, 255)) * 2) / 256. * m_TerrainInfo->TerrainScale.Z - Restit; //  + m_TerrainInfo->m_Location.Y / m_TerrainInfo->m_DrawScale
			float TerrainHeight = *((uint16*)m_TerrainInfo->TerrainMap->Data + (min(hmy, 255) * m_TerrainInfo->TerrainMap->Width + min(hmx, 255)));
			TerrainHeight = (-32768.0f + TerrainHeight) * TERRAIN_ZSCALE;
			m_VertexArray[(iy * 17 + ix) * 3 + 1] = TerrainHeight * (m_TerrainInfo->TerrainScale.Z) / 2 + m_TerrainInfo->m_Location.Y;
			m_VertexArray[(iy * 17 + ix) * 3 + 2] = hmy * Scale;
		}
	}

	m_IndexArray = new GLuint[16 * 16 * 6];

	for(uint16 iy = 0; iy < 16; iy++)
	{
		for(uint16 ix = 0; ix < 16; ix++)
		{
			//int16 quadIndex = (iy * 16 + ix);
			uint16 quadX = SectorX * 16 + ix;
			uint16 quadZ = SectorZ * 16 + iy;
			uint16 quadIndex = quadZ * 256 + quadX;
			//bool isVisible = m_TerrainInfo->QuadVisibilityBitmap[(SectorZ * 16 + SectorX) * 32 + (int8)(quadIndex / 8)] & (0x00000001 << (7 - quadIndex % 8));
			bool isVisible = m_TerrainInfo->QuadVisibilityBitmap[(uint16)(quadIndex / 8)] & (0x00000001 << (quadIndex % 8));

			if(!isVisible)
			{
				m_IndexArray[(iy * 16 + ix) * 6 + 0] = (iy * 17 + ix);
				m_IndexArray[(iy * 16 + ix) * 6 + 1] = (iy * 17 + ix);
				m_IndexArray[(iy * 16 + ix) * 6 + 2] = (iy * 17 + ix);

				m_IndexArray[(iy * 16 + ix) * 6 + 3] = (iy * 17 + ix);
				m_IndexArray[(iy * 16 + ix) * 6 + 4] = (iy * 17 + ix);
				m_IndexArray[(iy * 16 + ix) * 6 + 5] = (iy * 17 + ix);
				continue;
			}

			m_IndexArray[(iy * 16 + ix) * 6 + 0] = (iy * 17 + ix);
			m_IndexArray[(iy * 16 + ix) * 6 + 1] = ((iy + 1) * 17 + ix);
			m_IndexArray[(iy * 16 + ix) * 6 + 2] = (iy * 17 + (ix + 1));

			m_IndexArray[(iy * 16 + ix) * 6 + 3] = (iy * 17 + (ix + 1));
			m_IndexArray[(iy * 16 + ix) * 6 + 4] = ((iy + 1) * 17 + ix);
			m_IndexArray[(iy * 16 + ix) * 6 + 5] = ((iy + 1) * 17 + (ix + 1));
		}
	}

	m_TexCoordArrays = new float*[m_TerrainInfo->Layers.Size()];
	m_TexCoordBuffers = new GLuint[m_TerrainInfo->Layers.Size()];

	for(uint32 layer = 0; layer < m_TerrainInfo->Layers.Size(); layer++)
	{
		m_TexCoordArrays[layer] = new float[17 * 17 * 2];

		for(uint16 iy = 0; iy < 17; iy++)
		{
			for(uint16 ix = 0; ix < 17; ix++)
			{
				//float su = m_TerrainInfo->Layers[layer]->UScale;
				//float sx = m_TerrainInfo->Layers[layer]->Scale.X;
				//float scaleX = m_TerrainInfo->Layers[layer]->UScale * (m_TerrainInfo->Layers[layer]->Scale.X / m_TerrainInfo->TerrainScale.X);
				m_TexCoordArrays[layer][(iy * 17 + ix) * 2 + 0] = (SectorX * 16 + ix) / m_TerrainInfo->Layers[layer]->UScale * (m_TerrainInfo->Layers[layer]->Scale.X / m_TerrainInfo->TerrainScale.X) * 2.0;
				m_TexCoordArrays[layer][(iy * 17 + ix) * 2 + 1] = (SectorZ * 16 + iy) / m_TerrainInfo->Layers[layer]->VScale * (m_TerrainInfo->Layers[layer]->Scale.Y / m_TerrainInfo->TerrainScale.Y) * 2.0;
				//m_TexCoordArrays[layer][(iy * 17 + ix) * 2 + 0] = 0;
				//m_TexCoordArrays[layer][(iy * 17 + ix) * 2 + 1] = 0;
			}
		}
	}

	m_AlphaMapCoordArray = new float[17 * 17 * 2];

	for(uint16 iy = 0; iy < 17; iy++)
	{
		for(uint16 ix = 0; ix < 17; ix++)
		{
			uint16 hmx, hmy;

			hmx = ix + SectorX * 16;
			hmy = iy + SectorZ * 16;

			m_AlphaMapCoordArray[(iy * 17 + ix) * 2 + 0] = hmx / 255.;
			m_AlphaMapCoordArray[(iy * 17 + ix) * 2 + 1] = hmy / 255.;
		}
	}

	m_LightValArray = new float[17 * 17];
	for(uint32 iy = 0; iy < 17; iy++)
	{
		for(uint32 ix = 0; ix < 17; ix++)
		{
			if(UnkArr0.Size() != 0)
				m_LightValArray[iy * 17 + ix] = UnkArr0[iy * 17 + ix] / 255.;
			else
				m_LightValArray[iy * 17 + ix] = 1;
			//m_LightValArray[iy * 17 + ix] = 0;
		}
	}
}

void UTerrainSector::finishLoad()
{
	if(_UIsInitialized)
		return;

	if(m_TerrainInfo && m_TerrainInfo->TerrainMap)
	{
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		glBufferData(GL_ARRAY_BUFFER, 17 * 17 * 3 * sizeof(float), m_VertexArray, GL_STATIC_DRAW);
 
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 16 * 16 * 6 * sizeof(uint32), m_IndexArray, GL_STATIC_DRAW);

		for(uint32 layer = 0; layer < m_TerrainInfo->Layers.Size(); layer++)
		{
			glGenBuffers(1, &m_TexCoordBuffers[layer]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TexCoordBuffers[layer]);
			
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 17 * 17 * 2 * sizeof(float), m_TexCoordArrays[layer], GL_STATIC_DRAW);
		}

		glGenBuffers(1, &m_AlphaMapCoordBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_AlphaMapCoordBuffer);

		glBufferData(GL_ARRAY_BUFFER, 17 * 17 * 2 * sizeof(float), m_AlphaMapCoordArray, GL_STATIC_DRAW);

		glGenBuffers(1, &m_LightValBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LightValBuffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 17 * 17 * sizeof(float), m_LightValArray, GL_STATIC_DRAW);
	}

	_UIsInitialized = true;
}

void UTerrainSector::Tick()
{
	if(!m_TerrainInfo)
		return;

	if(!m_TerrainInfo->TerrainMap)
		return;

	_vector3 aPos = _vector3(BoundingBox.Min.X, BoundingBox.Min.Z, BoundingBox.Min.Y);

	if((aPos - m_TerrainInfo->getLevel()->_camPos).Length() > m_TerrainInfo->getLevel()->_visibleDist)
		return;

	for(uint32 layer = 0; layer < m_TerrainInfo->Layers.Size(); layer++)
	{
		char buf[256];
		sprintf(buf, "MultiTexCoord%d", layer);
		int loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, buf);
		if(loc > -1)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffers[layer]);

			glVertexAttribPointer(loc,                  // index
									2,        // number of values per vertex
									GL_FLOAT,                 // type (GL_FLOAT)
									GL_FALSE,
									2 * sizeof(float),               // stride (offset to next vertex data)
									(const GLvoid*)0);
			glEnableVertexAttribArray(loc);
		}
	}

	int loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "AlphaMapCoords");
	if(loc > -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_AlphaMapCoordBuffer);

		glVertexAttribPointer(loc,                  // index
								2,        // number of values per vertex
								GL_FLOAT,                 // type (GL_FLOAT)
								GL_FALSE,
								2 * sizeof(float),               // stride (offset to next vertex data)
								(const GLvoid*)0);
		glEnableVertexAttribArray(loc);
	}
	
	loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "LightVal");
	if(loc > -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_LightValBuffer);

		glVertexAttribPointer(loc,                  // index
								1,        // number of values per vertex
								GL_FLOAT,                 // type (GL_FLOAT)
								GL_FALSE,
								1 * sizeof(float),               // stride (offset to next vertex data)
								(const GLvoid*)0);
		glEnableVertexAttribArray(loc);
	}

	g_shader.SetAttribute3f(glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "CamPos"), m_TerrainInfo->getLevel()->_camPos.x, m_TerrainInfo->getLevel()->_camPos.y, m_TerrainInfo->getLevel()->_camPos.z);

	loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "Vertex");
	if(loc > -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		glVertexAttribPointer(loc,                  // index
								3,        // number of values per vertex
								GL_FLOAT,                 // type (GL_FLOAT)
								GL_FALSE,
								3 * sizeof(float),               // stride (offset to next vertex data)
								(const GLvoid*)0);
		glEnableVertexAttribArray(loc);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 3 * sizeof(float), 0);
 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

	//g_shader.Enable(0);
	glDrawElements(GL_TRIANGLES, 16 * 16 * 6, GL_UNSIGNED_INT, 0);
	//g_shader.Disable();

	glDisableClientState(GL_VERTEX_ARRAY);

	loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "Vertex");
	if(loc > -1)
	{
		glDisableVertexAttribArray(loc);
	}
	
	loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, "LightVal");
	if(loc > -1)
	{
		glDisableVertexAttribArray(loc);
	}

	for(uint32 layer = 0; layer < m_TerrainInfo->Layers.Size(); layer++)
	{
		char buf[256];
		sprintf(buf, "MultiTexCoord%d", layer);
		int loc = glGetAttribLocation(g_shader.GetByName("L2Terrain")->program, buf);
		if(loc > -1)
		{
			glDisableVertexAttribArray(loc);
		}
	}
}

UPackage& UTerrainSector::Serialize(UPackage& Ar)
{
	AActor::Serialize(Ar);

	int32 Index;

	int32 unk_32;

	UObject *TObject;

	Ar >> TObject;
	//Ar.Seek(1);

	m_TerrainInfo = (UTerrainInfo*)TObject;

	Ar.Seek(7); // num segments x and z?

	Ar >> OffsetX;
	Ar >> OffsetZ;

	m_Location.X = OffsetX / 2;
	m_Location.Z = OffsetZ / 2;

	Ar.Seek(1);

	//Ar.Seek(17);

	Ar >> BoundingBox;

	uint32 t = Ar.Tell();

	Ar >> CellNum;
	Ar >> SectorWidth;

	Ar.Seek(1);

	//Ar >> UIndex(Index);

	/*if(Index == 289)
		return Ar;

	for(uint32 i = 0; i < 50; i++)
	{
		Ar.Seek(t + i, U_SEEK_SET);
		int32 t16;
		Ar >> (t16);
		printf("%d, %d\n", i, t16);
	}*/

	//Ar.Seek(3)

	t = Ar.Tell();
	for(uint32 i = 0; i < 4000; i++)
	{
		Ar.Seek(t + i, U_SEEK_SET);
		Ar >> UIndex(Index);
		if(Index == 289)
		{
			Ar.Seek(-2);
			break;
		}
	}

	/*printf("===== %d =====", (Ar.Tell() - t));
	for(uint32 i = 0; i < 50; i++)
	{
		Ar.Seek(t + i, U_SEEK_SET);
		int8 t8;
		Ar >> (t8);
		printf("%d, 0x%02X\n", i, t8);
	}*/

	if(Index != 289)
	{
		return Ar;
	}

	uint32 Unk_32;

	Ar >> UnkArr0;
	Ar >> UnkArr1;
	Ar >> UnkArr2;
	Ar >> UnkArr3;
	Ar >> UnkArr4;
	Ar >> UnkArr5;
	Ar >> UnkArr6;
	Ar >> UnkArr7;

	Ar.Seek(64); // FFs, FEs ans zeros

	Ar >> TexInfo;

	return Ar;
}

UTerrainInfo::UTerrainInfo( void )
	: AActor()
{
	int32 OffsetBeg = ((size_t)&(((UTerrainInfo*)1)->__VarBeg)) - 1; // shutup gcc warning
	int32 OffsetEND = ((size_t)&(((UTerrainInfo*)1)->__VarEnd)) - 1;

	memset( ((BYTE*)this + OffsetBeg), 0, OffsetEND - OffsetBeg );

	myRenderIDVerts = -1;

	// defaultproperties
//	Texture = S_TerrainInfo;
	TerrainScale = UVector( 64, 64, 64 );
	//bStatic = true;
//	bStaticLighting = true;
//	bWorldGeometry = true;
//	bHidden = true;
	TerrainSectorSize = 16;
//	bKCollisionHalfRes = false
//	bUseDynamicLights = true;
}

bool UTerrainInfo::SetVariable( const UProperty& Prop )
{
	if(AActor::SetVariable(Prop))
		return true;

	UObject *t_Obj;

	if(UTIL_strcmpi(Prop.Name, "Layers") == 0)
	{
		if(UTIL_strcmpi(Prop.StructType, "TerrainLayer") == 0)
		{
			/*if(Prop.ArrayIndex > -1)
				Layers[Prop.ArrayIndex] = **(UTerrainLayer**)Prop.Data;
			else
				Layers[0] = **(UTerrainLayer**)Prop.Data;*/
			Layers.add(*(UTerrainLayer**)Prop.Data);
			UTerrainLayer* tl = Layers[Layers.Size() - 1];

			return true;
		}
	}
	else if(UTIL_strcmpi(Prop.Name, "MapX") == 0)
	{
		HeightmapX = *(int32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "MapY") == 0)
	{
		HeightmapY = *(int32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "TerrainMap") == 0)
	{
		TerrainMap = *(UTexture**)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "TerrainScale") == 0)
	{
		TerrainScale = *(UVector*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "QuadVisibilityBitmap") == 0)
	{
		QuadVisibilityBitmap = (int8*)Prop.Data;
		return true;
	}

	//if( AInfo::SetVariable( Prop ) )
	//	return true;

	/*PROP_GET_INT( TerrainSectorSize )
	PROP_GET_OBJECT( TerrainMap )
	PROP_GET_OBJECT( VertexLightMap ) // sjs
	PROP_GET_VECTOR( TerrainScale )
	// diese Strukturen können noch nicht in Unreal 2 gelesen werden
	if( myArVer > 110 ) {
		PROP_GET_ARRSTCT( Layers, UPS_TerrainLayer )
		PROP_ARRAY_BEG( DecoLayers )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, ShowOnTerrain, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, ScaleMap, UPT_Object )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, DensityMap, UPT_Object )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, ColorMap, UPT_Object )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, StaticMesh, UPT_Object )
			PROP_ARRAY_GET_STRUCT( DecoLayers, ScaleMultiplier, UPS_RangeVector )
			PROP_ARRAY_GET_STRUCT( DecoLayers, FadeoutRadius, UPS_Range )
			PROP_ARRAY_GET_STRUCT( DecoLayers, DensityMultiplier, UPS_Range )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, MaxPerQuad, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, Seed, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, AlignToTerrain, UPT_Int )
			PROP_ARRAY_GET_ENUM( DecoLayers, DrawOrder )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, ShowOnInvisibleTerrain, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, LitDirectional, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, DisregardTerrainLighting, UPT_Int )
			PROP_ARRAY_GET_SIMPLE( DecoLayers, RandomYaw, UPT_Int )
		PROP_ARRAY_END
	}
	PROP_GET_FLOAT( DecoLayerOffset )
	PROP_GET_BOOL( Inverted )

	// This option means use half the graphics res for Karma collision.
	PROP_GET_BOOL( bKCollisionHalfRes )

	// Internal data
	PROP_GET_INT( JustLoaded )
//	TArray<UDecorationLayerData>	DecoLayerData;
//	TArray<UTerrainSector>	Sectors;
//	TArray<FVector>			Vertices;
	PROP_GET_INT( HeightmapX )
	PROP_GET_INT( HeightmapY )
	PROP_GET_INT( SectorsX )
	PROP_GET_INT( SectorsY )
	PROP_GET_OBJECT( Primitive )
//	TArray<UNormalPair>		FaceNormals;
	PROP_GET_ARRSTCT( ToWorld, UPS_Vector )
	PROP_GET_ARRSTCT( ToHeightmap, UPS_Vector )
	PROP_ARRAY_SIMPLE( SelectedVertices )
	PROP_GET_INT( ShowGrid )
	PROP_ARRAY_SIMPLE( QuadVisibilityBitmap )
	PROP_ARRAY_SIMPLE( EdgeTurnBitmap )
//	TArray<UMaterial*>		QuadDomMaterialBitmap
	PROP_ARRAY_SIMPLE( RenderCombinations )
	PROP_ARRAY_SIMPLE( VertexStreams )
//	TArray<FColor>			VertexColors
//	TArray<FColor>			PaintedColor		// editor only
	PROP_GET_OBJECT( CollapsedLayers )

	// OLD
	PROP_GET_OBJECT( OldTerrainMap )*/
//	TArray<byte>			OldHeightmap;

	/*if(UTIL_strcmpi(Prop.Name, "QuadVisibilityBitmap") == 0)
	{
		int g = 0;
		return true;
	}*/

	return false;
}

void UTerrainInfo::Tick()
{
	Shader *shader = g_shader.GetByName("L2Terrain");

	glDisable(GL_ALPHA_TEST);

	UMatrix mvmat, prmat;
	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&mvmat.Data);
	glGetFloatv(GL_PROJECTION_MATRIX, (float*)&prmat.Data);
	mvmat *= prmat;
	g_shader.SetUniformMatrix4fv(shader, "ModelViewProjectionMatrix", mvmat.Data);
	
	g_shader.Enable(g_shader.GetByName("L2Terrain"));

	//g_shader.BindAttribLocation(0, 16, "TexInfo");
	//g_shader.BindAttribLocation(0, 17, "LightVal");

	char buf[256];

	for(uint32 i = 0; i < Layers.Size(); i++)
	{
		g_shader.SetActiveTexture(GL_TEXTURE0 + i);
		if(i > 0)
			glEnable(GL_TEXTURE_2D);
		UObject *TexObj = Layers[i]->Texture;
		if((UTexture*)Layers[i]->Texture != 0)
		{
			glBindTexture(GL_TEXTURE_2D, ((UTexture*)Layers[i]->Texture)->m_Handle);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		sprintf(buf, "tex%d", i);
		g_shader.SetUniform1i(shader, buf, i);
	}

	g_shader.SetActiveTexture(GL_TEXTURE0 + 13);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, AlphaMap1);
	g_shader.SetUniform1i(shader, "AlphaMap1", 13);

	g_shader.SetActiveTexture(GL_TEXTURE0 + 14);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, AlphaMap2);
	g_shader.SetUniform1i(shader, "AlphaMap2", 14);

	g_shader.SetActiveTexture(GL_TEXTURE0 + 15);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, AlphaMap3);
	g_shader.SetUniform1i(shader, "AlphaMap3", 15);

	//glBindTexture(GL_TEXTURE_2D, ((UTexture*)Layers[7]->Texture)->m_Handle);

	//g_shader.SetActiveTexture(GL_TEXTURE0_ARB);

	glPushMatrix();
		glTranslatef(m_Location.X, 0, m_Location.Z);
		for(uint32 i = 0; i < Sectors.Size(); i++)
		//for(uint32 i = 0; i < 20; i++)
		{
			Sectors[i]->Tick();
		}
	glPopMatrix();

	g_shader.SetActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	g_shader.SetActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	g_shader.SetActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	for(uint32 i = 0; i < Layers.Size(); i++)
	{
		g_shader.SetActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		if(i > 0)
			glDisable(GL_TEXTURE_2D);
	}

	g_shader.SetActiveTexture(GL_TEXTURE0);

	//g_shader.Disable();
	g_shader.Enable(L2LibVars->defaultObjectShader);
	g_shader.SetUniform1f(L2LibVars->defaultObjectShader, "cAlpha", 1.0);

	/*glPushMatrix();
		glTranslatef(m_Location.X, 5000, m_Location.Z);
		float layerSpacing = 5000;
		for(int i = 0; i < Layers.Size(); i++)
		{
			UTerrainLayer *layer = Layers[i];
			glBegin(GL_QUADS);
				if(layer->AlphaMap) //  && Layers[i]->AlphaMap->Palette.Colors.Size() == 0
				{
					layer->AlphaMap->StartPass();

					glTexCoord2f(0, 0);
					glNormal3f(0, 1, 0);
					glVertex3f(0, i * layerSpacing, 0);

					glTexCoord2f(1, 0);
					glNormal3f(0, 1, 0);
					glVertex3f(MAP_SIZE_X, i * layerSpacing, 0);

					glTexCoord2f(1, 1);
					glNormal3f(0, 1, 0);
					glVertex3f(MAP_SIZE_X, i * layerSpacing, MAP_SIZE_Z);

					glTexCoord2f(0, 1);
					glNormal3f(0, 1, 0);
					glVertex3f(0, i * layerSpacing, MAP_SIZE_Z);

					layer->AlphaMap->EndPass();
				}
			glEnd();
		}
	glPopMatrix();*/

	glEnable(GL_ALPHA_TEST);
}

UPackage& UTerrainInfo::Serialize(UPackage& Ar)
{
	/*uint32 t = Ar.Tell();

	for(uint32 u = 0; u < 100; u++)
	{
		Ar.Seek(t + u, U_SEEK_SET);
		int32 Index;
		Ar >> UIndex(Index);
		//if(Index < 0 && -Index - 1 < Imports.Size())
		//	printf("Import: %d, %s (%d)\n", u, Imports[-Index - 1].Name, Index);
		//if(Index > 0 && Index - 1 < Exports.Size())
		//	printf("Export: %d, %s (%d)\n", u, Exports[Index - 1].Name, Index);
		if(Index >= 0 && Index < Ar.Names.Size())
			//if(Ar.Names[Index].Name[0] == 'T' && Ar.Names[Index].Name[1] == 'e' && Ar.Names[Index].Name[2] == 'r')
				printf("%d Name: %s (%d, 0x%02X)\n", u, Ar.Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0));
	}*/

	AActor::Serialize(Ar);

	//return Ar;

	m_Location.X = (HeightmapX - 20) * MAP_SIZE_X;
	m_Location.Z = (HeightmapY - 18) * MAP_SIZE_Z;

	//Sectors.alloc(256);

	int8 PrefixLen = strlen("TerrainSector");

	UTerrainSector *TSector;

	for(uint32 i = 0; i < Ar.Exports.Size(); i++)
	{
		if(UTIL_strcmpi(Ar.Exports[i].BaseClass, "TerrainSector") == 0)
		{
			char *sectorName = Ar.Exports[i].Name;
			uint32 SectorId = atoi(Ar.Exports[i].Name + PrefixLen);
			//Sectors[SectorId] = (UTerrainSector*)Ar.SerializeObject(Ar.Exports[i]);
			Sectors.add((UTerrainSector*)Ar.SerializeObject(Ar.Exports[i]));
			//Sectors[SectorId]->m_TerrainInfo = this;
		}
	}

	for(uint32 i = 0; i < Sectors.Size(); i++)
	//for(uint32 i = 0; i < 20; i++)
	{
		Sectors[i]->Init(); // i % 16, i / 16
	}
	
	return Ar;
}

void UTerrainInfo::finishLoad()
{
	if(_UIsInitialized)
		return;

	const char *verstr = (const char *) glGetString(GL_VERSION);
	const char *glslverstr = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

	if(!g_shader.GetByName("L2Terrain"))
	{
		char *terrain_vert_shader;
		char *terrain_frag_shader;

		uint32 t, fend;

		FILE *sh_file = fopen("data/shaders/terrain.vert", "rb");
		t = ftell(sh_file);
		fseek(sh_file, 0, SEEK_END);
		fend = ftell(sh_file);
		fseek(sh_file, 0, SEEK_SET);
		terrain_vert_shader = new char[fend - t + 1];
		fread(terrain_vert_shader, fend - t, 1, sh_file);
		terrain_vert_shader[fend - t] = 0;
		fclose(sh_file);

		sh_file = fopen("data/shaders/terrain.frag", "rb");
		t = ftell(sh_file);
		fseek(sh_file, 0, SEEK_END);
		fend = ftell(sh_file);
		fseek(sh_file, 0, SEEK_SET);
		terrain_frag_shader = new char[fend - t + 1];
		fread(terrain_frag_shader, fend - t, 1, sh_file);
		terrain_frag_shader[fend - t] = 0;
		fclose(sh_file);

		g_shader.Add(terrain_vert_shader, terrain_frag_shader, "L2Terrain");
	}

	for(int i = 0; i < Layers.Size(); i++)
	{
		if(Layers[i]->Texture)
			Layers[i]->Texture->finishLoad();
		if(Layers[i]->AlphaMap)
			Layers[i]->AlphaMap->finishLoad();
	}

	glGenTextures(1, &AlphaMap1);

	uint8 *tmp_tex;

	tmp_tex = new uint8[1024 * 1024 * 4 * sizeof(uint8)];
	memset(tmp_tex, 0, 1024 * 1024 * 4 * sizeof(uint8));
	
	bool hasAlpha[12];
	memset(hasAlpha, 0, 12);
	for(int i = 0; i < Layers.Size(); i++)
	{
		if(Layers[i]->AlphaMap) //  && Layers[i]->AlphaMap->Palette.Colors.Size() == 0
			hasAlpha[i] = true;
	}

	uint8 *tmp_alpha_tex0;
	uint8 *tmp_alpha_tex1;
	uint8 *tmp_alpha_tex2;
	uint8 *tmp_alpha_tex3;

	if(hasAlpha[0])
	{
		Layers[0]->AlphaMap->StartPass();
		tmp_alpha_tex0 = new uint8[Layers[0]->AlphaMap->Width * Layers[0]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex0);
		Layers[0]->AlphaMap->EndPass();
	}

	if(hasAlpha[1])
	{
		Layers[1]->AlphaMap->StartPass();
		tmp_alpha_tex1 = new uint8[Layers[1]->AlphaMap->Width * Layers[1]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex1);
		Layers[1]->AlphaMap->EndPass();
	}

	if(hasAlpha[2])
	{
		Layers[2]->AlphaMap->StartPass();
		tmp_alpha_tex2 = new uint8[Layers[2]->AlphaMap->Width * Layers[2]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex2);
		Layers[2]->AlphaMap->EndPass();
	}

	if(hasAlpha[3])
	{
		Layers[3]->AlphaMap->StartPass();
		tmp_alpha_tex3 = new uint8[Layers[3]->AlphaMap->Width * Layers[3]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex3);
		Layers[3]->AlphaMap->EndPass();
	}

	/*UTexture *tt8 = Layers[8]->AlphaMap;
	UTexture *tt9 = Layers[9]->AlphaMap;
	UTexture *tt10 = Layers[10]->AlphaMap;
	UTexture *tt11 = Layers[11]->AlphaMap;*/

	for(int iy = 0; iy < 1024; iy++)
	{
		for(int ix = 0; ix < 1024; ix++)
		{
			uint8 R, G, B, A;

			// B, G, R, A o_O
			if(hasAlpha[0])
				R = tmp_alpha_tex0[((int)(iy / 1024. * Layers[0]->AlphaMap->Height) * Layers[0]->AlphaMap->Width + (int)(ix / 1024. * Layers[0]->AlphaMap->Width)) * 4 + 0];
			else
				R = 0;

			if(hasAlpha[1])
				G = tmp_alpha_tex1[((int)(iy / 1024. * Layers[1]->AlphaMap->Height) * Layers[1]->AlphaMap->Width + (int)(ix / 1024. * Layers[1]->AlphaMap->Width)) * 4 + 0];
			else
				G = 0;

			if(hasAlpha[2])
				B = tmp_alpha_tex2[((int)(iy / 1024. * Layers[2]->AlphaMap->Height) * Layers[2]->AlphaMap->Width + (int)(ix / 1024. * Layers[2]->AlphaMap->Width)) * 4 + 0];
			else
				B = 0;

			if(hasAlpha[3])
				A = tmp_alpha_tex3[((int)(iy / 1024. * Layers[3]->AlphaMap->Height) * Layers[3]->AlphaMap->Width + (int)(ix / 1024. * Layers[3]->AlphaMap->Width)) * 4 + 0];
			else
				A = 0;
			
			tmp_tex[(iy * 1024 + ix) * 4 + 0] = R;
			tmp_tex[(iy * 1024 + ix) * 4 + 1] = G;
			tmp_tex[(iy * 1024 + ix) * 4 + 2] = B;
			tmp_tex[(iy * 1024 + ix) * 4 + 3] = A;
		}
	}

	if(hasAlpha[0])
		delete tmp_alpha_tex0;
	if(hasAlpha[1])
		delete tmp_alpha_tex1;
	if(hasAlpha[2])
		delete tmp_alpha_tex2;
	if(hasAlpha[3])
		delete tmp_alpha_tex3;

	glBindTexture(GL_TEXTURE_2D, AlphaMap1);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // GL_NEAREST // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	///////////////////////////////////////////
	glGenTextures(1, &AlphaMap2);

	uint8 *tmp_alpha_tex4;
	uint8 *tmp_alpha_tex5;
	uint8 *tmp_alpha_tex6;
	uint8 *tmp_alpha_tex7;

	if(hasAlpha[4])
	{
		Layers[4]->AlphaMap->StartPass();
		tmp_alpha_tex4 = new uint8[Layers[4]->AlphaMap->Width * Layers[4]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex4);
		Layers[4]->AlphaMap->EndPass();
	}

	if(hasAlpha[5])
	{
		Layers[5]->AlphaMap->StartPass();
		tmp_alpha_tex5 = new uint8[Layers[5]->AlphaMap->Width * Layers[5]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex5);
		Layers[5]->AlphaMap->EndPass();
	}

	if(hasAlpha[6])
	{
		Layers[6]->AlphaMap->StartPass();
		tmp_alpha_tex6 = new uint8[Layers[6]->AlphaMap->Width * Layers[6]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex6);
		Layers[6]->AlphaMap->EndPass();
	}

	if(hasAlpha[7])
	{
		Layers[7]->AlphaMap->StartPass();
		tmp_alpha_tex7 = new uint8[Layers[7]->AlphaMap->Width * Layers[7]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex7);
		Layers[7]->AlphaMap->EndPass();
	}

	/*UTexture *tt4 = Layers[4]->AlphaMap;
	UTexture *tt5 = Layers[5]->AlphaMap;
	UTexture *tt6 = Layers[6]->AlphaMap;
	UTexture *tt7 = Layers[7]->AlphaMap;*/

	for(int iy = 0; iy < 1024; iy++)
	{
		for(int ix = 0; ix < 1024; ix++)
		{
			uint8 R, G, B, A;

			// B, G, R, A o_O
			if(hasAlpha[4])
				R = tmp_alpha_tex4[((int)(iy / 1024. * Layers[4]->AlphaMap->Height) * Layers[4]->AlphaMap->Width + (int)(ix / 1024. * Layers[4]->AlphaMap->Width)) * 4 + 0];
			else
				R = 0;

			if(hasAlpha[5])
				G = tmp_alpha_tex5[((int)(iy / 1024. * Layers[5]->AlphaMap->Height) * Layers[5]->AlphaMap->Width + (int)(ix / 1024. * Layers[5]->AlphaMap->Width)) * 4 + 0];
			else
				G = 0;

			if(hasAlpha[6])
				B = tmp_alpha_tex6[((int)(iy / 1024. * Layers[6]->AlphaMap->Height) * Layers[6]->AlphaMap->Width + (int)(ix / 1024. * Layers[6]->AlphaMap->Width)) * 4 + 0];
			else
				B = 0;

			if(hasAlpha[7])
				A = tmp_alpha_tex7[((int)(iy / 1024. * Layers[7]->AlphaMap->Height) * Layers[7]->AlphaMap->Width + (int)(ix / 1024. * Layers[7]->AlphaMap->Width)) * 4 + 0];
			else
				A = 0;
			
			tmp_tex[(iy * 1024 + ix) * 4 + 0] = R;
			tmp_tex[(iy * 1024 + ix) * 4 + 1] = G;
			tmp_tex[(iy * 1024 + ix) * 4 + 2] = B;
			tmp_tex[(iy * 1024 + ix) * 4 + 3] = A;
		}
	}

	if(hasAlpha[4])
		delete tmp_alpha_tex4;
	if(hasAlpha[5])
		delete tmp_alpha_tex5;
	if(hasAlpha[6])
		delete tmp_alpha_tex6;
	if(hasAlpha[7])
		delete tmp_alpha_tex7;

	glBindTexture(GL_TEXTURE_2D, AlphaMap2);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // GL_NEAREST // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	///////////////////////////////////////////
	glGenTextures(1, &AlphaMap3);

	uint8 *tmp_alpha_tex8;
	uint8 *tmp_alpha_tex9;
	uint8 *tmp_alpha_tex10;
	uint8 *tmp_alpha_tex11;

	if(hasAlpha[8])
	{
		Layers[8]->AlphaMap->StartPass();
		tmp_alpha_tex8 = new uint8[Layers[8]->AlphaMap->Width * Layers[8]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex8);
		Layers[8]->AlphaMap->EndPass();
	}

	if(hasAlpha[9])
	{
		Layers[9]->AlphaMap->StartPass();
		tmp_alpha_tex9 = new uint8[Layers[9]->AlphaMap->Width * Layers[9]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex9);
		Layers[9]->AlphaMap->EndPass();
	}

	if(hasAlpha[10])
	{
		Layers[10]->AlphaMap->StartPass();
		tmp_alpha_tex10 = new uint8[Layers[10]->AlphaMap->Width * Layers[10]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex10);
		Layers[10]->AlphaMap->EndPass();
	}

	if(hasAlpha[11])
	{
		Layers[11]->AlphaMap->StartPass();
		tmp_alpha_tex11 = new uint8[Layers[11]->AlphaMap->Width * Layers[11]->AlphaMap->Height * 4 * sizeof(uint8)];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_alpha_tex11);
		Layers[11]->AlphaMap->EndPass();
	}

	/*UTexture *tt8 = Layers[8]->AlphaMap;
	UTexture *tt9 = Layers[9]->AlphaMap;
	UTexture *tt10 = Layers[10]->AlphaMap;
	UTexture *tt11 = Layers[11]->AlphaMap;*/

	for(int iy = 0; iy < 1024; iy++)
	{
		for(int ix = 0; ix < 1024; ix++)
		{
			uint8 R, G, B, A;

			// B, G, R, A o_O
			if(hasAlpha[8])
				R = tmp_alpha_tex8[((int)(iy / 1024. * Layers[8]->AlphaMap->Height) * Layers[8]->AlphaMap->Width + (int)(ix / 1024. * Layers[8]->AlphaMap->Width)) * 4 + 0];
			else
				R = 0;

			if(hasAlpha[9])
				G = tmp_alpha_tex9[((int)(iy / 1024. * Layers[9]->AlphaMap->Height) * Layers[9]->AlphaMap->Width + (int)(ix / 1024. * Layers[9]->AlphaMap->Width)) * 4 + 0];
			else
				G = 0;

			if(hasAlpha[10])
				B = tmp_alpha_tex10[((int)(iy / 1024. * Layers[10]->AlphaMap->Height) * Layers[10]->AlphaMap->Width + (int)(ix / 1024. * Layers[10]->AlphaMap->Width)) * 4 + 0];
			else
				B = 0;

			if(hasAlpha[11])
				A = tmp_alpha_tex11[((int)(iy / 1024. * Layers[11]->AlphaMap->Height) * Layers[11]->AlphaMap->Width + (int)(ix / 1024. * Layers[11]->AlphaMap->Width)) * 4 + 0];
			else
				A = 0;
			
			tmp_tex[(iy * 1024 + ix) * 4 + 0] = R;
			tmp_tex[(iy * 1024 + ix) * 4 + 1] = G;
			tmp_tex[(iy * 1024 + ix) * 4 + 2] = B;
			tmp_tex[(iy * 1024 + ix) * 4 + 3] = A;
		}
	}

	if(hasAlpha[8])
		delete tmp_alpha_tex8;
	if(hasAlpha[9])
		delete tmp_alpha_tex9;
	if(hasAlpha[10])
		delete tmp_alpha_tex10;
	if(hasAlpha[11])
		delete tmp_alpha_tex11;

	glBindTexture(GL_TEXTURE_2D, AlphaMap3);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // GL_NEAREST // GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	delete tmp_tex;

	for(uint32 i = 0; i < Sectors.Size(); i++)
	{
		Sectors[i]->finishLoad();
	}

	_UIsInitialized = true;
}
