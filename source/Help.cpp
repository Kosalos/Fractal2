#include "common.h"
#include "Help.h"

Help help;

static char* CLASS_NAME = "Help";

LRESULT CALLBACK HelpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		help.hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, "Listbox", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOSEL, 5, 5, 480,560, hWnd, (HMENU)101, NULL, NULL);
		CreateWindowEx(NULL, TEXT("button"), TEXT("Close"), WS_VISIBLE | WS_CHILD, 10, 565, 60, 20, hWnd, (HMENU)106, NULL, NULL);
		SendMessage(help.hWndList, WM_SETFONT, WPARAM(help.font), 0);
		help.addHelptext();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case 106:
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
		break;
	case WM_ERASEBKGND:
	{
		RECT rc;
		HDC hdc = (HDC)wParam;
		GetClientRect(hWnd, &rc);
		FillRect(hdc, &rc, CreateSolidBrush(RGB(200, 235, 200)));
		return 1L;
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
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex)) ABORT(-1);

	RECT rc2 = { 10, 10, 500,600 };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	font = CreateFont(14, 7, 0, 0,
		FW_NORMAL,
		FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN,
		"Courier New");

	hWnd = CreateWindow(CLASS_NAME, "Help", WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);
	if (hWnd == NULL) ABORT(-1);

	ShowWindow(hWnd, SW_HIDE);
}

void Help::launch() {
	ShowWindow(hWnd, SW_SHOWNORMAL);
}

// -------------------------------------------------------------------

static const char* helptext[] = {
"Use the Arrow Keys to control the Parameters:",
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
"P: Save image to BMP file",
"Press <Spacebar> to Toggle Visibility of Parameter Window ",
" ",
"Mouse Commands:",
"Left Mouse Button + Drag = Move Camera in X and Y axes",
"Hold down 'A' while dragging  to Move Camera in X and Z axes",
"Hold down 'Z' while dragging  to Rotate Camera",
"Mouse Wheel moves Parameter focus up/down",
"Right Mouse Button + Drag = Alter focused parameter",
" ",
"Save/Load:",
"Press 'S' to save the current settings to file.",
"Press 'L' to launch the Save/Load dialog.",
"",  // must mark end of list with an empty string!
};

void Help::addHelptext() {
	int i = 0;
	for (;;) {
		const char* str = helptext[i++];
		if (strlen(str) == 0) break;
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)str);
	}
}

