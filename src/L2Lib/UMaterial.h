#pragma once

#include "UMath.h"
#include "UObject.h"
#include "UProperty.h"
#include "L2LibVarsHolder.h"

#define GET_COLOR_DWORD(color) (*(uint32*)&(color))
class DLLEXPORT UColor
{
public:
	int8 R,G,B,A;

	UColor(void) { }
	UColor(int8 InR, int8 InG, int8 InB)
	:	R(InR), G(InG), B(InB), A(255) { }
	UColor(int8 InR, int8 InG, int8 InB, int8 InA)
	:	R(InR), G(InG), B(InB), A(InA) { }

	// Operators.
	bool operator==( const UColor &C ) const
	{
		//return D==C.D;
		return *(DWORD*)this == GET_COLOR_DWORD(C);
	}

	bool operator!=( const UColor& C ) const
	{
		return GET_COLOR_DWORD(*this)!=GET_COLOR_DWORD(C);
	}

	int32 Brightness( void ) const
	{
		return (2 * (int32)R + 3 *(int32)G + 1 * (int32)B) >> 3;
	}

	float FBrightness( void ) const
	{
		return (2.f * R + 3.f * G + 1.f * B) / (6.f * 256.f);
	}

	uint32 TrueColor( void ) const
	{
		uint32 D = *(uint32*)this;
		return ((D&0xff) << 16) + (D & 0xff00) + ((D & 0xff0000) >> 16);
	}
	int16 HiColor565( void ) const
	{
		DWORD D = GET_COLOR_DWORD(*this);
		return ((D & 0xf8) << 8) + ((D & 0xfC00) >> 5) + ((D & 0xf80000) >> 19);
	}
	int16 HiColor555( void ) const
	{
		uint32 D=GET_COLOR_DWORD(*this);
		return ((D & 0xf8) << 7) + ((D & 0xf800) >> 6) + ((D & 0xf80000) >> 19);
	}

	UPackage& Serialize(UPackage& Ar)
	{
		if(Ar.Ver( ) > UPACKAGE_UNREAL2_VERSION) // I am confused....
			return Ar >> B >> G >> R >> A;
		else
			return Ar >> R >> G >> B >> A;
	}
	IMPLEMENT_USERIALIZER( UColor );
};

class DLLEXPORT UPalette : public UObject
{
public:
	jfArray2<UColor, uint32> Colors;
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UPalette);
public:
	UPalette();
};

class DLLEXPORT UMaterial : public UObject
{
public:
	L2LibVarsHolder *L2LibVars;
public:
	virtual bool SetVariable(const class UProperty& Prop);

	virtual void StartPass();
	virtual void EndPass();

	virtual void setL2LibVars(L2LibVarsHolder *vars) { L2LibVars = vars; }
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UMaterial);
public:
	UMaterial(void);
};

class DLLEXPORT UMaterialContainer : public UObject
{
public:
	UMaterial *m_Material;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	UMaterial *GetMaterial() { return m_Material; }

	virtual void StartPass();
	virtual void EndPass();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UMaterialContainer);
public:
	UMaterialContainer(void);
};
