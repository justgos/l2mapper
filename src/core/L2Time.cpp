#include "L2Time.h"

L2Time::L2Time(void)
{
	_ticksPrev = GetTickCount();
}

L2Time::~L2Time(void)
{
}

void L2Time::update()
{
	_ticks = GetTickCount();
	_ticksDelta = _ticks - _ticksPrev;
	_ticksPrev = _ticks;
}
