#include "UBase.h"
#include "UPackage.h"

#include "UZoneInfo.h"

UZoneInfo::UZoneInfo(void)
{
}

UZoneInfo::~UZoneInfo(void)
{
}

bool UZoneInfo::SetVariable(const UProperty& Prop)
{
	if(AActor::SetVariable(Prop))
		return true;

	return false;
}

void UZoneInfo::Tick()
{
}

UPackage& UZoneInfo::Serialize(UPackage& Ar)
{
	AActor::Serialize(Ar);

	return Ar;
}
