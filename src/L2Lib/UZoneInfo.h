#pragma once

#include "UMath.h"
#include "AActor.h"
#include "UTexture.h"
#include "UProperty.h"

class UZoneInfo : public AActor
{
public:
	//
public:
	bool SetVariable(const class UProperty& Prop);
	void OnLoaded() {};

	virtual void Tick();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UZoneInfo);
public:
	UZoneInfo(void);
	~UZoneInfo(void);
};

