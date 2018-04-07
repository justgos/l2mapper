#pragma once

#include "UMath.h"
#include "AActor.h"
#include "UModel.h"
#include "UMaterial.h"
#include "UProperty.h"

enum ECsgOper
{
    CSG_Active              = 0,
    CSG_Add                 = 1,
    CSG_Subtract            = 2,
    CSG_Intersect           = 3,
    CSG_Deintersect         = 4,
    CSG_MAX                 = 5,
};

class DLLEXPORT UBrush : public AActor
{
public:
	UModel *m_Model;
	UVector m_MainScale;
	UVector m_PrePivot;
	int32 m_PolyFlags;
	int8 m_CsgOper;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	UModel *GetModel() { return m_Model; }
	UVector GetMainScale() { return m_MainScale; }

	virtual void Tick();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UBrush);
public:
	UBrush(void);
	~UBrush(void);
};

