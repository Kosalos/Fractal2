#include "stdafx.h"
#include "View.h"
#include "cShader.h"
#include "vShader.h"
#include "pShader.h"

Control control;
View view;
int windowWidth;
int windowHeight;

struct SimpleVertex {
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

void View::Initialize(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext) {
	device = pd3dDevice;
	context = pImmediateContext;

	CreateTextureViews();

	SafeRelease(&cShader);
	SafeRelease(&vShader);
	SafeRelease(&pShader);
	SafeRelease(&vLayout);
	SafeRelease(&vBuffer);
	SafeRelease(&pSampler);
	SafeRelease(&controlBuffer);

	// =========================================================================
	// shaders loaded from HLSL generated header files
	HRESULT hr = device->CreateComputeShader(cShaderSource, sizeof(cShaderSource), nullptr, &cShader);
	ABORT(hr);
	hr = device->CreateVertexShader(vShaderSource, sizeof(vShaderSource), nullptr, &vShader);
	ABORT(hr);
	hr = device->CreatePixelShader(pShaderSource, sizeof(pShaderSource), nullptr, &pShader);
	ABORT(hr);

	// =========================================================================
	// vLayout = vertex shader input data layout definition
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = device->CreateInputLayout(layout, 2, vShaderSource, sizeof(vShaderSource), &vLayout);
	ABORT(hr);

	// =========================================================================
	// vBuffer = triangle strip vertices buffer
	SimpleVertex vertices[] = {
		{  XMFLOAT3(-1.0f,-1.0f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
		{  XMFLOAT3(-1.0f, 1.0f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
		{  XMFLOAT3(1.0f,-1.0f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{  XMFLOAT3(1.0f, 1.0f, 0.5f), XMFLOAT2(1.0f, 0.0f) }
	};

	D3D11_SUBRESOURCE_DATA InitData = { 0 };
	InitData.pSysMem = vertices;

	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, &InitData, &vBuffer);
	ABORT(hr);

	// =========================================================================
	// pSampler = pixel shader texture sampler definition
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0; 
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sd, &pSampler);
	ABORT(hr);

	// =========================================================================
	// controlBuffer = constants buffer for compute shader
	D3D11_BUFFER_DESC dc = { 0 };
	dc.ByteWidth = sizeof(Control);
	dc.Usage = D3D11_USAGE_DYNAMIC;
	dc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InitData = { 0 };
	InitData.pSysMem = &control;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&dc, &InitData, &controlBuffer);
	ABORT(hr);
		
	context->CSSetConstantBuffers(0, 1, &controlBuffer);
}

extern ID3D11Texture2D* srcTexture;
extern ID3D11ShaderResourceView* srcTextureView;

void View::Compute() {
	context->CSSetShader(cShader, NULL, 0);
	if(TONOFF != 0 && srcTextureView != NULL)
		context->CSSetShaderResources(0, 1, &srcTextureView);
	context->CSSetUnorderedAccessViews(0, 1, &cTextureView, NULL);
	context->CSSetConstantBuffers(0, 1, &controlBuffer);
	context->CSSetSamplers(0, 1, &pSampler);

	int sz = 12;  // thread count
	context->Dispatch((windowWidth + sz - 1) / sz, (windowHeight + sz - 1) / sz, 1);

	context->CopyResource(texture, cTexture);

	// reset resources after shader use ------------------
	static ID3D11UnorderedAccessView* ppUAViewNULL[2] = { NULL, NULL };
	static ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
	context->CSSetShader(NULL, NULL, 0);
	context->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);
	context->CSSetShaderResources(0, 1, ppSRVNULL);
}

void View::Render(ID3D11DeviceContext* pImmediateContext) {
	assert(pImmediateContext == context);

	UINT offset = 0, stride = sizeof(SimpleVertex);
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	context->IASetInputLayout(vLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->VSSetShader(vShader, NULL, 0);

	context->PSSetShaderResources(0, 1, &textureView);
	context->PSSetShader(pShader, NULL, 0);
	context->PSSetSamplers(0, 1, &pSampler);

	// viewport --------
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)windowWidth;
	vp.Height = (FLOAT)windowHeight;
	vp.MinDepth = 0; 
	vp.MaxDepth = 1;
	context->RSSetViewports(1, &vp);

	context->Draw(4, 0);
}

// =======================================================================

void View::UpdateControlBuffer() {
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };

	context->Map(controlBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	auto ptr = reinterpret_cast<Control*>(MappedResource.pData);
	*ptr = control;

	context->Unmap(controlBuffer, 0);
}

// =======================================================================

void View::DestroyTextures() {
	SafeRelease(&texture);
	SafeRelease(&cTexture);
	SafeRelease(&bmpTexture);
	SafeRelease(&textureView);
	SafeRelease(&cTextureView);
}

void View::Destroy() {
	SafeRelease(&srcTexture);
	SafeRelease(&srcTextureView);
	SafeRelease(&vShader);
	SafeRelease(&vLayout);
	SafeRelease(&vBuffer);
	SafeRelease(&pShader);
	SafeRelease(&pSampler);
	SafeRelease(&controlBuffer);
	SafeRelease(&cShader);
	DestroyTextures();
}

void View::CreateTextureViews() {
	DestroyTextures();

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = windowWidth;
	desc.Height = windowHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0.3;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	// rendering texture
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (FAILED(device->CreateTexture2D(&desc, NULL, &texture))) ABORT(-1);

	// compute shader texture
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	if (FAILED(device->CreateTexture2D(&desc, NULL, &cTexture))) ABORT(-1);

	// save to .BMP file texture
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	if (FAILED(device->CreateTexture2D(&desc, NULL, &bmpTexture))) ABORT(-1);

	// --------------------------------------------------------------------
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;
	if (FAILED(device->CreateShaderResourceView(texture, &viewDesc, &textureView))) ABORT(-1);

	// --------------------------------------------------------------------
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descView.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descView.Texture2D = { 0 };
	if (FAILED(device->CreateUnorderedAccessView(cTexture, &descView, &cTextureView))) ABORT(-1);
}
