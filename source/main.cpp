#include "stdafx.h"
#include "View.h"
#include "Fractal.h"
#include "Widget.h"
#include "Help.h"
#include "SaveLoad.h"
#include "WICTextureLoader.h"


HWND g_hWnd = NULL;
ID3D11Device* pd3dDevice = nullptr;
ID3D11DeviceContext* pImmediateContext = nullptr;
static IDXGISwapChain* pSwapChain = nullptr;
static ID3D11RenderTargetView* pRenderTargetView = nullptr;
ID3D11Texture2D* pBackBuffer = NULL;

#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

void windowSizePositionChanged();
void WriteToBmp(const char* inFilePath);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_EXITSIZEMOVE:
		windowSizePositionChanged();
		return 0;
	case WM_IME_NOTIFY:
		return 0;
	case WM_CREATE:
		SetTimer(hWnd, 1, 100, NULL);
		break;

	case WM_TIMER:
		fractal.timer();
		break;

	case WM_KEYDOWN:
		fractal.keyDown(wParam);
		break;
	case WM_KEYUP:
		fractal.keyUp(wParam);
		break;

	case WM_LBUTTONDOWN:
		fractal.lButtonDown(lParam);
		break;
	case WM_LBUTTONUP:
		fractal.lButtonUp();
		break;
	case WM_RBUTTONDOWN:
		fractal.rButtonDown(lParam);
		break;
	case WM_RBUTTONUP:
		fractal.rButtonUp();
		break;
	case WM_MOUSEMOVE:
		fractal.mouseMove(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
	{
		int direction = GET_WHEEL_DELTA_WPARAM(wParam);
		widget.moveFocus(-direction / 120);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

static char* CLASS_NAME = "MainWin";
static char* WINDOW_NAME = "Fractal";

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASS_NAME;
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return E_FAIL;
	}

	windowHeight = (int)::GetSystemMetrics(SM_CYSCREEN) - 100;
	windowWidth = windowHeight; //  (int)::GetSystemMetrics(SM_CXSCREEN);
	RECT rc = { 0, 0, windowWidth,windowHeight };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);
	if (g_hWnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return E_FAIL;
	}
	ShowWindow(g_hWnd, nCmdShow);

	widget.create(g_hWnd, hInstance);
	help.create(g_hWnd, hInstance);
	saveLoad.create(g_hWnd, hInstance);
	return S_OK;
}

HRESULT InitializeD3D11(HWND hWnd) {
	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = windowWidth;
	sd.BufferDesc.Height = windowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	SAFE_RELEASE(pSwapChain);
	SAFE_RELEASE(pd3dDevice);
	SAFE_RELEASE(pImmediateContext);
	SAFE_RELEASE(pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// Create Device, DeviceContext, SwapChain, FeatureLevel
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &pSwapChain, &pd3dDevice, &featureLevel, &pImmediateContext);
		if (SUCCEEDED(hr)) break;
	}
	ABORT(hr);
	
	// Create Render Target View Object from SwapChain's Back Buffer.
	// Access one of swap chain's back buffer.[0-based buffer index, interface type which manipulates buffer, output param]
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
	ABORT(hr);

	hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	ABORT(hr);

	return S_OK;
}

void windowSizePositionChanged() {
	RECT r;
	GetClientRect(g_hWnd, &r);
	int xs = r.right - r.left;
	int ys = r.bottom - r.top;

	if (xs != windowWidth || ys != windowHeight) {
		windowWidth = xs;
		windowHeight = ys;
		XSIZE = xs;
		YSIZE = ys;

		if (TONOFF != 0) { // for some reason, srcTextureView does not survive window resizing
			TONOFF = 0;
			fractal.refresh(false);
		}

		LRESULT ret = InitializeD3D11(g_hWnd);
		ABORT(ret);

		view.Initialize(pd3dDevice, pImmediateContext);
		fractal.isDirty = true;
	}
}

HRESULT Render(ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pTargetView) {
	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pContext->OMSetRenderTargets(1, &pTargetView, NULL);
	pContext->ClearRenderTargetView(pTargetView, ClearColor);
	view.Render(pContext);
	pSwapChain->Present(0, 0);
	return S_OK;
}

// ----------------------------------------------------------------------------------

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE, // hPrevInstance,
	_In_ LPWSTR, // lpCmdLine,
	_In_ int nShowCmd)
{
	LRESULT ret = InitWindow(hInstance, nShowCmd);
	if (ret != S_OK) { exit(-11); }

	ret = InitializeD3D11(g_hWnd);
	if (ret != S_OK) { exit(-12); }

	view.Initialize(pd3dDevice, pImmediateContext);
	fractal.init();

	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			fractal.update();
			Render(pImmediateContext, pRenderTargetView);
		}
	}

	SAFE_RELEASE(pRenderTargetView);
	SAFE_RELEASE(pSwapChain);



	SAFE_RELEASE(pSwapChain);
	SAFE_RELEASE(pd3dDevice);
	SAFE_RELEASE(pImmediateContext);
	SAFE_RELEASE(pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);



	view.Destroy();
	return (int)msg.wParam;
}

// ===============================

void abortProgram(char* name, int line) {
	char str[256];
	sprintf_s(str, 255, "Error in file % s, line % d", name, line);

	MessageBox(NULL, str, "Program Exit", MB_ICONEXCLAMATION | MB_OK);
	exit(-1);
}
