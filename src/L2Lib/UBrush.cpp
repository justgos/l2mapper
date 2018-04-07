#include "UBase.h"
#include "UPackage.h"
#include "UPackageManager.h"

#include "UBrush.h"

UBrush::UBrush(void)
	: AActor()
{
	m_Model = 0;
	m_MainScale = UVector(1, 1, 1);
	m_PrePivot = UVector(0, 0, 0);
	m_PolyFlags = 0;
	m_CsgOper = -1;
}

UBrush::~UBrush(void)
{
}

bool UBrush::SetVariable(const class UProperty& Prop)
{
	if(AActor::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Brush") == 0)
	{
		for(uint32 i = 0; i <= Prop.DataLength - sizeof(UModel*); i += sizeof(UModel*))
		{
			m_Model = dynamic_cast<UModel*>(*(UModel**)(Prop.Data + i));
			if(m_Model)
				return true;
		}
	}
	else if(UTIL_strcmpi(Prop.Name, "MainScale") == 0)
	{
		m_MainScale = (*(UVector*)Prop.Data);
		float t = m_MainScale.Z;
		m_MainScale.Z = m_MainScale.Y;
		m_MainScale.Y = t;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "PrePivot") == 0)
	{
		m_PrePivot = (*(UVector*)Prop.Data);
		float t = m_PrePivot.Z;
		m_PrePivot.Z = m_PrePivot.Y;
		m_PrePivot.Y = t;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "PolyFlags") == 0)
	{
		m_PolyFlags = (*(int32*)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "CsgOper") == 0)
	{
		m_CsgOper = (*(int8*)Prop.Data);
		return true;
	}

	return false;
}

void UBrush::Tick()
{
	if(!(m_PolyFlags & EPolyFlags::PF_Invisible || m_PolyFlags & EPolyFlags::PF_NotSolid || m_PolyFlags & EPolyFlags::PF_Semisolid)) // || (m_CsgOper == 2 && m_PolyFlags & EPolyFlags::PF_Semisolid) //  || m_PolyFlags & EPolyFlags::PF_Semisolid
	{
		if(m_Model && m_Model->Polys)
		{
			glPushMatrix();
			UMatrix mat, tmat;
			mat.SetIdentity();
			/*tmat.Identity();
			tmat.ScalingMatrix(m_DrawScale.X, m_DrawScale.Z, m_DrawScale.Y);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationZ(-m_Rotation.Roll / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationX(-m_Rotation.Pitch / 32768. * _PI);
			mat *= tmat;
			tmat.Identity();
			tmat.rotationY(-m_Rotation.Yaw / 32768. * _PI);
			mat *= tmat;*/
			/*tmat = FScaleMatrix(m_MainScale);
			mat *= tmat;*/
			tmat = FTranslationMatrix(m_Location - m_PrePivot);
			mat *= tmat;
			glMultMatrixf(mat.Data);
			for(uint32 surf = 0; surf < m_Model->Polys->Elements.Size(); surf++)
			{
				UPoly *tp = &(m_Model->Polys->Elements[surf]);
				if(m_Model->Polys->Elements[surf].Texture)
					m_Model->Polys->Elements[surf].Texture->StartPass();

				glCallList(m_Model->Polys->m_RenderLists + surf);

				if(m_Model->Polys->Elements[surf].Texture)
					m_Model->Polys->Elements[surf].Texture->EndPass();
			}
		glPopMatrix();
		}
	}
}

UPackage& UBrush::Serialize(UPackage& Ar)
{
	AActor::Serialize(Ar);

	if(m_Model)
	{
	}

	return Ar;
}
