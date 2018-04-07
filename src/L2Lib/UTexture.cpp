#include "UBase.h"
#include "UPackage.h"

#include "UTexture.h"

UTexture::UTexture(void)
	: UMaterial()
{
	Width = 0;
	Height = 0;
	Format = 0x05;

	Data = 0;

	UClamp = 0;
	VClamp = 0;

	IsPaletted = false;

	m_Handle = 0;
	
	bAlphaTexture = false;
	bMasked = false;

	L2LibVars = 0;

	tmp_tex = 0;
}

bool UTexture::SetVariable(const class UProperty& Prop)
{
	if(UMaterial::SetVariable(Prop))
		return true;

	if(UTIL_strcmpi(Prop.Name, "Format") == 0)
	{
		Format = *(uint8*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "USize") == 0)
	{
		Width = *(uint32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "VSize") == 0)
	{
		Height = *(uint32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "UClamp") == 0)
	{
		UClamp = *(uint32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "VClamp") == 0)
	{
		VClamp = *(uint32*)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "Palette") == 0)
	{
		IsPaletted = true;
		Palette = **(UPalette**)Prop.Data;
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "bAlphaTexture") == 0)
	{
		bAlphaTexture = *((int8*)Prop.Data);
		return true;
	}
	else if(UTIL_strcmpi(Prop.Name, "bMasked") == 0)
	{
		bMasked = *((int8*)Prop.Data);
		return true;
	}

	return false;
}

void UTexture::OnLoaded()
{
	if(Loaded)
		return;

	Loaded = true;

	if(Data)
		delete [] Data;
}

void UTexture::StartPass()
{
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	if(L2LibVars)
		g_shader.SetUniform1i(L2LibVars->defaultObjectShader, "tex", 0);
}

void UTexture::EndPass()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

static int Unpack565(uint8 *packed, uint8 *colour)
{
	// build the packed value
	int value = (int)packed[0] | ((int)packed[1] << 8);
	
	// get the components in the stored range
	uint8 red = (uint8)((value >> 11) & 0x1f);
	uint8 green = (uint8)((value >> 5) & 0x3f);
	uint8 blue = (uint8)(value & 0x1f);

	// scale up to 8 bits
	colour[0] = (red << 3) | (red >> 2);
	colour[1] = (green << 2) | (green >> 4);
	colour[2] = (blue << 3) | (blue >> 2);
	colour[3] = 255;
	
	// return the value
	return value;
}

void UTexture::DecompressColour(uint8 *rgba, void *block, bool isDxt1)
{
	// get the block bytes
	uint8 *bytes = reinterpret_cast<uint8*>(block);
	
	// unpack the endpoints
	uint8 codes[16];
	int a = Unpack565(bytes, codes);
	int b = Unpack565(bytes + 2, codes + 4);
	
	// generate the midpoints
	for( int i = 0; i < 3; ++i )
	{
		int c = codes[i];
		int d = codes[4 + i];

		if( isDxt1 && a <= b )
		{
			codes[8 + i] = (uint8)((c + d) / 2);
			codes[12 + i] = 0;
		}
		else
		{
			codes[8 + i] = (uint8)((2*c + d) / 3);
			codes[12 + i] = (uint8)((c + 2*d) / 3);
		}
	}
	
	// fill in alpha for the intermediate values
	codes[8 + 3] = 255;
	codes[12 + 3] = (isDxt1 && a <= b) ? 0 : 255;
	
	// unpack the indices
	uint8 indices[16];
	for( int i = 0; i < 4; ++i )
	{
		uint8* ind = indices + 4*i;
		uint8 packed = bytes[4 + i];
		
		ind[0] = packed & 0x3;
		ind[1] = (packed >> 2) & 0x3;
		ind[2] = (packed >> 4) & 0x3;
		ind[3] = (packed >> 6) & 0x3;
	}

	// store out the colours
	for(int i = 0; i < 16; ++i)
	{
		uint8 offset = 4 * indices[i];
		for(int j = 0; j < 4; ++j)
			rgba[4 * i + j] = codes[offset + j];
	}
}

