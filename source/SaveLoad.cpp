#include <time.h>
#include "SaveLoad.h"
#include "View.h"
#include "Fractal.h"

SaveLoad saveLoad;

static char* CLASS_NAME = "SaveLoad";

// SaveLoad dialog message handler
LRESULT CALLBACK SaveLoadWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		saveLoad.createListBox(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1: // listbox
			if (HIWORD(wParam) == LBN_SELCHANGE) {
				int index = (int)SendMessageW(saveLoad.hWndList, LB_GETCURSEL, 0, 0);
				saveLoad.loadControl(index);
				return 0;
			}
			break;
		case 2: // Close button
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_DELETE:
			saveLoad.deleteSelectedEntry();
			break;
		case VK_RETURN:
			ShowWindow(saveLoad.hWnd, SW_HIDE);
			break;
		}
		break;
	case WM_ERASEBKGND:
	{
		RECT rc;
		HDC hdc = (HDC)wParam;
		GetClientRect(hWnd, &rc);
		FillRect(hdc, &rc, CreateSolidBrush(RGB(210, 235, 210)));
		return 1L;
	}
	break;
	case WM_DESTROY:
		DeleteObject(saveLoad.font);
		return TRUE;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// create SaveLoad dialog on coldstart, initially hidden
void SaveLoad::create(HWND parent, HINSTANCE hInstance) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = SaveLoadWndProc;
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

	RECT rc2 = { 10, 10, 420,600 };
	AdjustWindowRect(&rc2, WS_OVERLAPPEDWINDOW, FALSE);

	font = CreateFont(20, 8, 0, 0,
		FW_NORMAL,
		FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN,
		"Courier New");

	hWnd = CreateWindow(CLASS_NAME, "SaveLoad", WS_OVERLAPPED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, rc2.right - rc2.left, rc2.bottom - rc2.top, parent, NULL, hInstance, NULL);
	if (hWnd == NULL) ABORT(-1);

	ShowWindow(hWnd, SW_HIDE);
}

// make SaveLoad dialog visible, called when 'L' is pressed
void SaveLoad::launch() {
	fillListBox();
	ShowWindow(hWnd, SW_SHOWNORMAL);
}

// message handler for listbox, so that <Enter> key hides the dialog
static WNDPROC lpfnEditWndProc = NULL;

LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_DELETE:
			saveLoad.deleteSelectedEntry();
			break;
		case VK_RETURN:
			ShowWindow(saveLoad.hWnd, SW_HIDE);
			break;
		}
		break;
	}

	return CallWindowProc(lpfnEditWndProc, hwnd, msg, wParam, lParam);
}

// create the child Listbox for the SaveLoad dialog
void SaveLoad::createListBox(HWND nhWnd) {
	hWnd = nhWnd;

	int style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY;

	hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, "Listbox", "", style, 5, 5, 400, 560, hWnd, (HMENU)1, NULL, NULL);

	CreateWindowEx(NULL, "button", "Close", WS_VISIBLE | WS_CHILD, 10, 560, 60, 20, hWnd, (HMENU)2, NULL, NULL);
	CreateWindowEx(NULL, "static", "     Press <Delete> to remove selected entry", WS_VISIBLE | WS_CHILD, 104, 560, 300, 20, hWnd, (HMENU)3, NULL, NULL);

	SendMessage(hWndList, WM_SETFONT, WPARAM(font), 0);
	lpfnEditWndProc = (WNDPROC)SetWindowLong(hWndList, GWL_WNDPROC, (DWORD)SubClassProc);
}

