#include "stdafx.h"
#include "Widget.h"
#include "Fractal.h"
#include "Help.h"

// ----------------------------------------------------------------------

bool WidgetData::alterValue(int direction) {
	switch (kind) {
	case kfloat:
	{
		fractal.updateAlterationSpeed();

		float value = *(float*)valuePtr;
		float oldValue = value;
		float amt = delta * fractal.alterationSpeed;

		//char str[128];
		//sprintf_s(str, 127, "alt%8.6f  amt %8.6f\n", alterationSpeed, amt);
		//OutputDebugString(str);

		value += direction > 0 ? amt : -amt;
		value = max(min(value, rangey), rangex);

		if (value != oldValue) {
			*(float*)valuePtr = value;
			return true;
		}
	}
	break;

	case kinteger:
	{
		int value = *(int*)valuePtr;
		int oldValue = value;

		value += direction * delta;
		value = max(min(value, int(rangey)), int(rangex));

		if (value != oldValue) {
			*(int*)valuePtr = value;
			return true;
		}
	}
	break;

	case kboolean:
	{
		int value = *(int*)valuePtr;
		*(int*)valuePtr = 1 - value;
		return true;
	}
	break;
	}

	return false;
}

char* WidgetData::valueString() {
	static char str[LEGEND_LENGTH + 1] = { 0 };
	switch (kind) {
	case kboolean:
	{
		bool value = *(bool*)valuePtr;
		return value ? "Yes" : "No";
	}
	break;
	case kinteger:
	{
		int value = *(int*)valuePtr;
		sprintf_s(str, LEGEND_LENGTH, "%3d", value);
	}
	break;
	case kfloat:
	{
		float value = *(float*)valuePtr;
		if (value > 1000)
			sprintf_s(str, LEGEND_LENGTH, "%8.2f", value);
		else
			sprintf_s(str, LEGEND_LENGTH, "%8.5f", value);
	}
	break;
	}
	return str;
}

char* WidgetData::displayString(bool showV) {
	static char str[LEGEND_LENGTH + 1];

	if (showV) {
		switch (kind) {
		case kfloat:
		case kinteger:
		case kboolean:
			sprintf_s(str, LEGEND_LENGTH, "%-22s : %s", legend, valueString());
			break;
		default:
			strcpy_s(str, LEGEND_LENGTH, legend);
			break;
		}
	}
	else {
		strcpy_s(str, LEGEND_LENGTH, legend);
	}

	return str;
}

// ---------------------------------------------------------------------

Widget widget;
static int yPos, previousFocus = 0;

static char* CLASS_NAME = "Widget";
#define BTN_BUTTON1  2000

