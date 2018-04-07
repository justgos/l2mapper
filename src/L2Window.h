#pragma once

#include "_afx.h"

class L2Window
{
public:
	bool create();

	void onResize();

	HWND getHWND() { return g_hwnd; }
	HDC getDC() { return g_dc; }

	bool isInitialized() { return _isInitialized; }
	void setIsInitialized(bool value) { _isInitialized = value; }
	bool isActive() { return _isActive; }
	void setIsActive(bool value) { _isActive = value; }

	_vector2 getInnerSize();
protected:
	void _create();
	bool _destroy();
protected:
	HWND g_hwnd;
	HINSTANCE g_hinst;
	HGLRC g_glrc;
	HDC g_dc;

	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;

	bool _isInitialized;
	bool _isActive;
	
	bool arbMultisampleSupported;
	int	 arbMultisampleFormat;
public:
	L2Window(void);
	~L2Window(void);
};

