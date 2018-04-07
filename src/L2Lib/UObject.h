#pragma once

#include "UMath.h"
#include "UObject.h"
#include "UProperty.h"

class DLLEXPORT UObject
{
public:
	uint32 Hash;
	char *ObjectName;
	char *Tag;
	char *Group;
	bool bUseAlpha;
	bool bAlphaBlend;

	char *BaseClass;

	bool Loaded;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded() {};

	virtual void finishLoad() {}
protected:
	bool _UIsInitialized;
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UObject);
public:
	UObject(void);
	virtual ~UObject(void);
};

