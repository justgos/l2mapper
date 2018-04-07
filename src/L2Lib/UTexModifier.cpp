#include "UBase.h"
#include "UPackage.h"

#include "UTexModifier.h"

#include "L2LibVarsHolder.h"
#include "UPackageManager.h"

UTexModifier::UTexModifier(void)
	: UMaterial()
{
	Material = 0;
}

UTexModifier::~UTexModifier(void)
{
}

bool UTexModifier::SetVariable(const class UProperty& Prop)
{
	if(UMaterial::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Material") == 0)
	{
		Material = *(UMaterial**)Prop.Data;
		/*if(UTIL_strcmpi(Material->BaseClass, "Texture") != 0
			&& UTIL_strcmpi(Material->BaseClass, "Shader") != 0
			&& UTIL_strcmpi(Material->BaseClass, "Combiner") != 0
			&& UTIL_strcmpi(Material->BaseClass, "TexCoordSource") != 0)
		{
			Material = 0;
		}*/
		return true;
	}

	return false;
}

void UTexModifier::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;
}

void UTexModifier::StartPass()
{
	if(Material)
	{
		Material->StartPass();
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, L2LibVars->dummyTex);
	}
}

void UTexModifier::EndPass()
{
	if(Material)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

UPackage& UTexModifier::Serialize(UPackage& Ar)
{
	UMaterial::Serialize(Ar);
	
	return Ar;
}

void UTexModifier::setL2LibVars(L2LibVarsHolder *vars)
{
	L2LibVars = vars;

	if(Material)
	{
		Material->setL2LibVars(vars);
	}
}

void UTexModifier::finishLoad()
{
	if(Material)
	{
		Material->finishLoad();
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
