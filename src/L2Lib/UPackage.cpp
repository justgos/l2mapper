#include "UPackage.h"

#include "UProperty.h"
#include "UMath.h"
#include "UObject.h"
#include "UMaterial.h"
#include "UTexture.h"
#include "UStaticMesh.h"
#include "UStaticMeshActor.h"
#include "AActor.h"
#include "UPrimitive.h"
#include "UObj.h"
#include "UModel.h"
#include "UBrush.h"
#include "UTerrain.h"
#include "ULevel.h"
#include "UShader.h"
#include "UCombiner.h"
#include "UTexModifier.h"
#include "UFinalBlend.h"
#include "UZoneInfo.h"

#include "UPackageManager.h"

UPackage& UIndex::Serialize(class UPackage& Ar)
{
	int32 Index;
	int8 B;
	uint32 Data, Shift;
	bool Neg = false;

	Ar >> B;

	Neg = B & (1 << 7);
	Index = B & 0x3f;

	if(B & (1 << 6))
	{
		Shift = 6;

		do
		{
			Ar >> B;

			Data = B & 0x7f;	// bits 1 - 7 spichern die Daten
			Data <<= Shift;
			Index |= Data;
			Shift += 7;
		}
		while((B & (1 << 7)) && (Shift < 32));
	}

	if(Neg)
		Index = -Index;

	int32 *Ptr = &Reference;
	*Ptr = Index;

	return Ar;
}

UPackage& UString::Serialize(UPackage& Ar)
{
	int32 Len;
	Ar >> UIndex(Len);
	//CHECK( abs( Len ) >= (1<<16) );
	if(Len < 0)
	{
		// new 127+ versions seems to store wide-character strings with negative length
		Len = -Len;
		Txt = new char[Len];
		int32 Pos = 0;
		uint32 Char; // but i dont like wchars!
		while(Pos < Len)
		{
			Ar >> Char;
			Txt[Pos++] = Char;
		}
	}
	else if(Len > 0)
	{
		Txt = new char[Len];
		Ar.Read(Txt, Len);
	}
	return Ar;
}

UPackage::UPackage(void)
{
	m_FileAddr = 0;
	IsEncrypted = false;
	m_IsLoaded = false;
	CryptKey = 0;
	ContentOffset = 0;
}

UPackage::~UPackage(void)
{
}

void UPackage::SetFile(char *fileName)
{
	m_Name = UTIL_CopyString(fileName);
	UTIL_StripExtension(m_Name);
	m_Name = UTIL_FileName(m_Name);
	m_FileAddr = UTIL_CopyString(fileName);
}

void UPackage::Load()
{
	if(!m_FileAddr)
		return;

	Open(m_FileAddr);
}

