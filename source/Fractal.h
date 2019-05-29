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
	void jogCameraAndFocusPosition(int dx, int dy, int dz);
	void jogRelease(int dx, int dy, int dz);
	void updateShaderDirectionVector(XMFLOAT4 v);
	void defineWidgetsForCurrentEquation(bool resetFocus);
	void updateWindowTitle();

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
