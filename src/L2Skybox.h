#pragma once

#include "_afx.h"

class L2Skybox
{
public:
	void Init();
	void draw();
private:
	GLuint tex[6];
	float _size;
public:
	L2Skybox(void);
	~L2Skybox(void);
};

