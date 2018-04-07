#include "L2UIEditor.h"

#include "../main.h"
#include "L2UIMap.h"

L2UIEditor::L2UIEditor()
	: L2UIBaseWidget()
{
}

L2UIEditor::~L2UIEditor()
{
}

void L2UIEditor::Init()
{
	ui_topMenu = MyGUI::Gui::getInstance().createWidget<MyGUI::MenuBar>("MenuBar", 0, 0, 300, 24, MyGUI::Align::Default, "L2Editor", "TopMenu");
	ui_topMenu->setAlign(MyGUI::Align::HStretch | MyGUI::Align::Top);
	MyGUI::MenuItemPtr ui_topMenu_Main = ui_topMenu->addItem(L"Menu", MyGUI::MenuItemType::Popup);
	MyGUI::PopupMenuPtr ui_topMenu_MainMenu = ui_topMenu_Main->createItemChildT<MyGUI::PopupMenu>();
	MyGUI::MenuItem * ui_topMenu_Main_Map = ui_topMenu_MainMenu->addItem(L"Map", MyGUI::MenuItemType::Normal, "TopMenu_Main_ShowMap");
	ui_topMenu_Main_Map->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIEditor::onTopMenu_Main_ShowMap);
	ui_topMenu_MainMenu->addItem("", MyGUI::MenuItemType::Separator);
	ui_topMenu_MainMenu->addItem(L"Exit", MyGUI::MenuItemType::Normal, "TopMenu_Main_Exit");
	ui_leftPanel = MyGUI::Gui::getInstance().createWidget<MyGUI::ImageBox>("ImageBox", 0, 22, 40, 300, MyGUI::Align::Default, "L2Editor", "LeftPanel");
	ui_leftPanel->setAlign(MyGUI::Align::Left | MyGUI::Align::VStretch);
	ui_leftPanel->setProperty("ImageTexture", "ui_leftPanelBg.png");
	MyGUI::Button *btn1 = ui_leftPanel->createWidget<MyGUI::Button>("Button", 4, 6, 32, 32, MyGUI::Align::Default, "btn1");
	btn1->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIEditor::onBtn1MouseClick);

	ui_sceneShowBsp = MyGUI::Gui::getInstance().createWidget<MyGUI::Button>("CheckBox", 45, 27, 150, 20, MyGUI::Align::Default, "L2Editor", "ShowBsp");
	ui_sceneShowBsp->setCaption(L"Draw BSP");
	ui_sceneShowBsp->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIEditor::onShowBspMouseClick);

	/*ui_propertyPanel = mGUI->createWidget<MyGUI::TabControl>("TabControl", 0, 24, 300, 500, MyGUI::Align::Default, "Main", "PropertyPanel");
	//ui_propertyPanel->setAlign(MyGUI::Align::Right | MyGUI::Align::VStretch);
	ui_propertyPanel_Common = ui_propertyPanel->addSheet(L"Общее");
	ui_propertyPanel_Common_List = ui_propertyPanel_Common->createWidget<MyGUI::MultiListBox>("MultiListBox", 5, 5, 285, 400, MyGUI::Align::Default, "PropertyPanel_Common_List");
	ui_propertyPanel_Common_List->addColumn(L"Парам.", 88);
	ui_propertyPanel_Common_List->addColumn(L"Знач.", 191);*/

	// onResize
	/*//g_main.ui_topMenu->setSize(width, 24);
	//g_main.ui_leftPanel->setSize(40, height - 22);
	g_main.ui_propertyPanel->setPosition(width - 300, 24);
	g_main.ui_propertyPanel->setSize(300, height - 24);
	g_main.ui_propertyPanel_Common_List->setSize(285, height - 60);*/

	// show AActor props
	/*char buf[256];

	g_main.ui_propertyPanel_Common_List->removeAllItems();
	if(targetActor->m_Mesh->ObjectName != 0)
	{
		g_main.ui_propertyPanel_Common_List->addItem("Name");
		g_main.ui_propertyPanel_Common_List->setSubItem(1, g_main.ui_propertyPanel_Common_List->getItemCount() - 1, targetActor->m_Mesh->ObjectName);
	}

	g_main.ui_propertyPanel_Common_List->addItem("Location");
	sprintf(buf, "%.2f, %.2f, %.2f", targetActor->m_Location.Z, targetActor->m_Location.Y, targetActor->m_Location.Z);
	g_main.ui_propertyPanel_Common_List->setSubItem(1, g_main.ui_propertyPanel_Common_List->getItemCount() - 1, buf);
	g_main.ui_propertyPanel_Common_List->addItem("Rotation");
	sprintf(buf, "%d, %d, %d", targetActor->m_Rotation.Pitch, targetActor->m_Rotation.Yaw, targetActor->m_Rotation.Roll);
	g_main.ui_propertyPanel_Common_List->setSubItem(1, g_main.ui_propertyPanel_Common_List->getItemCount() - 1, buf);
	g_main.ui_propertyPanel_Common_List->addItem("DrawScale3D");
	sprintf(buf, "%.2f, %.2f, %.2f", targetActor->m_DrawScale3D.Z, targetActor->m_DrawScale3D.Y, targetActor->m_DrawScale3D.Z);
	g_main.ui_propertyPanel_Common_List->setSubItem(1, g_main.ui_propertyPanel_Common_List->getItemCount() - 1, buf);
	g_main.ui_propertyPanel_Common_List->addItem("DrawScale");
	sprintf(buf, "%.2f", targetActor->m_DrawScale);
	g_main.ui_propertyPanel_Common_List->setSubItem(1, g_main.ui_propertyPanel_Common_List->getItemCount() - 1, buf);

	g_main.ui_propertyPanel->setVisible(true);*/
}