void UTexture::DecompressAlphaDxt3(uint8* rgba, void *block)
{
	uint8 * bytes = reinterpret_cast< uint8 * >( block );
	
	// unpack the alpha values pairwise
	for( int i = 0; i < 8; ++i )
	{
		// quantise down to 4 bits
		uint8 quant = bytes[i];
		
		// unpack the values
		uint8 lo = quant & 0x0f;
		uint8 hi = quant & 0xf0;

		// convert back up to bytes
		rgba[8*i + 3] = lo | ( lo << 4 );
		rgba[8*i + 7] = hi | ( hi >> 4 );
	}
}

void UTexture::DecompressAlphaDxt5(uint8* rgba, void *block)
{
	// get the two alpha values
	uint8 *bytes = reinterpret_cast<uint8*>(block);
	int alpha0 = bytes[0];
	int alpha1 = bytes[1];
	
	// compare the values to build the codebook
	uint8 codes[8];
	codes[0] = (uint8)alpha0;
	codes[1] = (uint8)alpha1;
	if( alpha0 <= alpha1 )
	{
		// use 5-alpha codebook
		for( int i = 1; i < 5; ++i )
			codes[1 + i] = (uint8)( ( ( 5 - i )*alpha0 + i*alpha1 )/5 );
		codes[6] = 0;
		codes[7] = 255;
	}
	else
	{
		// use 7-alpha codebook
		for( int i = 1; i < 7; ++i )
			codes[1 + i] = (uint8)( ( ( 7 - i )*alpha0 + i*alpha1 )/7 );
	}
	
	// decode the indices
	uint8 indices[16];
	uint8 *src = bytes + 2;
	uint8* dest = indices;
	for( int i = 0; i < 2; ++i )
	{
		// grab 3 bytes
		int value = 0;
		for( int j = 0; j < 3; ++j )
		{
			int byte = *src++;
			value |= ( byte << 8*j );
		}
		
		// unpack 8 3-bit values from it
		for( int j = 0; j < 8; ++j )
		{
			int index = ( value >> 3*j ) & 0x7;
			*dest++ = ( uint8 )index;
		}
	}
	
	// write out the indexed codebook values
	for( int i = 0; i < 16; ++i )
		rgba[4*i + 3] = codes[indices[i]];
}

void UTexture::DecompressDXTBlock(uint8 *rgba, void *block, int flags)
{
	// fix any bad flags
	//flags = FixFlags( flags );

	// get the block locations
	void *colourBlock = block;
	void *alphaBock = block;
	if((flags & ( kDxt3 | kDxt5)) != 0)
		colourBlock = reinterpret_cast<uint8*>(block) + 8;

	// decompress colour
	DecompressColour(rgba, colourBlock, (flags & kDxt1) != 0);

	// decompress alpha separately if necessary
	if( ( flags & kDxt3 ) != 0 )
		DecompressAlphaDxt3( rgba, alphaBock );
	else if( ( flags & kDxt5 ) != 0 )
		DecompressAlphaDxt5( rgba, alphaBock );
}

void UTexture::DecodeDXT(int8 *src, int flags)
{
	uint8 targetRgba[16 * 4];

	int bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;

	uint8 *sourceBlock = (uint8*)src;

	for(uint16 y = 0; y < Height; y += 4)
	{
		for(uint16 x = 0; x < Width; x += 4)
		{
			DecompressDXTBlock((uint8*)&targetRgba, sourceBlock, flags);

			// write the decompressed pixels to the correct image locations
			uint8 *sourcePixel = targetRgba;

			// exchange R & B
			for(uint16 i = 0; i < 64 ; i += 4)
			{
				int8 t = sourcePixel[i];
				sourcePixel[i] = sourcePixel[i + 2];
				sourcePixel[i + 2] = t;
			}

			for( int py = 0; py < 4; ++py )
			{
				for( int px = 0; px < 4; ++px )
				{
					// get the target location
					int sx = x + px;
					int sy = y + py;
					if(sx < Width && sy < Height)
					{
						uint8 *targetPixel = (uint8*)Data + 4 * (Width * sy + sx);
						
						// copy the rgba value
						for(int i = 0; i < 4; ++i)
							*targetPixel++ = *sourcePixel++;
					}
					else
					{
						// skip this pixel as its outside the image
						sourcePixel += 4;
					}
				}
			}

			sourceBlock += bytesPerBlock;
		}
	}
}

