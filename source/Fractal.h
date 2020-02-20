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
	void lButtonDown(LPARAM lParam);
	void lButtonUp();
	void mouseMove(WPARAM wParam, LPARAM lParam);
	void mouseTimerHandler();

	bool isDirty;
	float alterationSpeed;
	bool isShiftKeyPressed;
	bool isControlKeyPressed;
	XMFLOAT4 jogAmount;
	
	void alterMaxSteps(int dir);
	void alterParameter(float* var, float dir, float min, float max, float delta);
};

extern Fractal fractal;
extern Widget cWidget;
