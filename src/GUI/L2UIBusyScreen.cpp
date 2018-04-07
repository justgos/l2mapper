#include "L2UIBusyScreen.h"

#include "../main.h"

L2UIBusyScreen::L2UIBusyScreen(void)
	: L2UIBaseWidget()
{
}

L2UIBusyScreen::~L2UIBusyScreen(void)
{
}

void L2UIBusyScreen::Init()
{
	ui_busyScreen = MyGUI::Gui::getInstance().createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, 40, 300, MyGUI::Align::Stretch, "L2BusyScreenLayer", "BusyScreen");
	//ui_busyScreen->setAlign(MyGUI::Align::Left | MyGUI::Align::VStretch);
	//ui_busyScreen->setProperty("ImageTexture", "textures/loading_bg.png");
	ui_busyScreen->setImageTexture("busyScreenBg.png");
	//ui_busyScreen->setImageTile(MyGUI::IntSize(40, 40));
	ui_busyScreen_msgWnd = ui_busyScreen->createWidget<MyGUI::Window>("Window", 0, 0, 400, 120, MyGUI::Align::Default, "BusyScreen_MsgWnd");
	ui_busyScreen_msg = ui_busyScreen_msgWnd->createWidget<MyGUI::EditBox>("WordWrapEmpty", 0, 0, 390, 80, MyGUI::Align::Stretch, "BusyScreen_Msg");
	ui_busyScreen_msg->setTextAlign(MyGUI::Align::Center);
	ui_busyScreen_msg->setEditMultiLine(true);
	ui_busyScreen_msg->setEditWordWrap(true);
	ui_busyScreen_msg->setEditStatic(true);

	ui_busyScreen->setVisible(false);
	ui_busyScreen->setAlpha(0);
}

void L2UIBusyScreen::update()
{
	//
}

void L2UIBusyScreen::onResize(int width, int height)
{
	ui_busyScreen_msgWnd->setPosition(width / 2 - ui_busyScreen_msgWnd->getWidth() / 2,
										height / 2 - ui_busyScreen_msgWnd->getHeight() / 2);
}

bool L2UIBusyScreen::injectMouseDown(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

bool L2UIBusyScreen::injectMouseUp(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

void L2UIBusyScreen::setMessage(wchar_t *msg)
{
	ui_busyScreen_msg->setCaption(msg);
}

void L2UIBusyScreen::setVisible(bool value)
{
	if(value)
	{
		MyGUI::LayerManager::getInstance().upLayerItem(ui_busyScreen);
		ui_busyScreen->setVisible(true);
		MyGUI::InputManager::getInstance().addWidgetModal(ui_busyScreen);
	}

	MyGUI::ControllerItem* item = MyGUI::ControllerManager::getInstance().createItem(MyGUI::ControllerFadeAlpha::getClassTypeName());
	MyGUI::ControllerFadeAlpha* controller = item->castType<MyGUI::ControllerFadeAlpha>();

	controller->setCoef(2.0);
	if(value)
		controller->setAlpha(1.0);
	else
		controller->setAlpha(0.0);

	if(!value)
		controller->eventPostAction += MyGUI::newDelegate(this, &L2UIBusyScreen::onFadeOutFinished);

	MyGUI::ControllerManager::getInstance().addItem(ui_busyScreen, controller);
}

void L2UIBusyScreen::onFadeOutFinished(MyGUI::Widget *sender)
{
	ui_busyScreen->setVisible(false);
	MyGUI::InputManager::getInstance().removeWidgetModal(ui_busyScreen);
}
