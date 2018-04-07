#pragma once

#include "UBase.h"

class DLLEXPORT L2LibVarsHolder
{
public:
	GLuint dummyTex;
	uint8 defaultObjectShader;
	bool showBsp;
public:
	L2LibVarsHolder(void);
	~L2LibVarsHolder(void);
};

