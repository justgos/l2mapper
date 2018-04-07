#include "L2UIMap.h"

#include "../main.h"
#include "L2UIBusyScreen.h"

L2UIMap::L2UIMap(void)
	: L2UIBaseWidget()
{
	isDraggingMap = false;
	hasDraggedMap = false;
}

L2UIMap::~L2UIMap(void)
{
}

void L2UIMap::Init()
{
	ui_mapWnd = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("WindowCSX", 60, 44, 500, 400, MyGUI::Align::Default, "L2WindowsLayer", "MapWnd");
	ui_mapWnd->setCaption(L"Map");
	ui_mapWnd->eventWindowButtonPressed += MyGUI::newDelegate(this, &L2UIMap::onMapWindowClose);
	ui_mapView = ui_mapWnd->createWidget<MyGUI::ScrollView>("ScrollView", 0, 0, 500, 400, MyGUI::Align::Stretch, "MapView");
	//ui_mapView->setAlign(MyGUI::Align::HStretch | MyGUI::Align::VStretch);
	ui_mapView->setVisibleVScroll(false);
	ui_mapView->setVisibleHScroll(false);
	ui_mapView->setCanvasSize(2796, 2650);
	ui_map = ui_mapView->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, 2796, 2620, MyGUI::Align::Default, "Map");
	ui_map->setProperty("ImageTexture", "textures/map_ct25.jpg");
	ui_map->eventMouseButtonPressed += MyGUI::newDelegate(this, &L2UIMap::onMapMouseDown);
	ui_map->eventMouseButtonReleased += MyGUI::newDelegate(this, &L2UIMap::onMapMouseUp);
	ui_mapView->setViewOffset(MyGUI::IntPoint(-1443, -1745));

	char buf[256];
	char *tbuf;
	ui_mapTiles = new MyGUI::ImageBox**[MAP_TILES_X];
	tileStates = new int16*[MAP_TILES_X];
	for(int x = 10; x < 30; x++)
	{
		ui_mapTiles[x] = new MyGUI::ImageBox *[MAP_TILES_Y];
		memset(ui_mapTiles[x], 0, MAP_TILES_Y * sizeof(MyGUI::ImageBox*));
		tileStates[x] = new int16[MAP_TILES_Y];
		memset(tileStates[x], 0, MAP_TILES_Y * sizeof(int16));
		for(int y = 10; y < MAP_TILES_Y; y++)
		{
			sprintf(buf, "%d_%d", x, y);
			if(UPackageMgr.GetUPackage(buf))
			{
				float tileWidth = 164;
				float tileHeight = 162;
				MyGUI::ImageBox *ui_mapTile_temp = ui_mapView->createWidget<MyGUI::ImageBox>("ImageBox", 820 + (x - 15) * tileWidth, 2440 + (y - 25) * tileHeight, tileWidth, tileHeight, MyGUI::Align::Default);
				//ui_mapTile_temp->setImageTexture("textures/mapTile.png");
				tbuf = new char[3];
				sprintf(tbuf, "%d", x);
				ui_mapTile_temp->setUserString("map_x", tbuf);
				tbuf = new char[3];
				sprintf(tbuf, "%d", y);
				ui_mapTile_temp->setUserString("map_y", tbuf);
				ui_mapTile_temp->eventMouseButtonPressed += MyGUI::newDelegate(this, &L2UIMap::onMapMouseDown);
				ui_mapTile_temp->eventMouseButtonReleased += MyGUI::newDelegate(this, &L2UIMap::onMapMouseUp);
				//ui_mapTile_temp->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIMap::onMapMouseClick);
				ui_mapTiles[x][y] = ui_mapTile_temp;
				setTileState(x, y, 0);
			}
		}
	}

	ui_mapTileLoad = ui_mapWnd->createWidget<MyGUI::Button>("Button", 220, 320, 120, 30, MyGUI::Align::Right | MyGUI::Align::Bottom, "MapTileLoad");
	ui_mapTileLoad->setCaption(L"Load");
	ui_mapTileLoad->setEnabled(false);
	ui_mapTileLoad->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIMap::onMapTileLoadClick);

	ui_mapTileHide = ui_mapWnd->createWidget<MyGUI::Button>("Button", 355, 320, 120, 30, MyGUI::Align::Right | MyGUI::Align::Bottom, "MapTileHide");
	ui_mapTileHide->setCaption(L"Hide");
	ui_mapTileHide->setEnabled(false);
	ui_mapTileHide->eventMouseButtonClick += MyGUI::newDelegate(this, &L2UIMap::onMapTileHideClick);

	ui_mapWnd->setVisible(false);
}

