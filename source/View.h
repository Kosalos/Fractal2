#pragma once
#include "common.h"

class View
{
public:
	void	Initialize(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext);
	void	Render(ID3D11DeviceContext* pImmediateContext );
	void	Destroy();
	void    UpdateControlBuffer();
	void	Compute();
	void    CreateTextureViews();
	void	DestroyTextures();

	ID3D11Device*				device;
	ID3D11DeviceContext*		context;

	ID3D11VertexShader*			vShader;
	ID3D11InputLayout*			vLayout;
	ID3D11Buffer*				vBuffer;

	ID3D11PixelShader*			pShader;
	ID3D11SamplerState*			pSampler;

	ID3D11Texture2D*			texture;
	ID3D11ShaderResourceView*	textureView;

	ID3D11Buffer*               controlBuffer;
	ID3D11ComputeShader*		cShader;
    ID3D11Texture2D*            cTexture;
    ID3D11UnorderedAccessView*  cTextureView;
};

extern Control control;
extern View view;
extern int windowWidth, windowHeight;
