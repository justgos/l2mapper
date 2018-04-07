#pragma once

#include "L2UIBaseWidget.h"

class L2UIBusyScreen : public L2UIBaseWidget
{
public:
	virtual void Init();

	virtual void update();

	void onResize(int width, int height);

	virtual bool injectMouseDown(int x, int y, MyGUI::MouseButton btn);
	virtual bool injectMouseUp(int x, int y, MyGUI::MouseButton btn);

	void setMessage(wchar_t *msg);
	virtual void setVisible(bool value);
	void onFadeOutFinished(MyGUI::Widget *sender);
protected:
	MyGUI::ImageBox *ui_busyScreen;
	MyGUI::Window *ui_busyScreen_msgWnd;
	MyGUI::EditBox *ui_busyScreen_msg;
public:
	L2UIBusyScreen(void);
	~L2UIBusyScreen(void);
};

