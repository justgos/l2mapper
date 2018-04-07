#pragma once

#include <windows.h>
#include <stdio.h>
#include <conio.h>
/*#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/wglext.h>
#include <gl/glext.h>*/
#include <math.h>
#include <mmsystem.h>
#include <string>
#include <fstream>

#include "L2Lib/UEngine.h"

//#include <OIS/OIS.h>

//#include <CEGUI.h>
//#include <CEGUIDefaultResourceProvider.h>
//#include <RendererModules/OpenGL/CEGUIOpenGLRenderer.h>
//#include <CEGUIRenderingRoot.h>
//#include <CEGUIGeometryBuffer.h>

//#pragma comment (lib, "L2Lib.lib")

//#pragma comment (lib, "OIS_static.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLu32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "glaux.lib")
#pragma comment (lib, "glew32s.lib")
//#pragma comment (lib, "dsound.lib")

#include <FreeImage.h>
#ifdef _DEBUG
#pragma comment (lib, "FreeImage.lib")
#else
#pragma comment (lib, "FreeImage.lib")
#endif

#include "MyGUI.h"
#include "MyGUI_OpenGLPlatform.h"
#ifdef _DEBUG
#pragma comment (lib, "MyGUIEngine_d.lib")
#pragma comment (lib, "MyGUI.OpenGLPlatform_d.lib")
#else
#pragma comment (lib, "MyGUIEngine.lib")
#pragma comment (lib, "MyGUI.OpenGLPlatform.lib")
#endif


//#include "glext.h"

// typedefs from MaNGOS WoW serv
typedef __int64            int64;
typedef long               int32;
typedef short              int16;
typedef char               int8;
typedef unsigned __int64   uint64;
typedef unsigned long      uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;

#define _PI 3.141592653589793

#define GL_CLAMP_TO_EDGE	0x812F

#define WGL_SAMPLE_BUFFERS_ARB	0x2041
#define WGL_SAMPLES_ARB		0x2042
