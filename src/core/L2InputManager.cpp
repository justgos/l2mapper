#include "L2InputManager.h"

#include "../main.h"

L2InputManager::L2InputManager(void)
{
}

L2InputManager::~L2InputManager(void)
{
}

void L2InputManager::Init()
{
	_keyboard = new bool[256];
	memset(_keyboard, 0, 256);
}

void L2InputManager::update()
{
	POINT mousePos_Point;
	GetCursorPos(&mousePos_Point);

	_mousePosAbs.x = mousePos_Point.x;
	_mousePosAbs.y = mousePos_Point.y;

	ScreenToClient(g_window.getHWND(), &mousePos_Point);

	_mousePos.x = mousePos_Point.x;
	_mousePos.y = mousePos_Point.y;

	_mouseMove = _mousePos - _mousePosPrev;

	_mousePosPrev = _mousePos;
	_mousePosPrevAbs = _mousePosAbs;
}

void L2InputManager::setMousePos(int x, int y)
{
	SetCursorPos(x, y);
}

void L2InputManager::setCursorVisible(bool visible)
{
	ShowCursor(visible);
}

bool L2InputManager::injectMouseDown(MyGUI::MouseButton btn)
{
	if(g_ui.injectMouseDown(_mousePos.x, _mousePos.y, btn))
		return true;

	if(g_camera.injectMouseDown(btn))
		return true;

	if(g_scene.injectMouseDown(btn))
		return true;
	
	return false;
}

bool L2InputManager::injectMouseUp(MyGUI::MouseButton btn)
{
	if(g_ui.injectMouseUp(_mousePos.x, _mousePos.y, btn))
		return true;

	if(g_camera.injectMouseUp(btn))
		return true;

	if(g_scene.injectMouseUp(btn))
		return true;

	return false;
}

bool L2InputManager::injectMouseScroll(int16 scroll)
{
	_mouseScroll = scroll;

	if(g_camera.injectMouseScroll(_mouseScroll))
		return true;

	return false;
}

bool L2InputManager::injectKeyDown(uint8 key)
{
	//MyGUI::KeyCode::Enum(key);
	_keyboard[key] = true;

	return false;
}

bool L2InputManager::injectKeyUp(uint8 key)
{
	_keyboard[key] = false;

	return false;
}
