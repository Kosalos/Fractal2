#pragma once

#include "stdafx.h"
#include "Fractal.h"
#include "View.h"	

enum WidgetKind { kinteger, kfloat, klegend, kboolean };
#define MAX_WIDGETS 32
#define LEGEND_LENGTH 63

struct WidgetData {
	WidgetKind kind = kfloat;
	char legend[LEGEND_LENGTH + 1] = { 0 };
	void* valuePtr = NULL;
	float delta = 0;
	float rangex = 0, rangey = 0;
	bool showValue = false;

	bool alterValue(int direction);
	char* valueString();
	char* displayString();
};

// =====================================================================

class Widget
{
public:
	HWND hWnd;
	HDC hdc;
	HFONT font;

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
	
	Widget() {
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
		focus = 0;
		isVisible = false;
		alterationDirection = 0;
	}

	void create(HWND parent, HINSTANCE inst);

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
	const char* focusString();
};

extern Widget widget;