UPackage& UTexture::Serialize(UPackage& Ar)
{
	UObject::Serialize(Ar);

	int32 Index;

	//Ar.TestDump(100, true);
	//printf("\n\n");

	Ar.Seek(37);

	char *tstr;
	Ar >> &tstr; // texture's name
	
	Ar.Seek(1);
	Ar >> UIndex(Index);
	Ar.Seek(Index);

	//printf("==\n");
	//Ar.TestDump(20, true);

	/*uint32 t = Ar.Tell();
	for(uint32 u = 0; u < 200000; u++)
	{
		Ar.Seek(t + u, U_SEEK_SET);
		//float x, y, z;
		int32 x;
		Ar >> UIndex(x);
		//printf("%d, %f, %f, %f\n", u, x, y, z);
		if(x >= 512 * 512 && x <= 2048 * 2048)
			printf("%d, %d\n", u, x);
	}*/

	Ar.Seek(9);

	uint8 PixelSize = 4;

	//printf("\n");
	//Ar.TestDump(20, true);
	//printf("\n==\n");

	int32 BlockSize;
	Ar >> UIndex(BlockSize);

	if(Format == 0x0A)
	{
		if(BlockSize != Width * Height * 2)
		{
			uint32 t = Ar.Tell();
			for(uint32 i = 0; i < 200000; i++)
			{
				Ar.Seek(t + i, U_SEEK_SET);
				Ar >> UIndex(BlockSize);
				if(BlockSize == Width * Height * 2)
					break;
			}
		}
	}
	/*if(Format == 0x08)
	{
		if(BlockSize != Width * Height * 2)
		{
			uint32 t = Ar.Tell();
			for(uint32 i = 0; i < 200000; i++)
			{
				Ar.Seek(t + i, U_SEEK_SET);
				Ar >> UIndex(BlockSize);
				if(BlockSize == Width * Height * 2)
					break;
			}
		}
	}*/
	if(Format == 0x03)
	{
		if(BlockSize <= 0)
		{
			uint32 t = Ar.Tell();
			for(uint32 i = 0; i < 20000; i++)
			{
				Ar.Seek(t + i, U_SEEK_SET);
				Ar >> UIndex(BlockSize);
				if(BlockSize >= Width * Height / 2 && BlockSize <= Width * Height / 2 + 20)
					break;
			}
		}
	}

	if(Width <= 0 || Height <= 0)
	{
		//printf("\n\n================= TEXTURE HAS ZERO WIDTH! (%s) =================\n\n", tstr);
		return Ar;
	}
	if(BlockSize <= 0)
	{
		//printf("\n\n================= TEXTURE HAS ZERO BLOCK SIZE! (%s) =================\n\n", tstr);
		return Ar;
	}
	if(Width > 2048 ||  Height > 2048)
	{
		//printf("\n\n================= TEXTURE IS TOO LARGE! (%s, %d, %d) =================\n\n", tstr, Width, Height);
		return Ar;
	}
	
	int8 *Tmp = new int8[BlockSize];
	Ar.Read(Tmp, BlockSize);

	if(Format == 0x03) // DXT1
	{
		Data = new int8[Width * Height * PixelSize];
		DecodeDXT(Tmp, kDxt1);
	}
	else if(Format == 0x05) // RGBA8
	{
		if(BlockSize == Width * Height) // B&W
		{
			PixelSize = 1;
		}

		Data = new int8[Width * Height * PixelSize];
		memcpy(Data, Tmp, BlockSize);
		//Ar.Read(Data, Width * Height * PixelSize);
	}
	else if(Format == 0x06) // DXT3
	{
		Data = new int8[Width * Height * PixelSize];
		DecodeDXT(Tmp, kDxt3);
	}
	else if(Format == 0x07) // DXT5
	{
		Data = new int8[Width * Height * PixelSize];
		DecodeDXT(Tmp, kDxt5);
	}
	else if(Format == 0x08) // UYVY
	{
		printf("Video textures (UYVY) are not supported currently :(\n");

		if(BlockSize == Width * Height) // B&W
		{
			PixelSize = 1;
		}
		else
		{
			int gg = 0;
		}

		/*Data = new int8[Width * Height * PixelSize];
		memcpy(Data, Tmp, BlockSize);*/
	}
	/*else if(Format == 0x08) // L8
	{
		printf("..is there really sense in loading L8 textures?)\n");

		if(BlockSize == Width * Height) // B&W
		{
			PixelSize = 1;
		}

		Data = new int8[Width * Height * PixelSize];
		memcpy(Data, Tmp, BlockSize);
	}*/
	else if(Format == 0x0A) // heightmap
	{
		Data = new int8[BlockSize];
		memcpy(Data, Tmp, BlockSize);
		/*uint16 color = _tData[(iy * _tWidth + ix)] / 65536. * 255;
		R = color;
		G = color;
		B = color;
		A = 255;*/
	}
	else
	{
		printf("\n\n================= UKNOWN TEXTURE FORMAT! (%d) =================\n\n", Format);
	}
	
	delete [] Tmp;
		
	if(Format == 8)
	{
		//m_Handle = 0;
		return Ar;
	}

	if(!Data)
	{
		return Ar;
	}

	if(Format == 0x0A) // heightmap
	{
		//glGenTextures(1, &m_Handle);

		//uint8 *tmp_tex;

		//tmp_tex = new uint8[Width * Height * 4 * sizeof(uint8)];
		//memset(tmp_tex, 0, Width * Height * 4 * sizeof(uint8));

		//for(int iy = 0; iy < Height; iy++)
		//{
		//	for(int ix = 0; ix < Width; ix++)
		//	{
		//		uint8 R, G, B, A;
		//	
		//		// B, G, R, A o_O
		//		B = Data[(iy * Width + ix)] / 65535.;
		//		G = Data[(iy * Width + ix)] / 65535.;
		//		R = Data[(iy * Width + ix)] / 65535.;
		//		A = Data[(iy * Width + ix)] / 65535.;
		//	
		//		tmp_tex[(iy * Width + ix) * 4 + 0] = R;
		//		tmp_tex[(iy * Width + ix) * 4 + 1] = G;
		//		tmp_tex[(iy * Width + ix) * 4 + 2] = B;
		//		tmp_tex[(iy * Width + ix) * 4 + 3] = A;
		//	}
		//}

		//glBindTexture(GL_TEXTURE_2D, m_Handle);
		//glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // GL_NEAREST // GL_LINEAR
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		//delete [] tmp_tex;

		m_Handle = 0;

		return Ar;
	}

	// DISABLE_TEXTURES
	/*delete Data;
	m_Handle = 0;
	return Ar;*/
	
	tmp_tex = new uint8[Width * Height * 4 * sizeof(uint8)];
	memset(tmp_tex, 0, Width * Height * 4 * sizeof(uint8));

	for(int iy = 0; iy < Height; iy++)
	{
		for(int ix = 0; ix < Width; ix++)
		{
			uint8 R, G, B, A;
			
			// B, G, R, A o_O
			if(PixelSize == 1) // B&W
			{
				if(IsPaletted)
				{
					B = Palette.Colors[(uint8)Data[(iy * Width + ix)]].B;
					G = Palette.Colors[(uint8)Data[(iy * Width + ix)]].G;
					R = Palette.Colors[(uint8)Data[(iy * Width + ix)]].R;
					A = Palette.Colors[(uint8)Data[(iy * Width + ix)]].A;
				}
				else
				{
					B = Data[(iy * Width + ix)];
					G = Data[(iy * Width + ix)];
					R = Data[(iy * Width + ix)];
					A = 255;
				}
			}
			else
			{
				B = Data[(iy * Width + ix) * 4 + 0];
				G = Data[(iy * Width + ix) * 4 + 1];
				R = Data[(iy * Width + ix) * 4 + 2];
				A = Data[(iy * Width + ix) * 4 + 3];
			}
			
			tmp_tex[(iy * Width + ix) * 4 + 0] = R;
			tmp_tex[(iy * Width + ix) * 4 + 1] = G;
			tmp_tex[(iy * Width + ix) * 4 + 2] = B;
			tmp_tex[(iy * Width + ix) * 4 + 3] = A;
		}
	}

	return Ar;
}

void UTexture::finishLoad()
{
	if(_UIsInitialized)
		return;
	
	if(tmp_tex)
	{
		glGenTextures(1, &m_Handle);

		glBindTexture(GL_TEXTURE_2D, m_Handle);
		/*glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // GL_NEAREST // GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);*/

		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);*/

		delete [] tmp_tex;

		OnLoaded();
	}

	_UIsInitialized = true;
}
