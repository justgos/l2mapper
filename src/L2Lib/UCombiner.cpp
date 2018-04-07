#include "UBase.h"
#include "UPackage.h"

#include "UCombiner.h"

#include "L2LibVarsHolder.h"
#include "UPackageManager.h"

UCombiner::UCombiner(void)
	: UMaterial()
{
	Material1 = 0;
	Material2 = 0;
}

UCombiner::~UCombiner(void)
{
}

bool UCombiner::SetVariable(const class UProperty& Prop)
{
	if(UMaterial::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Material1") == 0)
	{
		Material1 = *(UMaterial**)Prop.Data;
		/*if(UTIL_strcmpi(Material1->BaseClass, "Texture") != 0
			&& UTIL_strcmpi(Material1->BaseClass, "Shader") != 0
			&& UTIL_strcmpi(Material1->BaseClass, "Combiner") != 0
			&& UTIL_strcmpi(Material1->BaseClass, "TexCoordSource") != 0)
		{
			Material1 = 0;
		}*/
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Material2") == 0)
	{
		Material2 = *(UMaterial**)Prop.Data;
		/*if(UTIL_strcmpi(Material2->BaseClass, "Texture") != 0
			&& UTIL_strcmpi(Material2->BaseClass, "Shader") != 0
			&& UTIL_strcmpi(Material2->BaseClass, "Combiner") != 0
			&& UTIL_strcmpi(Material2->BaseClass, "TexCoordSource") != 0)
		{
			Material2 = 0;
		}*/
		return true;
	}

	return false;
}

void UCombiner::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;
}

void UCombiner::StartPass()
{
	if(Material2)
	{
		Material2->StartPass();
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, L2LibVars->dummyTex);
	}
}

void UCombiner::EndPass()
{
	if(Material2)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

UPackage& UCombiner::Serialize(UPackage& Ar)
{
	UMaterial::Serialize(Ar);
	
	return Ar;
}

void UCombiner::setL2LibVars(L2LibVarsHolder *vars)
{
	L2LibVars = vars;

	if(Material2)
	{
		Material2->setL2LibVars(vars);
	}
}

void UCombiner::finishLoad()
{
	if(Material2)
	{
		Material2->finishLoad();
	}

	/*if(Opacity)
	{
		Opacity->finishLoad();
	}

	if(Diffuse && Opacity)
	{
		UTexture *DiffuseT = dynamic_cast<UTexture*>(Diffuse);
		UTexture *OpacityT = dynamic_cast<UTexture*>(Opacity);

		if(DiffuseT && OpacityT && (DiffuseT->m_Handle && OpacityT->m_Handle) && (DiffuseT->Width == OpacityT->Width && DiffuseT->Height == OpacityT->Height))
		{
			Diffuse->StartPass();
			uint8 *tmp_diff_tex = new uint8[DiffuseT->Width * DiffuseT->Height * 4 * sizeof(uint8)];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_diff_tex);
			Diffuse->EndPass();

			Opacity->StartPass();
			uint8 *tmp_op_tex = new uint8[OpacityT->Width * OpacityT->Height * 4 * sizeof(uint8)];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_op_tex);
			Opacity->EndPass();

			for(uint32 i = 0; i < DiffuseT->Width * DiffuseT->Height; i++)
			{
				tmp_diff_tex[i * 4 + 3] = tmp_op_tex[i * 4 + 3];
			}

			Diffuse->StartPass();
			glTexImage2D(GL_TEXTURE_2D, 0, 4, DiffuseT->Width, DiffuseT->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_diff_tex);
			Diffuse->EndPass();

			delete tmp_diff_tex;
			delete tmp_op_tex;
		}
	}*/
}
