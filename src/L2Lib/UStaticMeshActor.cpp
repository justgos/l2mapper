#include "UBase.h"
#include "UPackage.h"
#include "UPackageManager.h"
#include "UObj.h"
#include "math.h"

#include "UStaticMeshActor.h"

#include "ULevel.h"

UStaticMeshActor::UStaticMeshActor(void)
	: AActor()
{
	m_Mesh = 0;
	_isBackfacing = false;
}

UStaticMeshActor::~UStaticMeshActor(void)
{
}

bool UStaticMeshActor::SetVariable(const class UProperty& Prop)
{
	if(AActor::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "StaticMesh") == 0 || UTIL_strcmpi(Prop.Name, "Mesh") == 0)
	{
		//if(Prop.DataLength > 0)
		for(uint32 i = 0; i <= Prop.DataLength - sizeof(UStaticMesh*); i += sizeof(UStaticMesh*))
		{
			m_Mesh = dynamic_cast<UStaticMesh*>(*(UStaticMesh**)(Prop.Data + i));

			if(m_Mesh)
				return true;
		}
	}

	return false;
}

void UStaticMeshActor::setL2LibVars(L2LibVarsHolder *vars)
{
	L2LibVars = vars;

	if(m_Mesh && m_Mesh->Surfs.Size() < 500 && m_Mesh->Materials.Size() < 500)
	{
		for(uint32 surf = 0; surf < m_Mesh->Surfs.Size(); surf++)
		{
			UMaterialContainer *tc = m_Mesh->Materials[surf];
			if(m_Mesh->Materials[surf]->GetMaterial())
			{
				m_Mesh->Materials[surf]->GetMaterial()->setL2LibVars(vars);
			}
		}
	}
}