void UPackage::Open(char *fileName)
{
	m_Name = UTIL_CopyString(fileName);
	UTIL_StripExtension(m_Name);
	m_Name = UTIL_FileName(m_Name);
	m_FileAddr = UTIL_CopyString(fileName);

	_f = fopen(fileName, "rb");

	int32 Signature;

	*((UPackage*)this) >> Signature;

	if(Signature == 0x0069004C)
	{
		// lineage encrypted
		Seek(24);
		*((UPackage*)this) >> CryptKey;
		CryptKey = 0xC1 ^ CryptKey;
		IsEncrypted = true;
		ContentOffset = 28;
		//Seek(-1, U_SEEK_CUR);
		Seek(0, U_SEEK_SET);
		*((UPackage*)this) >> Signature;
	}

	if(Signature != 0x9E2A83C1)
	{
		fclose(_f);
		printf("Wrong file format\r\n");
		return;
	}

	*((UPackage*)this) >> m_Header.Version;
	m_Header.Version = LOWORD(m_Header.Version);

	*((UPackage*)this) >> m_Header.Flags;

	*((UPackage*)this) >> m_Header.NameCount;
	*((UPackage*)this) >> m_Header.NameOffset;

	*((UPackage*)this) >> m_Header.ExportCount;
	*((UPackage*)this) >> m_Header.ExportOffset;

	*((UPackage*)this) >> m_Header.ImportCount;
	*((UPackage*)this) >> m_Header.ImportOffset;

	if(m_Header.Version < 68)
	{
		*((UPackage*)this) >> m_Header.HeritageCount;
		*((UPackage*)this) >> m_Header.HeritageOffset;
	}
	else
	{
		Read(m_Header.GUID, 16);

		uint32 GenerationCount;
		*((UPackage*)this) >> GenerationCount;

		m_Header.Generations.alloc(GenerationCount);

		for(uint32 i = 0; i < m_Header.Generations.Size(); i++)
		{
			UGeneration Gen;
			
			*((UPackage*)this) >> Gen.ExportCount;
			*((UPackage*)this) >> Gen.NameCount;

			m_Header.Generations.add(Gen);
		}
	}

	Seek(m_Header.NameOffset, U_SEEK_SET);

	Names.alloc(m_Header.NameCount);

	for(uint32 i = 0; i < m_Header.NameCount; i++)
	{
		UName Name;
		
		*((UPackage*)this) >> &Name.Name;
		*((UPackage*)this) >> Name.Flags;
		Names[i] = Name;
	}

	Seek(m_Header.ExportOffset, U_SEEK_SET);

	Exports.alloc(m_Header.ExportCount);

	int32 Index;

	for(uint32 i = 0; i < m_Header.ExportCount; i++)
	{
		UExport Export;
		
		*((UPackage*)this) >> UIndex(Export.iClass);
		*((UPackage*)this) >> UIndex(Export.iSuper);
		*((UPackage*)this) >> Export.Package;
		*((UPackage*)this) >> UIndex(Index);
		Export.Name = Names[Index].Name;
		*((UPackage*)this) >> Export.Flags;
		*((UPackage*)this) >> UIndex(Export.Size);
		if(Export.Size > 0)
		{
			*((UPackage*)this) >> UIndex(Export.Offset);
		}
		Exports[i] = Export;
	}

	Seek(m_Header.ImportOffset, U_SEEK_SET);

	Imports.alloc(m_Header.ImportCount);

	for(uint32 i = 0; i < m_Header.ImportCount; i++)
	{
		UImport Import;
		
		*((UPackage*)this) >> UIndex(Import.ClassPackage);
		*((UPackage*)this) >> UIndex(Import.ClassName);
		*((UPackage*)this) >> Import.Package;
		*((UPackage*)this) >> UIndex(Index);
		Import.Name = Names[Index].Name;

		Imports[i] = Import;
	}

	for(uint32 i = 0; i < m_Header.ExportCount; i++)
	{
		UExport t = Exports[i];
		while(t.iSuper > 0)
		{
			t = Exports[t.iSuper];
		}
		Exports[i].BaseClass = IndexToName(t.iClass);
		UExport Export = Exports[i];

		if(UTIL_strcmpi(Name(), "FX_E_T") == 0)
		{
			int g = Export.Flags;
		}
	}

	for(uint32 i = 0; i < m_Header.ImportCount; i++)
	{
		UImport t = Imports[i];
		while(t.Package < 0)
		{
			t = Imports[-t.Package - 1];
		}
		Imports[i].BasePackage = t.Name;
	}

	fclose(_f);

	m_IsLoaded = true;
}

void UPackage::Read(void *dest, uint32 len)
{
	fread(dest, len, 1, _f);

	if(IsEncrypted)
	{
		for(uint32 i = 0; i < len; i++)
		{
			((int8*)dest)[i] ^= CryptKey;
		}
	}

	return;
}

uint32 UPackage::Tell()
{
	return ftell(_f) - ContentOffset;
}

void UPackage::Seek(uint32 offset, int8 origin)
{
	if(origin == U_SEEK_SET)
		fseek(_f, offset + ContentOffset, origin);
	else
		fseek(_f, offset, origin);
}

char *UPackage::IndexToName(int32 idx)
{
	if(idx < 0)
		return Imports[-idx - 1].Name;
	else
		return Exports[idx - 1].Name;
}

