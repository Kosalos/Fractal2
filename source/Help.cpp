#include "stdafx.h"
#include "common.h"
#include "Help.h"

Help help;

#define CLASS_NAME2  "Help"

LRESULT CALLBACK HelpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		help.hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, "Listbox", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOSEL, 5, 5, 480,560, hWnd, (HMENU)101, NULL, NULL);
		CreateWindowEx(NULL, TEXT("button"), TEXT("Close"), WS_VISIBLE | WS_CHILD, 10, 565, 60, 20, hWnd, (HMENU)106, NULL, NULL);
		help.addHelptext();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case 106:
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Help::create(HWND parent, HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HelpWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASS_NAME2;
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex)) ABORT(-1);

	RECT rc2 = { 10, 10, 500,600 };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(CLASS_NAME2, "Help", WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);
	if (hWnd == NULL) ABORT(-1);

	ShowWindow(hWnd, SW_HIDE);
}

void Help::launch() {
	ShowWindow(hWnd, SW_SHOWNORMAL);
}

// -------------------------------------------------------------------

static const char* helptext[] = {
"Use the Arrow Keys to control the Parameters:",
" ",
"Up / Dn Arrows : Move Parameter focus",
"Lt / Rt Arrows : Alter value of focused Parameter",
" ",
"Hold <Shift> key for slow changes",
"Hold <Ctrl> key for fast changes",
"Hold both <Shift> and <Ctrl> for very fast changes",
"You can also mouse click on a parameter to move the focus directly",
" ",
"Keyboard commands:",
"1, 2 : Change Equation (previous, next)",
"3 : Toggle cross-eyed stereo viewing",
" ",
"Jogging the camera, and Rotating the view direction:",
"4, 5 for X axis Jog",
"6, 7 for Y axis",
"8, 9 for Z axis",
"Add <Shift> for slow jog",
"Add <Ctrl> for fast jog",
"Add both <Shift> and <Ctrl> for very fast jog",
" ",
"Add <Z> to Rotate the view direction rather than jog",
" ",
"G: Select next coloring style",
" ",
"Press <Spacebar> to Toggle Visibility of Parameter Window ",
""
};

void Help::addHelptext() {
	int i = 0;
	for (;;) {
		const char* str = helptext[i++];
		if (strlen(str) == 0) break;
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)str);
	}
}

