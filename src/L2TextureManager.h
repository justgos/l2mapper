#pragma once

#include "_afx.h"

class L2TextureManager
{
public:
	GLuint get(char *fileName);
	GLuint get(char *fileName, bool clampToEdge);
public:
	L2TextureManager(void);
	~L2TextureManager(void);
};

