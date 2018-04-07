#pragma once

#include "_afx.h"

class config
{
public:
	bool showDialog();
public:
	bool CFG_FULLSCREEN;
	int CFG_SCREENRES_X;
	int CFG_SCREENRES_Y;
	int CFG_SCREENBITS;
	int CFG_ANTIALIASING;
public:
	config(void);
	~config(void);
};