void UStaticMeshActor::Tick()
{
	if(!m_Mesh)
		return;

	if(m_Mesh->Surfs.Size() > 500 || m_Mesh->Materials.Size() > 500)
		return;

	_vector3 aPos = _vector3(m_Location.X, m_Location.Y, m_Location.Z);

	if((aPos - getLevel()->_camPos).Length() > getLevel()->_visibleDist)
		return;

	/*if(!bAlphaBlend)
	{
		glDisable(GL_ALPHA_TEST);
	}*/

	if(_isBackfacing)
		glFrontFace(GL_CW);

	glPushMatrix();
		UVector loc = m_Location;
		//if(m_Region)
		//	loc.Y -= m_Region->Zone->m_Location.Y / m_Region->Zone->m_DrawScale;

		/*UMatrix mat, tmat;
		mat.SetIdentity();
		//if((m_DrawScale.X != 1 && m_DrawScale.X != -1) || (m_DrawScale.Y != 1 && m_DrawScale.Y != -1) || (m_DrawScale.Z != 1 && m_DrawScale.Z != -1))
		{
			int gg = 0;
		}
		tmat = FScaleMatrix(m_DrawScale3D * m_DrawScale);
		mat *= tmat;
		tmat = FRotationMatrix(m_Rotation);
		mat *= tmat;
		tmat = FTranslationMatrix(loc);
		mat *= tmat;*/

		_matrix mat, tmat;
		mat.Identity();
		tmat.Identity();
		tmat.ScalingMatrix(m_DrawScale3D.X * m_DrawScale, m_DrawScale3D.Y * m_DrawScale, m_DrawScale3D.Z * m_DrawScale);
		mat *= tmat;
		tmat.Identity();
		tmat.rotationZ(-m_Rotation.Yaw / 32768. * _PI);
		mat *= tmat;
		tmat.Identity();
		tmat.rotationX(2 * _PI - m_Rotation.Roll / 32768. * _PI);
		mat *= tmat;
		tmat.Identity();
		tmat.rotationY(2 * _PI - m_Rotation.Pitch / 32768. * _PI);
		mat *= tmat;
		g_shader.SetUniformMatrix4fv(L2LibVars->defaultObjectShader, "normalMat", mat.data);
		tmat.Identity();
		tmat.TranslationMatrix(loc.X, loc.Y, loc.Z);
		mat *= tmat;

		glMultMatrixf(mat.data);
		//glMultMatrixf(mat.Data);

		/*if(m_Mesh->Surfs.Size() > 500 || m_Mesh->Materials.Size() > 500)
		{
			_vector3 bboxMin = _vector3(0, 0, 0);
			_vector3 bboxMax = _vector3(1000, 1000, 1000);

			_vector3 bnl = (bboxMin);
			_vector3 tfr = (bboxMax);
			_vector3 bnr = (_vector3(bboxMax.x, bboxMin.y, bboxMin.z));
			_vector3 bfr = (_vector3(bboxMax.x, bboxMin.y, bboxMax.z));
			_vector3 bfl = (_vector3(bboxMin.x, bboxMin.y, bboxMax.z));
			_vector3 tnr = (_vector3(bboxMax.x, bboxMax.y, bboxMin.z));
			_vector3 tnl = (_vector3(bboxMin.x, bboxMax.y, bboxMin.z));
			_vector3 tfl = (_vector3(bboxMin.x, bboxMax.y, bboxMax.z));

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			//glDisable(GL_DEPTH_TEST);

			glBegin(GL_LINES);
				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(bnr.x, bnr.y, bnr.z);

				glVertex3f(bnr.x, bnr.y, bnr.z);
				glVertex3f(bfr.x, bfr.y, bfr.z);

				glVertex3f(bfr.x, bfr.y, bfr.z);
				glVertex3f(bfl.x, bfl.y, bfl.z);

				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(bfl.x, bfl.y, bfl.z);

				glVertex3f(tnl.x, tnl.y, tnl.z);
				glVertex3f(tnr.x, tnr.y, tnr.z);

				glVertex3f(tnr.x, tnr.y, tnr.z);
				glVertex3f(tfr.x, tfr.y, tfr.z);

				glVertex3f(tfr.x, tfr.y, tfr.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);

				glVertex3f(tnl.x, tnl.y, tnl.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);

				glVertex3f(bnl.x, bnl.y, bnl.z);
				glVertex3f(tnl.x, tnl.y, tnl.z);

				glVertex3f(bnr.x, bnr.y, bnr.z);
				glVertex3f(tnr.x, tnr.y, tnr.z);

				glVertex3f(bfr.x, bfr.y, bfr.z);
				glVertex3f(tfr.x, tfr.y, tfr.z);

				glVertex3f(bfl.x, bfl.y, bfl.z);
				glVertex3f(tfl.x, tfl.y, tfl.z);
			glEnd();

			//glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
		}
		else*/
		{
			for(uint32 surf = 0; surf < m_Mesh->Surfs.Size(); surf++)
			{
				UMaterialContainer *tc = m_Mesh->Materials[surf];
				if(m_Mesh->Materials[surf]->GetMaterial())
					m_Mesh->Materials[surf]->GetMaterial()->StartPass();

				glCallList(m_Mesh->m_RenderLists + surf);

				if(m_Mesh->Materials[surf]->GetMaterial())
					m_Mesh->Materials[surf]->GetMaterial()->EndPass();
			}
		}
	glPopMatrix();

	/*if(!bAlphaBlend)
	{
		glEnable(GL_ALPHA_TEST);
	}*/

	if(_isBackfacing)
		glFrontFace(GL_CCW);
}

UPackage& UStaticMeshActor::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	//

	return Ar;
}

void UStaticMeshActor::finishLoad()
{
	if(m_Mesh && !(m_Mesh->Surfs.Size() > 500 || m_Mesh->Materials.Size() > 500))
	{
		m_Mesh->finishLoad();
	}

	_isBackfacing = m_DrawScale3D.X * m_DrawScale3D.Y * m_DrawScale3D.Z < 0;

	_UIsInitialized = true;
}