// search for all Fractal data files in the current folder. Add to class's data storage, and a display rendition to the listbox
void SaveLoad::fillListBox() {
	count = 0;
	SendMessage(hWndList, LB_RESETCONTENT, 0, 0);

	//char str[128];
	WIN32_FIND_DATA fdata;
	HANDLE hFind = FindFirstFile("Fractal*.dat", &fdata);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			//sprintf_s(str,127, "%s\n", fdata.cFileName);
			//OutputDebugStringA(str);

			strcpy_s(data[count], 31, fdata.cFileName);
			SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)displayString(fdata.cFileName));

			if (++count == MAX_ENTRIES) {
				FindClose(hFind);
				return;
			}
		} while (FindNextFile(hFind, &fdata));

		FindClose(hFind);
	}

	if (count == 0) {
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"No save files found.");
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"Press 'S' to save parameter settings to file.");
	}
}

// generate the Fractal filename, given the current time/date and equation#
char* SaveLoad::createFilename() {
	time_t rawtime;
	struct tm timeinfo;
	static char buffer[128];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	// 0123456789a123456789b12
	// Fractal_EE_YYMMDDHHMMSS.dat
	strftime(buffer, 127, "Fractal_xx_%y%m%d%H%M%S.dat", &timeinfo);

	buffer[8] = '0' + EQUATION / 10;
	buffer[9] = '0' + EQUATION % 10;

	return buffer;
}

// user has pressed 'S'. Save current parameter settings to file.
void SaveLoad::saveControl() {
	const char* name = createFilename();
	FILE* fp = NULL;

	if (fopen_s(&fp, name, "wb") != 0) {
		char str[256];
		sprintf_s(str, 255, "Error creating file %s", name);
		MessageBox(NULL, str, "Cannot continue", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	if (fp != NULL) {
		fwrite(&control, sizeof(Control), 1, fp);
		fclose(fp);

		if (IsWindowVisible(hWnd)) fillListBox();
	}

	MessageBox(NULL, "Saved settings to File", "Success", MB_OK);
}

// user has clicked on a listbox entry. Load the parametere settings from file.
void SaveLoad::loadControl(int index) {
	if (index >= count) return;

	const char* name = data[index];
	FILE* fp = NULL;

	if (fopen_s(&fp, name, "rb") != 0) {
		char str[256];
		sprintf_s(str, 255, "Error opening file %s", name);
		MessageBox(NULL, str, "Cannot continue", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	if (fp != NULL) {
		int isStereo = ISSTEREO;	// memorize current settings before they're stomped on
		float parallax = PARALLAX;

		fread(&control, sizeof(Control), 1, fp);
		fclose(fp);

		// some settings should be updated to match current session
		XSIZE = windowWidth;
		YSIZE = windowHeight;
		ISSTEREO = isStereo;
		PARALLAX = parallax;

		fractal.refresh(true);
		fractal.updateWindowTitle();
	}
}

void SaveLoad::deleteSelectedEntry() {
	int index = (int)SendMessageW(saveLoad.hWndList, LB_GETCURSEL, 0, 0);
	if (index < 0 || index >= count) return;

	if (MessageBox(NULL, "Delete the selected entry?", "Confirmation", MB_ICONEXCLAMATION | MB_YESNO) == IDYES) {
		remove(data[index]);
		fillListBox();
	}
}

// extract 2 digit number from portion of passed string
int digits(const char* filename, int index) {
	static char str[128];
	strcpy_s(str, 127, filename);
	str[index + 2] = 0;
	return atoi(&str[index]);
}

// generate display rendtion of Fractal data filename (equation + date/time)
const char* SaveLoad::displayString(const char* filename) {
	static char str[128];

	// 0123456789a123456789b12
	// Fractal_EE_YYMMDDHHMMSS.dat
	int equation = digits(filename, 8);
	int index = 11;
	int year = digits(filename, index);  index += 2;
	int month = digits(filename, index);  index += 2;
	int day = digits(filename, index);  index += 2;
	int hour = digits(filename, index);  index += 2;
	int minute = digits(filename, index);  index += 2;
	int second = digits(filename, index);  index += 2;

	sprintf_s(str, 127, "%2d %-24s %02d/%02d/%2d %02d:%02d:%02d", equation, equationName[equation], day, month, year, hour, minute, second);
	return str;
}
