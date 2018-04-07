#include "L2TextureManager.h"

L2TextureManager::L2TextureManager(void)
{
}

L2TextureManager::~L2TextureManager(void)
{
}

GLuint L2TextureManager::get(char *fileName)
{
	return get(fileName, false);
}

GLuint L2TextureManager::get(char *fileName, bool clampToEdge)
{
	FIBITMAP *tbm = FreeImage_Load(FreeImage_GetFileType(fileName), fileName);
	
	BITMAPINFOHEADER *info = FreeImage_GetInfoHeader(tbm);
	
	uint8 *tmp_tex = new uint8[info->biWidth * info->biHeight * (info->biBitCount / 8)];

	RGBQUAD aPixel;

	for(uint16 iy = 0; iy < info->biHeight; iy++)
	{
		for(uint16 ix = 0; ix < info->biWidth; ix++)
		{
			FreeImage_GetPixelColor(tbm, ix, iy, &aPixel); // info->biHeight - iy
			tmp_tex[(iy * info->biWidth + ix) * (info->biBitCount / 8) + 0] = aPixel.rgbRed;
			tmp_tex[(iy * info->biWidth + ix) * (info->biBitCount / 8) + 1] = aPixel.rgbGreen;
			tmp_tex[(iy * info->biWidth + ix) * (info->biBitCount / 8) + 2] = aPixel.rgbBlue;
			if((info->biBitCount / 8) == 4)
				tmp_tex[(iy * info->biWidth + ix) * (info->biBitCount / 8) + 3] = aPixel.rgbReserved;
		}
	}

	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	if((info->biBitCount / 8) == 4)
		gluBuild2DMipmaps(GL_TEXTURE_2D, (info->biBitCount / 8), info->biWidth, info->biHeight, GL_RGBA, GL_UNSIGNED_BYTE, tmp_tex);
	else if((info->biBitCount / 8) == 3)
		gluBuild2DMipmaps(GL_TEXTURE_2D, (info->biBitCount / 8), info->biWidth, info->biHeight, GL_RGB, GL_UNSIGNED_BYTE, tmp_tex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(clampToEdge)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	delete tmp_tex;

	return tex;
}

