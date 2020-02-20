#pragma once

#include "stdafx.h"
#include "View.h"	

class Widget
{
public:
	HWND hWnd;
	HDC hdc;
	HFONT font;
	RECT rc;

	Widget() {
		hWnd = NULL;
		hdc = NULL;
		font = NULL;
		rc.left = 0;
	}

	void drawWindow();
	void create(char* className, char *title,HWND parent, HINSTANCE inst);
	void destroy();
};
