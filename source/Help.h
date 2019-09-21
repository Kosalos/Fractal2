#pragma once

class Help
{
public:
	HWND hWnd,hWndList;
	HFONT font;

	void create(HWND parent, HINSTANCE inst);
	void launch(int who);
	void addHelptext();
};

extern Help help;
