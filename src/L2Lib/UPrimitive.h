#pragma once

#include "UMath.h"
#include "UObject.h"
#include "UProperty.h"

class DLLEXPORT UPrimitive : public UObject
{
public:
	UBox BoundingBox;
	USphere BoundingSphere;
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UPrimitive);
public:
	UPrimitive(void) : UObject(), BoundingBox(0), BoundingSphere(0) {};
};

