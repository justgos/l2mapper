#pragma once

#include "../_afx.h"

class L2UIBaseWidget
{
public:
	virtual void Init() { };

	virtual void update() { };

	virtual void onResize(int width, int height) { };

	virtual bool injectMouseDown(int x, int y, MyGUI::MouseButton btn) { return false; }
	virtual bool injectMouseUp(int x, int y, MyGUI::MouseButton btn) { return false; }

	virtual void setVisible(bool value) { }
public:
	L2UIBaseWidget();
	virtual ~L2UIBaseWidget();
};

