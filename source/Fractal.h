#pragma once

#include <DirectXMath.h>
#include "Widget.h"

using namespace DirectX;

class Fractal
{
public:
	void init();
	void update();
	void timer();
	void keyDown(int key);
	void keyUp(int key);
	void reset();
	void updateAlterationSpeed();

	void moveCamera(XMFLOAT4 amt);
	void jogCameraAndFocusPosition(int dx, int dy, int dz);
	void jogRelease(int dx, int dy, int dz);
	void updateShaderDirectionVector(XMFLOAT4 v);

	void defineWidgetsForCurrentEquation(bool resetFocus);
	void updateWindowTitle();

	void lButtonDown(LPARAM lParam);
	void lButtonUp();
	void rButtonDown(LPARAM lParam);
	void rButtonUp();
	void mouseMove(WPARAM wParam, LPARAM lParam);
	void mouseTimerHandler();
	void resetColors();
	void cycleFocus(bool setToPwidget = false);

	void refresh(bool resetFocus);
	bool isDirty;
	float alterationSpeed;
	bool isShiftKeyPressed;
	bool isControlKeyPressed;
	
private:
	XMFLOAT4 jogAmount;
	float lightAngle;

	void juliaGroup(FLOAT range, FLOAT delta);
	void changeEquationIndex(int dir);
	void saveImageToFile();
	void toggleTexture();
};

extern Fractal fractal;
extern const char* equationName[];

extern Widget pWidget;
extern Widget cWidget;
