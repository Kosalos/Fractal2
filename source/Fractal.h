#pragma once

#include <DirectXMath.h>

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
	
	bool isDirty;

private:
	void refresh(bool resetFocus);
	void juliaGroup(FLOAT range, FLOAT delta);
	void changeEquationIndex(int dir);

};

extern Fractal fractal;
extern FLOAT alterationSpeed;
extern bool shiftDown;
extern bool controlDown;
