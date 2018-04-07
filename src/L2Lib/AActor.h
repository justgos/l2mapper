#pragma once

#include "UMath.h"
#include "UObject.h"
#include "UProperty.h"
#include "L2LibVarsHolder.h"

class ULevel;

class DLLEXPORT AActor : public UObject
{
public:
	UVector m_Location;
	URotator m_Rotation;
	UVector m_DrawScale3D;
	float m_DrawScale;
	class UObject *m_LevelInfo;
	class UPointRegion *m_Region;
	float camDist;
	ULevel *_level;
	L2LibVarsHolder *L2LibVars;
public:
	UVector GetLocation() { return m_Location; }
	URotator GetRotation() { return m_Rotation; }
	UVector GetDrawScale() { return m_DrawScale; }
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();
	virtual void setLevel(ULevel *level) { _level = level; }
	virtual ULevel *getLevel() { return _level; }
	virtual void setL2LibVars(L2LibVarsHolder *vars) { L2LibVars = vars; }

	virtual void Tick();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(AActor);
public:
	AActor(void);
	~AActor(void);
};

