#include "UBase.h"
#include "UPackage.h"

#include "AActor.h"

#include "ULevel.h"
#include "UZoneInfo.h"
#include "UObj.h"

AActor::AActor(void)
	: UObject()
{
	m_Location = UVector(0, 0, 0);
	m_Rotation = URotator(0, 0, 0);
	m_DrawScale3D = UVector(1, 1, 1);
	m_DrawScale = 1;
	m_LevelInfo = 0;
}

AActor::~AActor(void)
{
}

bool AActor::SetVariable(const class UProperty& Prop)
{
	if(UTIL_strcmpi(Prop.Name, "Location") == 0)
	{
		m_Location = (*(UVector*)Prop.Data);
		float t = m_Location.Z;
		m_Location.Z = m_Location.Y;
		m_Location.Y = t;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Rotation") == 0)
	{
		m_Rotation = (*(URotator*)Prop.Data);
		int32 t = m_Rotation.Pitch;
		m_Rotation.Pitch = m_Rotation.Yaw;
		m_Rotation.Yaw = t;

		m_Rotation.Roll = 65535 - m_Rotation.Roll;
		//m_Rotation.Yaw = 65535 - m_Rotation.Yaw;

		/*t = m_Rotation.Roll;
		m_Rotation.Roll = m_Rotation.Yaw;
		m_Rotation.Yaw = t;*/
		//m_Rotation.Pitch = -m_Rotation.Pitch;
		//m_Rotation.Yaw = -m_Rotation.Yaw;
		//m_Rotation.Roll = -m_Rotation.Roll;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "DrawScale3D") == 0)
	{
		m_DrawScale3D = (*(UVector*)Prop.Data);
		float t = m_DrawScale3D.Z;
		m_DrawScale3D.Z = m_DrawScale3D.Y;
		m_DrawScale3D.Y = t;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "DrawScale") == 0)
	{
		m_DrawScale = (*(float*)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "LevelInfo") == 0)
	{
		m_LevelInfo = (*(UObject**)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Region") == 0)
	{
		m_Region = (*(UPointRegion**)Prop.Data);
		return true;
	}

	return false;
}

void AActor::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;

	//if(Data)
	//	delete [] Data;
}

void AActor::Tick()
{
}

UPackage& AActor::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	return Ar;
}
