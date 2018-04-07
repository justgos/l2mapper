#pragma once

#include "UMath.h"
#include "UMaterial.h"
#include "UProperty.h"

enum ETexMapAxis
{
	TEXMAPAXIS_XY,
	TEXMAPAXIS_XZ,
	TEXMAPAXIS_YZ,
};

enum
{
	//! Use DXT1 compression.
	kDxt1 = ( 1 << 0 ), 
	
	//! Use DXT3 compression.
	kDxt3 = ( 1 << 1 ), 
	
	//! Use DXT5 compression.
	kDxt5 = ( 1 << 2 ), 
};

class DLLEXPORT UTexture : public UMaterial
{
public:
	int32 MipMapCount;
	int8 *Data;

	uint8 Format;
	uint32 Width;
	uint32 Height;

	uint32 UClamp;
	uint32 VClamp;

	bool IsPaletted;
	UPalette Palette;

	bool bAlphaTexture;
	bool bMasked;

	GLuint m_Handle;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded();

	virtual void StartPass();
	virtual void EndPass();

	virtual void finishLoad();
private:
	void DecompressColour(uint8 *rgba, void *block, bool isDxt1);
	void DecompressAlphaDxt3(uint8* rgba, void *block);
	void DecompressAlphaDxt5(uint8* rgba, void *block);
	void DecompressDXTBlock(uint8 *rgba, void *block, int flags);
	void DecodeDXT(int8 *src, int flags);
protected:
	uint8 *tmp_tex;
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UTexture);
public:
	UTexture(void);
};

