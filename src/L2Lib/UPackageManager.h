#pragma once

#define UPackageMgr ::Singleton<UPackageManager>::get_instance()
//#define UPackageMgr UPackageManager::get_instance()

#define UPKG_HASHSIZE 18531893

class DLLEXPORT UPackageManager
{
public:
	void Init(char *BaseDir);
	UPackage *GetUPackage(char *name);
	UObject *GetUObject(char *name);
	ULevel *GetULevel(char *name);
	uint32 MakeHash(char *Name);

	/*static UPackageManager *get_instance()
	{
		if(!theInstance)
			theInstance = new UPackageManager();
		return theInstance;
	}*/
private:
	//static UPackageManager *theInstance;

	char *m_BaseDir;

	jfArray<UPackage*, uint32> Packages;
	jfArray<UObject*, uint32> Objects;
public:
	UPackageManager(void);
	~UPackageManager(void);
};