UObject *UPackage::GetUObject(char *name)
{
	for(uint32 i = 0; i < Exports.Size(); i++)
	{
		char *nm = Exports[i].Name;
		if(UTIL_strcmpi(Exports[i].Name, name) == 0 && UTIL_strcmpi(Exports[i].BaseClass, "Package") != 0)
		{
			UObject *Result = 0;

			_f = fopen(m_FileAddr, "rb");

			if(!_f)
			{
				printf("ERROR: Cannot open file \"%s\"\n", m_FileAddr);
			}

			//Seek(Exports[i].Offset, U_SEEK_SET);
			
			//TestDump(Exports[i].Size, false);

			/*Seek(52);
			int32 ind;
			*((UPackage*)this) >> UIndex(ind);
			printf("index: %d\n", ind);*/

			uint32 t = Tell();
			//printf("Size: %d, Offset: %d\n", Exports[i].Size, Exports[i].Offset);
			/*for(uint32 u = 0; u < Exports[i].Size; u++)
			{
				Seek(t + u, U_SEEK_SET);
				//float x, y, z;
				int16 x, y, z;
				*((UPackage*)this) >> x >> y >> z;
				//printf("%d, %f, %f, %f\n", u, x, y, z);
				printf("%d, %d, %d, %d\n", u, x, y, z);
			}*/
			/*for(uint32 u = 0; u < Exports[i].Size; u++)
			{
				Seek(t + u, U_SEEK_SET);
				int32 Index;
				*((UPackage*)this) >> UIndex(Index);
				//if(Index < 0 && -Index - 1 < Imports.Size())
				//	printf("Import: %d, %s (%d)\n", u, Imports[-Index - 1].Name, Index);
				//if(Index > 0 && Index - 1 < Exports.Size())
				//	printf("Export: %d, %s (%d)\n", u, Exports[Index - 1].Name, Index);
				if(Index >= 0 && Index < Names.Size())
					printf("Name: %d, %s (%d, 0x%02X)\n", u, Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0));
			}*/
			
			//Seek(Exports[i].Offset, U_SEEK_SET);

			if(Exports[i].iClass != 0)
			{
				//Result = SerializeObject(IndexToName(Exports[i].iClass));
				Result = SerializeObject(Exports[i]);
			}
			else
			{
				printf("ololo :(\n");
			}

			fclose(_f);

			return Result;
		}
	}

	return 0;
}

UPackage& UPackage::operator >> (class UObject*& Res)
{
	int32 Index;

	*((UPackage*)this) >> UIndex(Index);

	if(Index == 0)
	{
		Res = 0;
		return *this;
	}
	
	if(Index < 0)
	{
		char buf[256];
		//sprintf(buf, "%s.%s",  Ar.Imports[- Ar.Imports[-Index - 1].Package - 1].Name, Ar.Imports[-Index - 1].Name);
		sprintf(buf, "%s.%s",  Imports[-Index - 1].BasePackage, Imports[-Index - 1].Name);
		//printf("Import Obj: %s, %s\n", buf, Names[Imports[-Index - 1].ClassName].Name);
		/*if(UTIL_strcmpi(Imports[-Index - 1].Name, "SkybackgroundColor") == 0)
		{
			int gg = 0;
		}*/
		Res = PkgMgr->GetUObject(buf);
	}
	else
	{
		//printf("Export Obj: %s\n", Exports[Index - 1].Name);

		UExport texp = Exports[Index - 1];
		
		//printf("\n==Before\n");
		//Ar.TestDump(100, true);
		UExport e = Exports[Index - 1];
		/*if(UTIL_strcmpi(Exports[Index - 1].Name, "SkybackgroundColor") == 0)
		{
			int gg = 0;
		}*/
		Res = SerializeObject(Exports[Index - 1]);
		/*if(Res != 0)
		{
			char buf[256];
			sprintf(buf, "%s.%s",  Name(), Exports[Index - 1].Name);
			Res->ObjectName = UTIL_CopyString(buf);
		}*/
		//printf("\n==After\n");
		//Ar.TestDump(100, true);
	}

	return *this;
}

void UPackage::TestDump(uint32 Len, bool ToScreen)
{
	uint32 Pos = Tell();

	char *tbuf = new char[1000 * 1024];
	Read(tbuf, Len);
	char buf[256];

	if(ToScreen)
	{
		for(uint32 u = 0; u < Len; u++)
		{
			printf("%02X", (uint8)tbuf[u]);
		}
		printf("\n");
	}
	else
	{
		FILE *fl = fopen("log.txt", "w");
		for(uint32 u = 0; u < Len; u++)
		{
			sprintf(buf, "%02X", (uint8)tbuf[u]);
			fputs(buf, fl);
		}
		fputs("\r\n", fl);
		fclose(fl);
	}

	delete [] tbuf;

	Seek(Pos, U_SEEK_SET);
}

#define PROCESS_NEW_EXPORT \
			Result->ObjectName = UTIL_CopyString(NameBuf); \
			ObjExport.ObjectHandle = Result;

