#pragma once

#include "UMath.h"
#include "UTexModifier.h"
#include "UProperty.h"

class DLLEXPORT UFinalBlend : public UTexModifier
{
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void StartPass();
	virtual void EndPass();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UFinalBlend);
public:
	UFinalBlend(void);
	~UFinalBlend(void);
};

