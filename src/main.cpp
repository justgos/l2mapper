#include "main.h"

//FGlobalMath	GMath;

UStaticMeshActor *targetActor = 0;

bool main::Init()
{
	/*if(!g_cfg.showDialog())
	{
		return false;
	}*/

	//g_geo.Load("25_14.l2j");

	g_window.create();

	g_input.Init();

	g_levelMgr.Init();

	//uttex = dynamic_cast<UTexture*>(UPackageMgr.GetUObject("t_schtgart.SCSN01"));
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	g_skybox.Init();

	g_ui.Init();

	g_scene.Init();

	g_geo.Init();

	g_window.onResize();

	return true;
}

void main::MainLoop()
{
	if(g_window.isActive())
		Sleep(10);
	else
		Sleep(1000);

	g_time.update();

	g_input.update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffers
	//glClearColor(120.0f/255.0f, 120.0f/255.0f, 120.0f/255.0f, 1.0f);
	//glClearColor(1, 1, 1, 1.0f);
	glClearColor(0.5, 0.5, 0.6, 1.0f);
	glLoadIdentity(); // reset world matrix..

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	g_scene.preUpdate();

	g_camera.update();

	g_scene.update();

	g_ui.update();

	SwapBuffers(g_window.getDC());

	//Sleep(1000);

	if(GetAsyncKeyState(VK_ESCAPE) && g_window.isActive())
		exit(0);

	if(!g_window.isInitialized())
		g_window.setIsInitialized(true);
}

int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	MSG msg;

	ZeroMemory(&msg, sizeof(msg));

	if(!g_main.Init())
		return 0;

	while(msg.message != WM_QUIT) 
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_main.MainLoop();
		}
	}
	return 0;
}
