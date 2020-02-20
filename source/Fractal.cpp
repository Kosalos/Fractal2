#include "stdafx.h"
#include <time.h>
#include <iostream>
#include <tchar.h>
#include "Fractal.h"
#include "View.h"
#include "common.h"

Fractal fractal;
Widget pWidget;
Widget cWidget;

extern HWND g_hWnd;

void Fractal::init() {
	jogAmount = XMFLOAT4(0, 0, 0, 0);
	alterationSpeed = 1;
	isShiftKeyPressed = false;
	isControlKeyPressed = false;

	reset();
	isDirty = true;
}

FLOAT length(XMFLOAT4 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

XMFLOAT4 toRectangular(XMFLOAT4 sph) {
	FLOAT ss = sph.x * sin(sph.z);
	return XMFLOAT4(
		ss * cos(sph.y),
		ss * sin(sph.y),
		sph.x * cos(sph.z),
		0);
}

XMFLOAT4 toSpherical(XMFLOAT4 rec) {
	return XMFLOAT4(
		length(rec),
		atan2(rec.y, rec.x),
		atan2(sqrtf(rec.x * rec.x + rec.y * rec.y), rec.z),
		0);
}

void Fractal::updateShaderDirectionVector(XMFLOAT4 v) {
	// control.viewVector = normalize(v)
	XMVECTOR viewVector = XMVector4Normalize(DirectX::XMLoadFloat4(&v));
	DirectX::XMStoreFloat4(&control.viewVector, viewVector);

	control.topVector = toSpherical(control.viewVector);
	control.topVector.z += 1.5708;

	// control.topVector = normalize(toRectangular(control.topVector));
	XMFLOAT4 temp = v;
	temp = toRectangular(control.topVector);
	XMVECTOR topVector = XMVector4Normalize(DirectX::XMLoadFloat4(&temp));
	DirectX::XMStoreFloat4(&control.topVector, topVector);

	XMVECTOR sideVector = XMVector3Cross(viewVector, topVector);
	sideVector = XMVector4Normalize(sideVector) * XMVector4Length(topVector);
	DirectX::XMStoreFloat4(&control.sideVector, sideVector);
}

void Fractal::reset() {
	XSIZE = windowWidth;
	YSIZE = windowHeight;
	updateShaderDirectionVector(XMFLOAT4(0, 0.1, 1, 0));

	control.camera = XMFLOAT4(0.42461035, 10.847559, 2.5749633, 0);
	MAXSTEPS = 12;

	//  Q1 multiplier
	//  Q2 foam
	//  Q3 foam2
	//  Q4 bend
	Q1 = 25.0f;
	Q2 = 1.05265248;
	Q3 = 1.06572711f;
	Q4 = 0.0202780124f;
}

XMFLOAT4 add4(XMFLOAT4 v1, XMFLOAT4 v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
	return v1;
}

XMFLOAT4 sub4(XMFLOAT4 v1, XMFLOAT4 v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
	return v1;
}

XMFLOAT4 mult4(XMFLOAT4 v1, float v2) {
	v1.x *= v2;
	v1.y *= v2;
	v1.z *= v2;
	v1.w *= v2;
	return v1;
}

XMFLOAT4 recipical4(XMFLOAT4 v) {
	if (v.x != 0) v.x = 1.0 / v.x;
	if (v.y != 0) v.y = 1.0 / v.y;
	if (v.z != 0) v.z = 1.0 / v.z;
	if (v.w != 0) v.w = 1.0 / v.w;
	return v;
}

XMFLOAT4 normalize4(XMFLOAT4 v) {
	float t = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	if (t != 0) {
		v.x /= t;
		v.y /= t;
		v.z /= t;
		v.w /= t;
	}

	return v;
}

void Fractal::update() {
	if (isDirty) {
		isDirty = false;
		view.UpdateControlBuffer();
		view.Compute();
	}
}

bool alternateJogMode = false; // 'A' key pressed while mouse jogging? = XZ movement rather than XY
bool rotateMode = false; // 'Z' key pressed while jogging? = rotate camera rather than move it.

void Fractal::moveCamera(XMFLOAT4 amt) {
	if (rotateMode) {
		updateShaderDirectionVector(add4(control.viewVector, amt));
	}
	else {
		control.camera = sub4(control.camera, mult4(control.sideVector, amt.x));
		control.camera = sub4(control.camera, mult4(control.topVector, amt.y));
		control.camera = add4(control.camera, mult4(control.viewVector, amt.z));
	}
}

void Fractal::timer() {
	// mouse dragging to move/rotate camera
	mouseTimerHandler();

	// number keys pressed to move/rotate camera
	if (jogAmount.x != 0 || jogAmount.y != 0 || jogAmount.z != 0) {
		isDirty = true;
		moveCamera(jogAmount);
	}
}

void Fractal::updateAlterationSpeed() {
	alterationSpeed = 1.0;

	if (isShiftKeyPressed && isControlKeyPressed) alterationSpeed *= 50; else
		if (isShiftKeyPressed) alterationSpeed *= 0.1; else
			if (isControlKeyPressed) alterationSpeed *= 10;
}

void Fractal::jogCameraAndFocusPosition(int dx, int dy, int dz) {
	updateAlterationSpeed();
	float speed = alterationSpeed * 0.01;

	if (dx != 0) { jogAmount.x = FLOAT(dx) * speed; }
	if (dy != 0) { jogAmount.y = FLOAT(dy) * speed; }
	if (dz != 0) { jogAmount.z = FLOAT(dz) * speed; }
}

void Fractal::jogRelease(int dx, int dy, int dz) {
	if (dx != 0) { jogAmount.x = 0; }
	if (dy != 0) { jogAmount.y = 0; }
	if (dz != 0) { jogAmount.z = 0; }
}

extern ID3D11Device* pd3dDevice;
extern ID3D11DeviceContext* pImmediateContext;
ID3D11Texture2D* srcTexture = NULL;
ID3D11ShaderResourceView* srcTextureView = NULL;

void Fractal::keyDown(int key) {
	//char str[32];
	//sprintf_s(str,31, "KD %d %c\n", key, key);
	//OutputDebugStringA(str);

	switch (key) {
	case VK_ESCAPE:
		SendMessage(g_hWnd, WM_CLOSE, 0, 0);
		SendMessage(cWidget.hWnd, WM_CLOSE, 0, 0);
		break;
	case VK_SHIFT:
		isShiftKeyPressed = true;
		return;
	case VK_CONTROL:
		isControlKeyPressed = true;
		return;
	}

	switch (tolower(key)) {
	case '1':   alterMaxSteps(-1);		break;
	case '2':   alterMaxSteps(+1);		break;
	case '3':	reset(); isDirty = true;  break;
	case ' ':   break;
	case 'q':	alterParameter(&Q1, -1, 10, 300, 0.2);	break;
	case 'w':	alterParameter(&Q1, +1, 10, 300, 0.2);	break;
	case 'a':	alterParameter(&Q2, -1, 0.1, 3, 0.02);	break;
	case 's':	alterParameter(&Q2, +1, 0.1, 3, 0.02);	break;
	case 'z':	alterParameter(&Q3, -1, 0.1, 3, 0.02);	break;
	case 'x':	alterParameter(&Q3, +1, 0.1, 3, 0.02);	break;
	case 'e':   alterParameter(&Q4, -1, 0.01, 0.03, 0.0001);	break;
	case 'r':   alterParameter(&Q4, +1, 0.01, 0.03, 0.0001);	break;

	case '4':	jogCameraAndFocusPosition(-1, 0, 0);	break;
	case '5':	jogCameraAndFocusPosition(+1, 0, 0);	break;
	case '6':	jogCameraAndFocusPosition(0, -1, 0);	break;
	case '7':	jogCameraAndFocusPosition(0, +1, 0);	break;
	case '8':	jogCameraAndFocusPosition(0, 0, -1);	break;
	case '9':	jogCameraAndFocusPosition(0, 0, +1);	break;
	}
}

void Fractal::keyUp(int key) {
	//char str[32];
	//sprintf_s(str,31, "       KU %d %c\n", key, key);
	//OutputDebugStringA(str);

	switch (tolower(key)) {
	case VK_SHIFT:
		isShiftKeyPressed = false;
		break;
	case VK_CONTROL:
		isControlKeyPressed = false;
		break;

	case '4':	jogRelease(-1, 0, 0);	break;
	case '5':	jogRelease(+1, 0, 0);	break;
	case '6':	jogRelease(0, -1, 0);	break;
	case '7':	jogRelease(0, +1, 0);	break;
	case '8':	jogRelease(0, 0, -1);	break;
	case '9':	jogRelease(0, 0, +1);	break;
	}
}

// ==========================================================

POINTS mouseJoggingStartPos, mouseJoggingCurrentPos;
POINTS mouseAlteringStartPos, mouseAlteringCurrentPos;
bool isMouseJogging = false;

void Fractal::lButtonDown(LPARAM lParam) {
	mouseJoggingStartPos = MAKEPOINTS(lParam);
	mouseJoggingCurrentPos = mouseJoggingStartPos;
	isMouseJogging = true;
}

void Fractal::lButtonUp() {
	isMouseJogging = false;
}

void Fractal::mouseMove(WPARAM wParam, LPARAM lParam) {
	if (isMouseJogging && (wParam & MK_LBUTTON))
		mouseJoggingCurrentPos = MAKEPOINTS(lParam);
	else
		isMouseJogging = false;
}

void Fractal::mouseTimerHandler() {
	if (isMouseJogging) {
		XMFLOAT4 amt = XMFLOAT4(0, 0, 0, 0);
		float scale = 0.001;
		amt.x = float(mouseJoggingCurrentPos.x - mouseJoggingStartPos.x) * scale;
		amt.y = -float(mouseJoggingCurrentPos.y - mouseJoggingStartPos.y) * scale;

		moveCamera(amt);
		isDirty = true;
	}
}

void Fractal::alterMaxSteps(int dir) {
	MAXSTEPS += dir;
	if (MAXSTEPS < 2) MAXSTEPS = 2; else
	if (MAXSTEPS > 30) MAXSTEPS = 30;
	isDirty = true;
}

void Fractal::alterParameter(float* var, float dir, float min, float max, float delta) {
	float oldValue = *var;
	float d = delta;
	if (isShiftKeyPressed && isControlKeyPressed) d *= 100; else {
		if (isShiftKeyPressed) d *= 0.1;
		if (isControlKeyPressed) d *= 10;
	}

	*var += d * dir;
	if (*var < min) *var = min; else if (*var > max) *var = max;

	if (*var != oldValue) isDirty = true;
}

