#pragma once

#ifdef _WIN32
#include "Platform_Win32.h"
#endif

#ifdef _WIN64
typedef unsigned __int64	PTRINT;		// Integer large enough to hold a pointer.
#else
typedef unsigned long		PTRINT;		// Integer large enough to hold a pointer.
#endif
