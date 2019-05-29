#pragma once

class Help
{
public:
	HWND hWnd,hWndList;

	void create(HWND parent, HINSTANCE inst);
	void launch();
	void addHelptext();
};

extern Help help;
