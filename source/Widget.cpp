#include "stdafx.h"
#include "Widget.h"
#include "Fractal.h"
#include "Help.h"
#include "ColorMap.h"

// ----------------------------------------------------------------------

bool WidgetData::alterValue(int direction, float speed) {
	switch (kind) {
	case kfloat:
	{
		float value = *(float*)valuePtr;
		float oldValue = value;
		float amt = delta * speed;

		//char str[128];
		//sprintf_s(str, 127, "alt%8.6f  amt %8.6f\n", speed, amt);
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
		int amt = int(delta * speed);
		if (amt == 0) amt = 1;

		//char str[128];
		//sprintf_s(str, 127, "alt%8.6f  amt %d\n", speed, amt);
		//OutputDebugString(str);

		value += direction * amt;
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

#define HELP_BUTTON   2000
#define RESET_BUTTON  2001

LRESULT CALLBACK WidgetWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Widget* ptr = NULL;

	if (hWnd == pWidget.hWnd) {
		ptr = &pWidget;
	}
	if (hWnd == cWidget.hWnd) {
		ptr = &cWidget;
	}

	switch (message) {
	case WM_PAINT:
		if (ptr != NULL) ptr->drawWindow();
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
		if (ptr != NULL) ptr->mouseDown(lParam);
		break;
	case WM_MOUSEMOVE:
		if (ptr != NULL) ptr->mouseMove(lParam);
		break;
	case WM_LBUTTONUP:
		if (ptr != NULL) ptr->mouseUp();
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == HELP_BUTTON)
			if (ptr != NULL) help.launch(ptr->ident);
		if (LOWORD(wParam) == RESET_BUTTON) {
			fractal.resetColors();
			fractal.isDirty = true;
		}
		SetFocus(hWnd);
		break;
	case WM_MOUSEWHEEL:
	{
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		if (ptr != NULL) ptr->moveFocus(-direction / 120);
	}
	break;
	case WM_SETFOCUS:
		if (ptr != NULL) ptr->gainFocus();
		break;
	case WM_KILLFOCUS:
		if (ptr != NULL) ptr->loseFocus();
		break;
	case WM_DESTROY:
		if (ptr != NULL) ptr->destroy();
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Widget::create(int id, char* className, char* title, HWND parent, HINSTANCE hInstance, int height) {
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

	RECT rc2 = { 100, 100, 100 + 280,100 + height };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(className, title, WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT,
		rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox(NULL, "Widget Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	ident = id;

	if (ident == PWIDGETS)
		CreateWindow(TEXT("button"), TEXT("Help"), WS_CHILD | WS_VISIBLE, 110, height - 25, 65, 20, hWnd, (HMENU)HELP_BUTTON, 0, 0);
	if (ident == CWIDGETS) {
		CreateWindow(TEXT("button"), TEXT("Reset"), WS_CHILD | WS_VISIBLE, 30, height - 25, 65, 20, hWnd, (HMENU)RESET_BUTTON, 0, 0);
		CreateWindow(TEXT("button"), TEXT("Help"), WS_CHILD | WS_VISIBLE, 180, height - 25, 65, 20, hWnd, (HMENU)HELP_BUTTON, 0, 0);
	}

	isVisible = true;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetFocus(hWnd);

	font = CreateFont(16, 8, 0, 0,
		FW_NORMAL,
		FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN,
		"Courier New");
}

void Widget::destroy() {
	DeleteObject(font);
	DeleteObject(hdcMem);
	DeleteObject(hbmMem);
	DeleteObject(hbmOld);
	DeleteObject(hbrBkGnd);
	DeleteObject(hfntOld);
}

void Widget::toggleVisible() {
	ShowWindow(hWnd, isVisible ? SW_HIDE : SW_SHOWNORMAL);
	isVisible = !isVisible;
}

#define YTOP 10
#define YHOP 14

int findXColorRow() {
	for (int i = 0; i < cWidget.count; ++i) {
		if (!strcmp(cWidget.data[i].legend, "X Color"))
			return YTOP + i * YHOP + 1;
	}

	return 400;
}

void Widget::colorSwatch(int colorIndex, RECT& r) {
	XMFLOAT3* cMap = colorMapList[paletteIndex];
	XMFLOAT3 color = cMap[colorIndex];
	HBRUSH br = CreateSolidBrush(RGB(color.x * 255, color.y * 255, color.z * 255));
	FillRect(hdcMem, &r, br);
	DeleteObject(br);

	HBRUSH wp = CreateSolidBrush(RGB(0, 0, 0));
	FrameRect(hdcMem, &r, wp);
	DeleteObject(wp);

	r.top += YHOP;
	r.bottom += YHOP;
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
		hbrBkGnd = CreateSolidBrush(RGB(230, 230, 230));
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

	// add colored rectangles to Colors dialog
	if (ident == CWIDGETS) {
		x = 235;
		y = findXColorRow();
		RECT rect = { x,y, x + 35, y + YHOP - 2 };

		colorSwatch(OTindexX, rect);
		colorSwatch(OTindexY, rect);
		colorSwatch(OTindexZ, rect);
		colorSwatch(OTindexR, rect);
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
	if (count < 2 || focus == INACTIVE) return;
	focus += direction > 0 ? 1 : -1;
	if (focus < 0) focus = count - 1; else if (focus >= count) focus = 0;

	if (data[focus].kind == klegend || data[focus].kind == kboolean) moveFocus(direction);

	refresh();
	fractal.updateWindowTitle();
}

void Widget::jumpFocus() {
	int index = (pt.y - YTOP) / YHOP;

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
		data[index].alterValue(1, 1);
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
	if (focus == INACTIVE || alterationDirection == 0)
		return false;

	if (data[focus].alterValue(alterationDirection, alterationSpeed))
		refresh();

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

// ================================================

void Widget::mouseDown(LPARAM lp) {
	pt.x = GET_X_LPARAM(lp);
	pt.y = GET_Y_LPARAM(lp);
	jumpFocus();

	if (focus != INACTIVE && (data[focus].kind == kfloat || data[focus].kind == kinteger))
		isMouseDown = true;
}

void Widget::mouseMove(LPARAM lp) {
	if (isMouseDown) {
		int nx = GET_X_LPARAM(lp);

		alterationDirection = (nx < pt.x) ? -1 : 1;
		alterationSpeed = float(abs(nx - pt.x)) / 100.0;
	}
}

void Widget::mouseUp() {
	isMouseDown = false;
	alterationDirection = 0;
}

// ================================================

void Widget::reset() {
	count = 0;
	previousFocus = focus;
	if (focus != INACTIVE) focus = 0;
}

void Widget::refresh() { InvalidateRect(hWnd, NULL, TRUE); }
const char* Widget::focusString(bool showV) { return data[focus].displayString(showV); }
