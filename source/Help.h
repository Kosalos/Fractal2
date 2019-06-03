#pragma once

class Help
{
public:
	HWND hWnd,hWndList;
	HFONT font;

	void create(HWND parent, HINSTANCE inst);
	void launch();
	void addHelptext();
};

extern Help help;
