#include "stdafx.h"
#
#include "Widget.h"
#include "Fractal.h"

LRESULT CALLBACK WidgetWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT:
		cWidget.drawWindow();
		break;
	case WM_ERASEBKGND:
		return (LRESULT)1;

	case WM_DESTROY:
		cWidget.destroy();
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Widget::create(char* className, char* title, HWND parent, HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WidgetWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, "Widget Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	RECT rc2 = { 100, 100, 100 + 300,100 + 340 };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(className, title, WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT,
		rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, "Widget Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetFocus(hWnd);

	font = CreateFont(16, 8, 0, 0,
		FW_NORMAL,
		FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN,
		"Terminal");
}

void Widget::destroy() {
	DeleteObject(font);
}

#define YHOP 16

const char* help1[] = {
	"Apollonian",
	" ",
	"Key Commands",
	"1,2 : alter #iterations",
	"Q,W : alter Multiplier",
	"A,S : alter Foam",
	"Z,X : alter Foam2",
	"E,R : alter Bend",
	"4,5 : Jog Left/Right",
	"6,7 : Jog Up/Down",
	"8,9 : Jog Forward/Back",
	" ",
	"3   : Reset",
	"<Spc> : Switch to MandelBrot",
	" ",
	"Hold <Shift> to alter slowly",
	"Hold <Ctrl>  to alter quickly",
	"Hold <Shift> + <Ctrl> to alter very quickly",
	" ",
	"Mouse pans the image",
	""
};

void Widget::drawWindow() {
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	int index = 0;
	int x = 5;
	int y = 5;
	const char** help = help1;


	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	SetTextColor(hdc,RGB(0, 0, 0));
	
	for (;;) {
		int len = strlen(help[index]);
		if (len == 0) break;
		TextOut(hdc, x, y, help[index], len);
		y += YHOP;
		++index;

	}

	EndPaint(hWnd, &ps);
}
