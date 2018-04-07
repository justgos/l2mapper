#include "UBase.h"
#include "UPackage.h"

#include "UObject.h"

UObject::UObject(void)
{
	Hash = 0;
	ObjectName = 0;
	Tag = 0;
	Group = 0;
	bUseAlpha = false;
	bAlphaBlend = false;

	Loaded = false;

	_UIsInitialized = false;
}

UObject::~UObject(void)
{
	if(ObjectName)
		delete [] ObjectName;
}

bool UObject::SetVariable(const class UProperty& Prop)
{
	if(UTIL_strcmpi(Prop.Name, "Tag") == 0)
	{
		Tag = UTIL_CopyString((char*)Prop.Data);
		if(UTIL_strcmpi(Tag, "Dion_tree_S.DionRtree2"))
		{
			int g = 0;
		}
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Group") == 0)
	{
		Group = UTIL_CopyString((char*)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "bUseAlpha") == 0)
	{
		bUseAlpha = *((int8*)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "bAlphaBlend") == 0)
	{
		bAlphaBlend = *((int8*)Prop.Data);
		return true;
	}

	return false;
}

UPackage& UObject::Serialize(class UPackage& Ar)
{
	UProperty Prop;

	Ar >> Prop;

	while(UTIL_strcmpi(Prop.Name, "None") != 0)
	{
		SetVariable(Prop);
		Ar >> Prop;
	}

	return Ar;
}
