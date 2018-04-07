#include "UBase.h"
#include "UPackage.h"

#include "UShader.h"

#include "L2LibVarsHolder.h"
#include "UPackageManager.h"

UShader::UShader(void)
	: UMaterial()
{
	Diffuse = 0;
	Opacity = 0;
}

UShader::~UShader(void)
{
}

bool UShader::SetVariable(const class UProperty& Prop)
{
	if(UMaterial::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Diffuse") == 0)
	{
		Diffuse = *(UMaterial**)Prop.Data;
		/*if(UTIL_strcmpi(Diffuse->BaseClass, "Texture") != 0
			&& UTIL_strcmpi(Diffuse->BaseClass, "Shader") != 0
			&& UTIL_strcmpi(Diffuse->BaseClass, "Combiner") != 0
			&& UTIL_strcmpi(Diffuse->BaseClass, "TexModifier") != 0)
		{
			Diffuse = 0;
		}*/
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Opacity") == 0)
	{
		Opacity = *(UMaterial**)Prop.Data;
		return true;
	}

	return false;
}

void UShader::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;
}

void UShader::StartPass()
{
	if(Diffuse)
	{
		if(!Opacity)
			glDisable(GL_ALPHA_TEST);
		Diffuse->StartPass();
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, L2LibVars->dummyTex);
	}
}

void UShader::EndPass()
{
	if(Diffuse)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		if(!Opacity)
			glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

UPackage& UShader::Serialize(UPackage& Ar)
{
	UMaterial::Serialize(Ar);
	
	return Ar;
}

void UShader::setL2LibVars(L2LibVarsHolder *vars)
{
	L2LibVars = vars;

	if(Diffuse)
	{
		Diffuse->setL2LibVars(vars);
	}
}

void UShader::finishLoad()
{
	if(Diffuse)
	{
		Diffuse->finishLoad();
	}

	if(Opacity)
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
	}
}
