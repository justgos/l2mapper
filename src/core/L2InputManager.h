#pragma once

#include "../_afx.h"

class L2InputManager
{
public:
	virtual void Init();
	virtual void update();

	_vector2 getMousePos() { return _mousePos; }
	_vector2 getMousePosAbs() { return _mousePosAbs; }
	_vector2 getMousePosPrev() { return _mousePosPrev; }
	_vector2 getMousePosPrevAbs() { return _mousePosPrevAbs; }
	_vector2 getMouseMove() { return _mouseMove; }
	int16 getMouseScroll() { return _mouseScroll; }
	bool getKeyState(int8 key) { return _keyboard[key]; };

	void setMousePos(int x, int y);
	void setCursorVisible(bool visible);
	
	bool injectMouseDown(MyGUI::MouseButton btn);
	bool injectMouseUp(MyGUI::MouseButton btn);
	bool injectMouseScroll(int16 scroll);
	bool injectKeyDown(uint8 key);
	bool injectKeyUp(uint8 key);
protected:
	_vector2 _mousePos;
	_vector2 _mousePosAbs;
	_vector2 _mousePosPrev;
	_vector2 _mousePosPrevAbs;
	_vector2 _mouseMove;
	int16 _mouseScroll;

	bool *_keyboard;
public:
	L2InputManager(void);
	~L2InputManager(void);
};

