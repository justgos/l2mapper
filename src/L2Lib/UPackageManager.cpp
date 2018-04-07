#include "UBase.h"
#include "UPackage.h"

#include "UPackageManager.h"

#include "UObject.h"

UPackageManager::UPackageManager(void)
{
}

UPackageManager::~UPackageManager(void)
{
}

struct UnrealPath
{
	char*	Path;
	char*	Ext;
};

UnrealPath Paths[] = {
	//{ "System", "u" },
	{ "Maps", "unr" },
	{ "SysTextures", "utx" },
	{ "Textures", "utx" },
	//{ "Sounds", "uax" },
	//{ "Music", "umx" }, // it's "ogg"
	{ "StaticMeshes", "usx" },
	//{ "Animations", "ukx" },
	{ 0, 0 }
};

void UPackageManager::Init(char *BaseDir)
{
	m_BaseDir = UTIL_CopyString(BaseDir);

	Objects._size = 0;

	//Objects = new UObject*[UPKG_HASHSIZE];
	//memset(Objects, 0, UPKG_HASHSIZE * sizeof(UObject*));

	char PathStr[CM_SYSTEM_MAXNAME];
	char ExtStr[CM_SYSTEM_MAXNAME];
	char FileStr[CM_SYSTEM_MAXNAME];

	UPackage *Package;

	for(UnrealPath *Path = Paths; Path->Path; Path++)
	{
		sprintf(PathStr, "%s%s/", m_BaseDir, Path->Path);
		sprintf(ExtStr, "*.%s", Path->Ext);
		jfArray<char *, uint32> Files = *(UTIL_FindFiles(PathStr, ExtStr));

		if(Files.Size() > 0)
			Packages.realloc(Packages.Size() + Files.Size());

		for(uint32 i = 0; i < Files.Size(); i++)
		{
			
			Package = new UPackage();
			Package->PkgMgr = this;
			sprintf(FileStr, "%s%s", PathStr, Files[i]);
			Package->SetFile(FileStr);
			Packages.add(Package);
		}
	}
}

UPackage *UPackageManager::GetUPackage(char *name)
{
	for(uint32 i = 0; i < Packages.Size(); i++)
	{
		if(UTIL_strcmpi(Packages[i]->Name(), name) == 0)
		{
			if(!Packages[i]->InLoaded())
			{
				printf("Loading %s\n", Packages[i]->m_FileAddr);
				Packages[i]->Load();
			}

			return Packages[i];
		}
	}

	return 0;
}

UObject *UPackageManager::GetUObject(char *name)
{
	char *tname = UTIL_CopyString(name);
	char *ObjectAbsoluteName = UTIL_CopyString(name);
	uint32 Hash = MakeHash(tname);
	//if(Objects[Hash] != 0)
	//	return Objects[Hash];
	/*for(uint32 i = 0; i < Objects.Size(); i++)
	{
		if(Objects[i]->Hash != 0 && Objects[i]->Hash == Hash)
			return Objects[i];
	}*/
	/*for(uint32 i = 0; i < Objects.Size(); i++)
	{
		if(Objects[i]->ObjectName != 0 && UTIL_strcmpi(Objects[i]->ObjectName, ObjectAbsoluteName) == 0)
			return Objects[i];
	}*/

	char *PackageName = tname;
	char *ObjectName = tname;

	for(uint16 i = 0; i < strlen(tname); i++)
	{
		if(tname[i] == '.')
		{
			tname[i] = 0;
			ObjectName = tname + (i + 1);
			break;
		}
	}

	for(uint32 i = 0; i < Packages.Size(); i++)
	{
		UPackage *pkg = Packages[i];
		if(UTIL_strcmpi(pkg->Name(), PackageName) == 0)
		{
			if(!pkg->InLoaded())
			{
				printf("Loading %s\n", pkg->m_FileAddr);
				pkg->Load();
			}

			UObject *Result = 0;

			Result = pkg->GetUObject(ObjectName);
			
			//Objects[Hash] = Result;
			/*if(Result != 0)
			{
				Result->Hash = Hash;
				Result->ObjectName = ObjectAbsoluteName;
				Objects.add(Result);
			}*/

			return Result;
		}
	}

	return 0;
}

ULevel *UPackageManager::GetULevel(char *name)
{
	ULevel *Result = 0;

	UPackage *LevelPkg = GetUPackage(name);

	if(!LevelPkg)
		return Result;

	for(uint32 i = 0; i < LevelPkg->Exports.Size(); i++)
	{
		if(UTIL_strcmpi(LevelPkg->Exports[i].BaseClass, "Level") == 0)
		{
			Result = (ULevel*)LevelPkg->GetUObject(LevelPkg->Exports[i].Name);
			break;
		}
	}

	return Result;
}

uint32 UPackageManager::MakeHash(char *Name)
{
	uint64 Hash = 57821394;
	uint64 Len = strlen(Name);
	/*Hash ^= Len << 5;

	for(uint64 i = 0; i < Len; i++)
	{
		uint64 ch = tolower(Name[i]);
		//Hash ^= (ch << 2) ^ i << 3;
		Hash = (Hash * (ch ^ (i << 2))) % UPKG_HASHSIZE;
	}*/

	Hash = UTIL_32BitChecksum(Name, Len);

	return Hash;
}
