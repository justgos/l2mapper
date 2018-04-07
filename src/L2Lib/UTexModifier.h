#pragma once

#include "UMath.h"
#include "UTexture.h"
#include "UProperty.h"

class DLLEXPORT UTexModifier : public UMaterial
{
public:
	UMaterial *Material;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void StartPass();
	virtual void EndPass();

	virtual void setL2LibVars(L2LibVarsHolder *vars);

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UTexModifier);
public:
	UTexModifier(void);
	~UTexModifier(void);
};

