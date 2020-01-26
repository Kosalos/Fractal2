#pragma once

#include "stdafx.h"
#include "View.h"	

enum WidgetKind { kinteger, kfloat, klegend, kboolean };

#define MAX_WIDGETS 50
#define LEGEND_LENGTH 63
#define INACTIVE -1
#define PWIDGETS 0
#define CWIDGETS 1

struct WidgetData {
	WidgetKind kind = kfloat;
	char legend[LEGEND_LENGTH + 1] = { 0 };
	void* valuePtr = NULL;
	float delta = 0;
	float rangex = 0, rangey = 0;
	bool showValue = false;

	bool alterValue(int direction, float speed);
	char* valueString();
	char* displayString(bool showV);
};

// =====================================================================

class Widget
{
public:
	HWND hWnd;
	HDC hdc;
	HFONT font;

	int ident;
	HDC hdcMem;
	HBITMAP hbmMem, hbmOld;
	HBRUSH hbrBkGnd;
	HFONT hfntOld;
	RECT rc;

	WidgetData data[MAX_WIDGETS];
	int count;
	int focus;
	bool isVisible;
	int alterationDirection;
	float alterationSpeed;
	int previousFocus;
	POINT pt;
	bool isMouseDown;
	
	Widget() {
		ident = 0;
		hWnd = NULL;
		hdc = NULL;
		font = NULL;
		hdcMem = NULL;
		hbmMem = NULL;
		hbmOld = NULL;
		hbrBkGnd = NULL;
		hfntOld = NULL;
		rc.left = 0;
		count = 0;
		isVisible = false;
		alterationDirection = 0;
		alterationSpeed = 0;
		pt.x = 0; pt.y = 0;
		isMouseDown = false;
		focus = INACTIVE;
		previousFocus = 0;
	}

	void create(int id,char* className, char *title,HWND parent, HINSTANCE inst, int height);
	void destroy();

	void loseFocus() {
		if (focus != INACTIVE) {
			previousFocus = focus;
			focus = INACTIVE;
		}

		refresh();
	}

	void gainFocus() {
		if(focus == INACTIVE) focus = max(previousFocus, 0);
		SetForegroundWindow(hWnd);
		refresh();
	}

	void updateWindowFocus() {
		PostMessage(hWnd, (focus == INACTIVE) ? WM_SETFOCUS : WM_KILLFOCUS, 0, 0);
	}

	void addEntry(
		char* nlegend,
		void* ptr,
		float minValue,
		float maxValue,
		float nDelta,
		WidgetKind nKind,
		bool nShowValue = false);

	void reset();
	void addLegend(char* nlegend);
	void addBoolean(char* nlegend, void* ptr);
	bool keyDown(int key);
	void keyUp(int key);
	void moveFocus(int direction);
	void jumpFocus();
	void jumpToPreviousFocus();
	void refresh();
	void drawWindow();
	void toggleVisible();
	bool isAltering();
	const char* focusString(bool showV);
	void mouseDown(LPARAM pt);
	void mouseMove(LPARAM pt);
	void mouseUp();
	void colorSwatch(int colorIndex, RECT& r);
};
