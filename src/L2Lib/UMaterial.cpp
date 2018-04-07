#include "UBase.h"
#include "UPackage.h"

#include "UMaterial.h"

UPalette::UPalette(void)
	: UObject()
{
}

UPackage& UPalette::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	Ar >> Colors;
	UColor g = Colors[255];
	UColor g1 = Colors[0];
	UColor g2 = Colors[1];

	return Ar;
}

UMaterial::UMaterial(void)
	: UObject()
{
}

bool UMaterial::SetVariable(const class UProperty& Prop)
{
	if(UObject::SetVariable(Prop))
		return true;

	return false;
}

UPackage& UMaterial::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	//

	return Ar;
}

void UMaterial::StartPass()
{
}

void UMaterial::EndPass()
{
}

UMaterialContainer::UMaterialContainer(void)
{
	m_Material = 0;
}

bool UMaterialContainer::SetVariable(const class UProperty& Prop)
{
	if(UTIL_strcmpi(Prop.Name, "Material") == 0)
	{
		if(Prop.DataLength > 0)
		{
			m_Material = dynamic_cast<UMaterial*>(*(UMaterial**)Prop.Data);
			return true;
		}
	}

	return false;
}

UPackage& UMaterialContainer::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	//

	return Ar;
}

void UMaterialContainer::StartPass()
{
}

void UMaterialContainer::EndPass()
{
}
