#include "config.h"

#include "main.h"

config::config(void)
{
	CFG_FULLSCREEN = false;
	CFG_SCREENRES_X = 1024;
	CFG_SCREENRES_Y = 700;
	CFG_SCREENBITS = 32;
	CFG_ANTIALIASING = 0;
}

config::~config(void)
{
}

static BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndDlgItem;
	int sel;

	switch (iMsg)
	{
		case WM_INITDIALOG:
			hwndDlgItem = GetDlgItem(hDlg, IDC_FULLSCREEN);
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"No");
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"Yes");
			SendMessageA(hwndDlgItem, CB_SETCURSEL, (WPARAM)0, 0);

			hwndDlgItem = GetDlgItem(hDlg, IDC_SCREENRES);
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"1024x768");
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"1280x1024");
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"1366x768");
			SendMessageA(hwndDlgItem, CB_SETCURSEL, (WPARAM)0, 0);

			hwndDlgItem = GetDlgItem(hDlg, IDC_ANTIALIASING);
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"no");
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"x2");
			SendMessageA(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)"x4");
			SendMessageA(hwndDlgItem, CB_SETCURSEL, (WPARAM)0, 0);

			// move to center
			int x, y, screenWidth, screenHeight;
            RECT rcDlg;
            GetWindowRect(hDlg, &rcDlg);
            screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
            screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

            x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
            y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);

            MoveWindow(hDlg, x, y, (rcDlg.right - rcDlg.left),
                (rcDlg.bottom - rcDlg.top), TRUE);

			return true;
		case WM_COMMAND:
            switch(LOWORD(wParam))
            {
				/*case IDC_SCREENRES:
					if(HIWORD(wParam) == CBN_SELCHANGE )
					{
						hwndDlgItem = GetDlgItem(hDlg, IDC_SCREENRES);
						sel = SendMessage(hwndDlgItem, LB_GETCURSEL, 0, 0);

						CFG_FULLSCREEN = sel == 1 ? true : false;
					}
					break;*/
				case IDOK:
					sel = SendDlgItemMessage(hDlg , IDC_FULLSCREEN, CB_GETCURSEL, 0, 0);

					g_cfg.CFG_FULLSCREEN = sel == 1 ? true : false;

					sel = SendDlgItemMessage(hDlg , IDC_SCREENRES, CB_GETCURSEL, 0, 0);

					switch(sel)
					{
						case 0:
							g_cfg.CFG_SCREENRES_X = 1024;
							g_cfg.CFG_SCREENRES_Y = 768;
							break;
						case 1:
							g_cfg.CFG_SCREENRES_X = 1280;
							g_cfg.CFG_SCREENRES_Y = 1024;
							break;
						case 2:
							g_cfg.CFG_SCREENRES_X = 1366;
							g_cfg.CFG_SCREENRES_Y = 768;
							break;
					}

					sel = SendDlgItemMessage(hDlg , IDC_ANTIALIASING, CB_GETCURSEL, 0, 0);

					switch(sel)
					{
						case 0:
							g_cfg.CFG_ANTIALIASING = 0;
							break;
						case 1:
							g_cfg.CFG_ANTIALIASING = 2;
							break;
						case 2:
							g_cfg.CFG_ANTIALIASING = 4;
							break;
					}

					EndDialog(hDlg, true);
					return true;

				case IDCANCEL:
					EndDialog(hDlg, false);
					return true;
			}
			break;
	}

	return false;
}

bool config::showDialog()
{
	HINSTANCE mHInstance = GetModuleHandle(NULL);
	int rs = DialogBoxA(mHInstance, MAKEINTRESOURCEA(IDD_STARTUPDLG), NULL, (DLGPROC)DlgProc);

	if (rs == -1)
    {
        int winError = GetLastError();
        char* errDesc;
        int i;

        errDesc = new char[255];

        i = FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            winError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPSTR) errDesc,
            255,
            NULL
        );
		MessageBoxA(0, errDesc, 0, 0);
    }

	return rs;
}
