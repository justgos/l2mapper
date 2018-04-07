#include "L2Window.h"

#include "main.h"

L2Window::L2Window(void)
{
	_isInitialized = false;
	_isActive = false;

	arbMultisampleSupported = false;
	arbMultisampleFormat = 0;
}

L2Window::~L2Window(void)
{
}

LRESULT CALLBACK WinMsgDefProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_DESTROY: 
		{
			if(g_window.isInitialized())
			{
				PostQuitMessage(0);
				return 0;
			}
		}
		case WM_ACTIVATE:
		{
			if(wparam == WA_INACTIVE)
				g_window.setIsActive(false);
			else
				g_window.setIsActive(true);

			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			if(g_input.injectMouseDown(MyGUI::MouseButton::Left))
				return 0;

			return 0;
		}
		case WM_LBUTTONUP:
		{
			if(g_input.injectMouseUp(MyGUI::MouseButton::Left))
				return 0;

			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			if(g_input.injectMouseDown(MyGUI::MouseButton::Right))
				return 0;

			return 0;
		}
		case WM_RBUTTONUP:
		{
			if(g_input.injectMouseUp(MyGUI::MouseButton::Right))
				return 0;

			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			int16 diff = HIWORD(wparam);
			g_input.injectMouseScroll(diff);
			return 0;
		}
		case WM_KEYDOWN:
		{
			g_input.injectKeyDown(wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			g_input.injectKeyUp(wparam);
			return 0;
		}
		case WM_SIZE:
			g_window.onResize();
			break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool L2Window::create()
{
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)g_cfg.CFG_SCREENRES_X;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)g_cfg.CFG_SCREENRES_Y;		// Set Bottom Value To Requested Height

	g_hinst			    = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= WinMsgDefProc;					// WndProc Handles Messages // (WNDPROC) MainWinProc
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= g_hinst;							// Set The Instance
	wc.hIcon			= LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON1));			// Load The Default Icon
	//wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hCursor			= LoadCursor(g_hinst, MAKEINTRESOURCE(IDC_L2POINTER));			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= L"l2mapper";								// Set The Class Name

	RegisterClass(&wc);

	///////////////////////////////
	_create();
	///////////////////////////////

	if(g_cfg.CFG_ANTIALIASING > 0)
	{
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
			(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		int pixelFormat;
		UINT numFormats;
		float fAttributes[] = {0,0};

		int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,24,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB, g_cfg.CFG_ANTIALIASING,						// 4x multisampling
			0,0};

		wglChoosePixelFormatARB(g_dc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

		arbMultisampleSupported = true;
		arbMultisampleFormat = pixelFormat;

		_destroy();
		_create();
	}

	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		MessageBox(0, TEXT("Glew failed to initialize!"), 0, 0);
		return false;
	}

	ShowWindow(g_hwnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(g_hwnd);						// Slightly Higher Priority
	SetFocus(g_hwnd);									// Sets Keyboard Focus To The Window

	return true;
}

void L2Window::_create()
{
	if(g_cfg.CFG_FULLSCREEN)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= g_cfg.CFG_SCREENRES_X;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= g_cfg.CFG_SCREENRES_Y;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= g_cfg.CFG_SCREENBITS;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);

		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		//ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW; //  & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	g_hwnd=CreateWindowEx(dwExStyle,							// Extended Style For The Window
								L"l2mapper",							// Class Name
								L"L2 Content Editor v0.1",								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								g_cfg.CFG_SCREENRES_X,
								g_cfg.CFG_SCREENRES_Y,
								NULL,								// No Parent Window
								NULL,								// No Menu
								g_hinst,							// Instance
								NULL);								// Dont Pass Anything To WM_CREATE

	static PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		g_cfg.CFG_SCREENBITS,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	g_dc = GetDC(g_hwnd);

	if(!arbMultisampleSupported)
	{
		PixelFormat = ChoosePixelFormat(g_dc,&pfd);
	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}

	if(!SetPixelFormat(g_dc,PixelFormat,&pfd)) MessageBoxA(0, "", 0, 0);

	g_glrc=wglCreateContext(g_dc);

	wglMakeCurrent(g_dc,g_glrc);
}

bool L2Window::_destroy()
{
	if (g_hwnd != 0)												// Does The Window Have A Handle?
	{	
		if (g_dc != 0)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent(g_dc, 0);							// Set The Current Active Rendering Context To Zero
			if (g_dc != 0)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext(g_glrc);							// Release The Rendering Context
				g_dc = 0;										// Zero The Rendering Context

			}
			ReleaseDC(g_hwnd, g_dc);						// Release The Device Context
			g_dc = 0;											// Zero The Device Context
		}
		DestroyWindow(g_hwnd);									// Destroy The Window
		g_hwnd = 0;												// Zero The Window Handle
	}


	//ChangeDisplaySettings(NULL,0);									// Switch Back To Desktop Resolution
	ShowCursor(true);												// Show The Cursor	

	return true;														// Return True
}

void L2Window::onResize()
{
	RECT rect;
	GetClientRect(g_window.getHWND(), &rect);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	
	g_camera.onResize(width, height);

	g_ui.onResize(width, height);
}

_vector2 L2Window::getInnerSize()
{
	RECT rect;
	GetClientRect(g_hwnd, &rect);
	return _vector2(rect.right - rect.right, rect.bottom - rect.top);
}
