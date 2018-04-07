#pragma once

#include "UMath.h"
#include "AActor.h"
#include "UStaticMesh.h"
#include "UProperty.h"
#include "L2LibVarsHolder.h"

class DLLEXPORT UStaticMeshActor : public AActor
{
public:
	UStaticMesh *m_Mesh;
	bool _isBackfacing;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	UStaticMesh *GetMesh() { return m_Mesh; }

	virtual void setL2LibVars(L2LibVarsHolder *vars);

	virtual void Tick();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UStaticMeshActor);
public:
	UStaticMeshActor(void);
	~UStaticMeshActor(void);
};

