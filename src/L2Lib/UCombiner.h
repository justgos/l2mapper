#pragma once

#include "UMath.h"
#include "UTexture.h"
#include "UProperty.h"

class DLLEXPORT UCombiner : public UMaterial
{
public:
	UMaterial *Material1;
	UMaterial *Material2;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void StartPass();
	virtual void EndPass();

	virtual void setL2LibVars(L2LibVarsHolder *vars);

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UCombiner);
public:
	UCombiner(void);
	~UCombiner(void);
};