UObject *UPackage::SerializeObject(UExport &ObjExport)
{
	if(ObjExport.ObjectHandle)
		return ObjExport.ObjectHandle;

	uint32 TPos;

	TPos = Tell();

	Seek(ObjExport.Offset, U_SEEK_SET);

	UObject *Result = 0;

	char NameBuf[256];
	sprintf(NameBuf, "%s.%s",  Name(), ObjExport.Name);

	if(UTIL_strcmpi(NameBuf, "anghel_fall_pan") == 0)
	{
		int g = 0;
	}

	int32 Index;

	if(UTIL_strcmpi(ObjExport.BaseClass, "StaticMesh") == 0)
	{
		Result = new UStaticMesh();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UStaticMesh*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Material") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UMaterial*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "MaterialContainer") == 0)
	{
		Result = new UMaterialContainer();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UMaterialContainer*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Texture") == 0)
	{
		Result = new UTexture();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexture*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "StaticMeshActor") == 0)
	{
		Seek(15);
		Result = new UStaticMeshActor();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UStaticMeshActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "MovableStaticMeshActor") == 0)
	{
		Seek(15);
		Result = new UStaticMeshActor();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UStaticMeshActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "L2MovableStaticMeshActor") == 0)
	{
		Seek(15);
		Result = new UStaticMeshActor();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UStaticMeshActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "CustomizableStaticMeshActor") == 0)
	{
		Seek(15);
		Result = new UStaticMeshActor();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UStaticMeshActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "StaticMeshInstance") == 0)
	{
		Seek(12);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "AmbientSoundObject") == 0)
	{
		//Seek(12);
		Result = new AActor();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((AActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "PlayerStart") == 0)
	{
		Seek(15);
		Result = new AActor();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((AActor*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TerrainSector") == 0)
	{
		Result = new UTerrainSector();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTerrainSector*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "PhysicsVolume") == 0 || UTIL_strcmpi(ObjExport.BaseClass, "DefaultPhysicsVolume") == 0)
	{
		Seek(17);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		// while trying to load SoI
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "BlockingVolume") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "ServerBlockingVolume") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "WaterVolume") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "MusicVolume") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		// while trying to load SoI
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "AmbientVolume") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TerrainInfo") == 0)
	{
		Seek(17);
		Result = new UTerrainInfo();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTerrainInfo*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "LevelInfo") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "ZoneInfo") == 0)
	{
		//Seek(17);
		Seek(15);
		Result = new UZoneInfo();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UZoneInfo*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "SkyZoneInfo") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "L2FogInfo") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "L2SeamlessInfo") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "FluidSurfaceInfo") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Level") == 0)
	{
		Result = new ULevel();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((ULevel*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Brush") == 0)
	{
		Seek(15);
		Result = new UBrush();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UBrush*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Camera") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Light") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "NMovableSunLight") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "NSun") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "NMoon") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Mover") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "LineagePlayerController") == 0)
	{
		Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Emitter") == 0)
	{
		//Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Projector") == 0)
	{
		//Seek(15);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	/*else if(UTIL_strcmpi(ObjExport.BaseClass, "LevelSummary") == 0)
	{
		Seek(12);
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}*/
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Polys") == 0)
	{
		Result = new UPolys();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UPolys*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Model") == 0)
	{
		Result = new UModel();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UModel*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Shader") == 0)
	{
		Result = new UShader();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UShader*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Combiner") == 0)
	{
		Result = new UCombiner();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UCombiner*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexPanner") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexPannerTriggered") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexOscillator") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexOscillatorTriggered") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexRotator") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexScaler") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "FinalBlend") == 0)
	{
		Result = new UFinalBlend();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UFinalBlend*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "ColorModifier") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "WetTexture") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "FadeColor") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexCoordSource") == 0)
	{
		Result = new UTexModifier();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UTexModifier*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "TexEnvMap") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Cubemap") == 0)
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Palette") == 0)
	{
		Result = new UPalette();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UPalette*)Result);
	}
	else if(UTIL_strcmpi(ObjExport.BaseClass, "Package") == 0) // o_O
	{
		Result = new UMaterial();
		PROCESS_NEW_EXPORT
		//*((UPackage*)this) >> *((UObject*)Result);
	}
	else
	{
		Result = new UObject();
		PROCESS_NEW_EXPORT
		*((UPackage*)this) >> *((UObject*)Result);
	}

	Result->BaseClass = ObjExport.BaseClass;

	Seek(TPos, U_SEEK_SET);

	return Result;
}
