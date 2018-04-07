#include "UBase.h"
#include "UPackage.h"

#include "UPrimitive.h"

UPackage& UPrimitive::Serialize( UPackage& Ar )
{
	UObject::Serialize( Ar );

	Ar >> BoundingBox >> BoundingSphere;

	return Ar;
}