void L2UIMap::update()
{
	if(isDraggingMap)
	{
		if(abs(g_input.getMousePos().x - mapDragStartMousePos.x) > 2
			|| abs(g_input.getMousePos().y - mapDragStartMousePos.y) > 2)
		{
			hasDraggedMap = true;
		}
		ui_mapView->setViewOffset(MyGUI::IntPoint(mapDragStartContentPos.x + (g_input.getMousePos().x - mapDragStartMousePos.x),
													mapDragStartContentPos.y + (g_input.getMousePos().y - mapDragStartMousePos.y)));
	}
}

bool L2UIMap::injectMouseDown(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

bool L2UIMap::injectMouseUp(int x, int y, MyGUI::MouseButton btn)
{
	return false;
}

void L2UIMap::setVisible(bool value)
{
	MyGUI::LayerManager::getInstance().upLayerItem(ui_mapWnd);
	ui_mapWnd->setVisible(value);
}

void L2UIMap::onTilesLoaded()
{
	g_ui.getL2BusyScreen()->setVisible(false);
}

void L2UIMap::onMapMouseDown(MyGUI::Widget* sender, int x, int y, MyGUI::MouseButton btn)
{
	if(btn == MyGUI::MouseButton::Left)
	{
		if(!isDraggingMap)
		{
			isDraggingMap = true;
			hasDraggedMap = false;
			mapDragStartMousePos = g_input.getMousePos();
			mapDragStartContentPos.x = ui_mapView->getViewOffset().left;
			mapDragStartContentPos.y = ui_mapView->getViewOffset().top;
		}
	}
}

void L2UIMap::onMapMouseUp(MyGUI::Widget* sender, int x, int y, MyGUI::MouseButton btn)
{
	if(btn == MyGUI::MouseButton::Left)
	{
		if(isDraggingMap)
		{
			isDraggingMap = false;

			if(!hasDraggedMap)
			{
				onMapMouseClick(sender, btn);
			}
		}
	}
	else if(btn == MyGUI::MouseButton::Right)
	{
		onMapMouseClick(sender, btn);
	}
}

void L2UIMap::onMapMouseClick(MyGUI::Widget* sender, MyGUI::MouseButton btn)
{
	if(btn == MyGUI::MouseButton::Left)
	{
		if(sender && sender->isType<MyGUI::ImageBox>() && sender != ui_map)
		{
			int16 map_x = atoi(sender->castType<MyGUI::ImageBox>()->getUserStrings().at("map_x").c_str());
			int16 map_y = atoi(sender->castType<MyGUI::ImageBox>()->getUserStrings().at("map_y").c_str());

			for(uint16 x = 10; x < MAP_TILES_X; x++)
			{
				for(uint16 y = 10; y < MAP_TILES_Y; y++)
				{
					if(!(x == map_x && map_y == y))
						setTileState(x, y, getTileState(x, y) & ~L2UIMTS_SELECTED);
				}
			}

			int16 tileState = getTileState(map_x, map_y);

			if(tileState & L2UIMTS_SELECTED)
				setTileState(map_x, map_y, tileState & ~L2UIMTS_SELECTED);
			else
				setTileState(map_x, map_y, tileState | L2UIMTS_SELECTED);

			bool nonloadedTileSelected = false;
			bool activeTileSelected = false;

			for(uint16 x = 10; x < MAP_TILES_X; x++)
			{
				for(uint16 y = 10; y < MAP_TILES_Y; y++)
				{
					uint16 tileState = getTileState(x, y);
					if((tileState & L2UIMTS_SELECTED) && !(tileState & L2UIMTS_ACTIVE))
					{
						nonloadedTileSelected = true;
					}
					if((tileState & L2UIMTS_SELECTED) && (tileState & L2UIMTS_ACTIVE))
					{
						activeTileSelected = true;
					}
				}
			}

			if(nonloadedTileSelected)
				ui_mapTileLoad->setEnabled(true);
			else
				ui_mapTileLoad->setEnabled(false);

			if(activeTileSelected)
				ui_mapTileHide->setEnabled(true);
			else
				ui_mapTileHide->setEnabled(false);

			/*g_ui.getL2BusyScreen()->setMessage(L"Загрузка сектора...");
			g_ui.getL2BusyScreen()->setVisible(true);
			DWORD tid;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&loadLevel, sender, 0, &tid);
			setVisible(false);*/
		}
	}
	else if(btn == MyGUI::MouseButton::Right)
	{
		/*if(sender->isType<MyGUI::ImageBox>())
		{
			g_levelMgr.setLevelVisible(atoi(sender->castType<MyGUI::ImageBox>()->getUserStrings().at("map_x").c_str()),
										atoi(sender->castType<MyGUI::ImageBox>()->getUserStrings().at("map_y").c_str()),
										false);
		}*/
	}
}

void L2UIMap::onMapWindowClose(MyGUI::Window* sender, const std::string& evt)
{
	setVisible(false);
}

void L2UIMap::onMapTileLoadClick(MyGUI::Widget* sender)
{
	jfArray<L2MapTileInfo*, uint16> *tiles = new jfArray<L2MapTileInfo*, uint16>();
	for(uint16 x = 10; x < MAP_TILES_X; x++)
	{
		for(uint16 y = 10; y < MAP_TILES_Y; y++)
		{
			uint16 tileState = getTileState(x, y);
			if((tileState & L2UIMTS_SELECTED) && !(getTileState(x, y) & L2UIMTS_ACTIVE))
			{
				L2MapTileInfo *tileInfo = new L2MapTileInfo();
				tileInfo->map_x = x;
				tileInfo->map_y = y;
				tiles->add(tileInfo);
				//g_geo.Load(x, y);
				setTileState(x, y, tileState | L2UIMTS_LOADED | L2UIMTS_ACTIVE);
			}
			setTileState(x, y, getTileState(x, y) & ~L2UIMTS_SELECTED);
		}
	}

	ui_mapTileLoad->setEnabled(false);
	ui_mapTileHide->setEnabled(false);

	g_ui.getL2BusyScreen()->setMessage(L"Loading tiles...");
	g_ui.getL2BusyScreen()->setVisible(true);
	//setVisible(false);
	g_levelMgr.loadTiles(tiles);
}

void L2UIMap::onMapTileHideClick(MyGUI::Widget* sender)
{
	//jfArray<L2MapTileInfo*, uint16> *tiles = new jfArray<L2MapTileInfo*, uint16>();
	for(uint16 x = 10; x < MAP_TILES_X; x++)
	{
		for(uint16 y = 10; y < MAP_TILES_Y; y++)
		{
			uint16 tileState = getTileState(x, y);
			if((tileState & L2UIMTS_SELECTED) && (getTileState(x, y) & L2UIMTS_ACTIVE))
			{
				g_levelMgr.setLevelVisible(x, y, false);
				setTileState(x, y, tileState & ~L2UIMTS_ACTIVE);
			}
			setTileState(x, y, getTileState(x, y) & ~L2UIMTS_SELECTED);
		}
	}

	ui_mapTileLoad->setEnabled(false);
	ui_mapTileHide->setEnabled(false);
}

int16 L2UIMap::getTileState(int x, int y)
{
	return tileStates[x][y];
}

void L2UIMap::setTileState(int x, int y, int16 state)
{
	tileStates[x][y] = state;

	if(!ui_mapTiles[x][y])
		return;

	if(state & L2UIMTS_ACTIVE)
	{
		if(state & L2UIMTS_SELECTED)
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile_selected.png");
		}
		else
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile_active.png");
		}
	}
	else if(state & L2UIMTS_LOADED)
	{
		if(state & L2UIMTS_SELECTED)
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile_selected.png");
		}
		else
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile_loaded.png");
		}
	}
	else
	{
		if(state & L2UIMTS_SELECTED)
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile_selected.png");
		}
		else
		{
			ui_mapTiles[x][y]->setImageTexture("textures/mapTile.png");
		}
	}
}