void L2UIEditor::update()
{
	//
}

bool L2UIEditor::injectMouseDown(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

bool L2UIEditor::injectMouseUp(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

void L2UIEditor::onTopMenu_Main_ShowMap(MyGUI::Widget* sender)
{
	g_ui.getL2Map()->setVisible(true);
}

void L2UIEditor::onBtn1MouseClick(MyGUI::Widget* sender)
{
	char *terrain_vert_shader;
	char *terrain_frag_shader;

	uint32 t, fend;

	FILE *sh_file = fopen("data/shaders/terrain.vert", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	terrain_vert_shader = new char[fend - t + 1];
	fread(terrain_vert_shader, fend - t, 1, sh_file);
	terrain_vert_shader[fend - t] = 0;
	fclose(sh_file);

	sh_file = fopen("data/shaders/terrain.frag", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	terrain_frag_shader = new char[fend - t + 1];
	fread(terrain_frag_shader, fend - t, 1, sh_file);
	terrain_frag_shader[fend - t] = 0;
	fclose(sh_file);

	g_shader.Add(terrain_vert_shader, terrain_frag_shader, "L2Terrain");


	sh_file = fopen("data/shaders/default.vert", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	terrain_vert_shader = new char[fend - t + 1];
	fread(terrain_vert_shader, fend - t, 1, sh_file);
	terrain_vert_shader[fend - t] = 0;
	fclose(sh_file);

	sh_file = fopen("data/shaders/default.frag", "rb");
	t = ftell(sh_file);
	fseek(sh_file, 0, SEEK_END);
	fend = ftell(sh_file);
	fseek(sh_file, 0, SEEK_SET);
	terrain_frag_shader = new char[fend - t + 1];
	fread(terrain_frag_shader, fend - t, 1, sh_file);
	terrain_frag_shader[fend - t] = 0;
	fclose(sh_file);

	g_shader.Add(terrain_vert_shader, terrain_frag_shader, "L2Default");
}

void L2UIEditor::onShowBspMouseClick(MyGUI::Widget* sender)
{
	sender->castType<MyGUI::Button>()->setStateCheck(!sender->castType<MyGUI::Button>()->getStateCheck());
	g_levelMgr.getVars()->showBsp = sender->castType<MyGUI::Button>()->getStateCheck();
}
