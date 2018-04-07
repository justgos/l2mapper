#pragma once

#include "../_afx.h"

class L2Time
{
public:
	void update();

	uint16 getTicks() { return _ticks; }
	uint16 getTicksDelta() { return _ticksDelta; }
protected:
	uint16 _ticks;
	uint16 _ticksPrev;
	uint16 _ticksDelta;
public:
	L2Time(void);
	~L2Time(void);
};