LRESULT CALLBACK WidgetWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT:
		widget.drawWindow();
		break;
	case WM_ERASEBKGND:
		return (LRESULT)1;

	case WM_KEYDOWN:
		fractal.keyDown(int(wParam));
		break;

	case WM_KEYUP:
		fractal.keyUp(int(wParam));
		break;

	case WM_LBUTTONDOWN:
		yPos = GET_Y_LPARAM(lParam);
		widget.jumpFocus();
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == BTN_BUTTON1)
			help.launch();
		SetFocus(hWnd);
		break;
	case WM_MOUSEWHEEL:
	{
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		widget.moveFocus(-direction / 120);
	}
	break;
	case WM_DESTROY:
		DeleteObject(widget.font);
		DeleteObject(widget.hdcMem);
		DeleteObject(widget.hbmMem);
		DeleteObject(widget.hbmOld);
		DeleteObject(widget.hbrBkGnd);
		DeleteObject(widget.hfntOld);
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Widget::create(HWND parent, HINSTANCE hInstance) {
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
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, "Widget Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	RECT rc2 = { 100, 100, 100 + 220,650 };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(CLASS_NAME, "Parameters", WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT,
		rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);
	if (hWnd == NULL) {
		MessageBox(NULL, "Widget Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	CreateWindow(TEXT("button"), TEXT("Help"), WS_CHILD | WS_VISIBLE, 60, 510, 80, 30, hWnd, (HMENU)BTN_BUTTON1, 0, 0);

	isVisible = true;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetFocus(hWnd);

	font = CreateFont(16, 7, 0, 0,
		FW_NORMAL,
		FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN,
		"Courier New");
}

void Widget::toggleVisible() {
	ShowWindow(hWnd, isVisible ? SW_HIDE : SW_SHOWNORMAL);
	isVisible = !isVisible;
}

void Widget::drawWindow() {
#define YTOP 10
#define YHOP 14
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	int x = 5;
	int y = YTOP;
	const char* str;

	if (hdcMem == NULL) {
		GetClientRect(hWnd, &rc);
		hdcMem = CreateCompatibleDC(hdc);
		hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
		hbrBkGnd = CreateSolidBrush(RGB(230, 245, 230));
		hfntOld = (HFONT)SelectObject(hdcMem, font);
		SetBkMode(hdcMem, TRANSPARENT);
	}

	FillRect(hdcMem, &rc, hbrBkGnd);

	for (int i = 0; i < count; ++i) {
		SetTextColor(hdcMem, i == focus ? RGB(255, 0, 0) : RGB(0, 0, 0));

		str = data[i].displayString(true);
		TextOut(hdcMem, x, y, str, strlen(str));

		y += YHOP;
	}

	BitBlt(hdc,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hdcMem,
		0, 0,
		SRCCOPY);

	EndPaint(hWnd, &ps);
}

// =======================================

void Widget::addEntry(
	char* nlegend,
	void* ptr,
	float minValue,
	float maxValue,
	float nDelta,
	WidgetKind nKind,
	bool nShowValue)
{
	if (count >= MAX_WIDGETS) ABORT(-1);

	WidgetData& w = data[count++];
	strcpy_s(w.legend, LEGEND_LENGTH, nlegend);
	w.valuePtr = ptr;
	w.rangex = minValue;
	w.rangey = maxValue;
	w.delta = nDelta;
	w.kind = nKind;
	w.showValue = nShowValue;
}

void Widget::addLegend(char* nlegend) {
	if (count >= MAX_WIDGETS) ABORT(-1);

	WidgetData& w = data[count++];
	strcpy_s(w.legend, LEGEND_LENGTH, nlegend);
	w.kind = klegend;
	w.showValue = false;
}

void Widget::addBoolean(char* nlegend, void* ptr) {
	if (count >= MAX_WIDGETS) ABORT(-1);

	WidgetData& w = data[count++];
	strcpy_s(w.legend, 63, nlegend);
	w.valuePtr = ptr;
	w.kind = kboolean;
	w.showValue = true;
}

void Widget::moveFocus(int direction) {
	if (count < 2) return;
	focus += direction > 0 ? 1 : -1;
	if (focus < 0) focus = count - 1; else if (focus >= count) focus = 0;

	if (data[focus].kind == klegend || data[focus].kind == kboolean) moveFocus(direction);

	refresh();
	fractal.updateWindowTitle();
}

void Widget::jumpFocus() {
	int index = (yPos - YTOP) / YHOP;

	//char str[32];
	//sprintf_s(str,31, "jump %d Index %d\n", yPos,index);
	//OutputDebugStringA(str);

	if (index < 0 || index >= count) return;

	switch (data[index].kind) {
	case kfloat:
	case kinteger:
		focus = index;
		refresh();
		break;
	case kboolean:
		data[index].alterValue(1);
		fractal.defineWidgetsForCurrentEquation(false);
		fractal.isDirty = true;
		break;
	}
}

void Widget::jumpToPreviousFocus() {
	if (previousFocus < 0 || previousFocus >= count) return;

	switch (data[previousFocus].kind) {
	case kfloat:
	case kinteger:
	case kboolean:
		focus = previousFocus;
		refresh();
		break;
	}
}

bool Widget::isAltering() {
	if (alterationDirection == 0) return false;

	if (data[focus].alterValue(alterationDirection)) {
		refresh();
	}

	return true;
}

bool Widget::keyDown(int key) {
	//char str[32];
	//sprintf_s(str,31, "KD %d %c\n", key, key);
	//OutputDebugStringA(str);
	//return;

	switch (key) {
	case VK_LEFT:
		alterationDirection = -1;
		break;
	case VK_RIGHT:
		alterationDirection = +1;
		break;
	case VK_DOWN: moveFocus(+1);
		break;
	case VK_UP: moveFocus(-1);
		break;
	}

	return true;
}

void Widget::keyUp(int key) {
	//char str[32];
	//sprintf_s(str,31, "KU %d %c\n", key, key);
	//OutputDebugStringA(str);
	//return;

	switch (key) {
	case VK_SHIFT:
		fractal.isShiftKeyPressed = false;
		break;
	case VK_CONTROL:
		fractal.isControlKeyPressed = false;
		break;
	case VK_LEFT:
	case VK_RIGHT:
		alterationDirection = 0;
		break;
	}
}

void Widget::reset() {
	count = 0;
	previousFocus = focus;
	focus = 0;
}

void Widget::refresh() { InvalidateRect(hWnd, NULL, TRUE); }
const char* Widget::focusString(bool showV) { return data[focus].displayString(showV); }
