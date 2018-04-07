#pragma once

#include "_afx.h"

#include "../Resources/resource.h"

#include "config.h"
//#include "_shader.h"
//#include "math.h"

#include "core/L2Time.h"
#include "core/L2InputManager.h"
#include "L2Window.h"
#include "GUI/L2UIMain.h"
#include "scene/L2Scene.h"
#include "scene/L2Camera.h"
#include "scene/L2LevelManager.h"
#include "L2Geodata.h"
#include "L2TextureManager.h"
#include "L2Skybox.h"

#define g_main ::Singleton<main>::get_instance()
#define g_cfg ::Singleton<config>::get_instance()
//#define g_shader ::Singleton<_shader>::get_instance()
#define g_time ::Singleton<L2Time>::get_instance()
#define g_input ::Singleton<L2InputManager>::get_instance()
#define g_window ::Singleton<L2Window>::get_instance()
#define g_scene ::Singleton<L2Scene>::get_instance()
#define g_camera ::Singleton<L2Camera>::get_instance()
#define g_ui ::Singleton<L2UIMain>::get_instance()
#define g_geo ::Singleton<L2Geodata>::get_instance()
#define g_texMgr ::Singleton<L2TextureManager>::get_instance()
#define g_levelMgr ::Singleton<L2LevelManager>::get_instance()
#define g_skybox ::Singleton<L2Skybox>::get_instance()

class main
{
public:
	bool Init();
	void MainLoop();
public:
	main() {};
	~main() {};
};
