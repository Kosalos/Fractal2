#include "common.h"
#include "View.h"

void WriteToBmp(const char* inFilePath)
{
	FILE* fp = NULL;

	errno_t result = fopen_s(&fp, inFilePath, "wb");
	if (result != 0 || fp == NULL) {
		MessageBox(NULL, "Error creating file", "Cannot continue", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

#pragma pack(push, 2)

	struct BMPHeader
	{
		UINT16  m_id;
		UINT32  m_fileSize;
		UINT32  m_unused;
		UINT32  m_pixelArrayOffset;
	};

	struct DIBHeader
	{
		UINT32  m_dibHeaderSize;
		UINT32  m_widthPixels;
		UINT32  m_heightPixels;
		UINT16  m_numPlanes;
		UINT16  m_bitsPerPixel;
		UINT32  m_compressionMethod;
		UINT32  m_pixelDataSize;
		UINT32  m_pixelsPerMeterHorizontal;
		UINT32  m_pixelsPerMeterVertical;
		UINT32  m_colorsInPalette;
		UINT32  m_importantColors;
	};

#pragma pack(pop)

	D3D11_TEXTURE2D_DESC desc;
	view.texture->GetDesc(&desc);

	UINT32 pixelWidth = ((desc.Width + 15) / 16) * 16;
	UINT32 pixelHeight = desc.Height;	
	UINT32 byteWidthNoPadding = pixelWidth * 4;
	UINT32 byteWidth = (byteWidthNoPadding + 3) & ~3;
	UINT32 bytePadding = byteWidth - byteWidthNoPadding;
	UINT32 pixelDataSize = byteWidth * pixelHeight;

	BMPHeader bmpHeader;
	bmpHeader.m_id = 0x4D42;
	bmpHeader.m_fileSize = sizeof(BMPHeader) + sizeof(DIBHeader) + pixelDataSize;
	bmpHeader.m_pixelArrayOffset = sizeof(BMPHeader) + sizeof(DIBHeader);
	bmpHeader.m_unused = 0;

	DIBHeader dibHeader;
	dibHeader.m_bitsPerPixel = 24;
	dibHeader.m_colorsInPalette = 0;
	dibHeader.m_compressionMethod = 0;
	dibHeader.m_dibHeaderSize = sizeof(DIBHeader);
	dibHeader.m_heightPixels = pixelHeight;
	dibHeader.m_importantColors = 0;
	dibHeader.m_numPlanes = 1;
	dibHeader.m_pixelDataSize = pixelDataSize;
	dibHeader.m_pixelsPerMeterHorizontal = 2835;
	dibHeader.m_pixelsPerMeterVertical = 2835;
	dibHeader.m_widthPixels = pixelWidth;

	fwrite(&bmpHeader, sizeof(bmpHeader), 1, fp);
	fwrite(&dibHeader, sizeof(dibHeader), 1, fp);

	view.context->CopyResource(view.bmpTexture, view.texture);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hr = view.context->Map(view.bmpTexture, 0, D3D11_MAP_READ, 0, &mappedSubresource);
	ABORT(hr);

	UINT8* pData = reinterpret_cast<UINT8*>(mappedSubresource.pData);

	DWORD padding = 0;
	for (UINT32 row = 0; row < pixelHeight; row++) {
		UINT8* pRow = pData + ((pixelHeight - row - 1) * (pixelWidth * 4));

		for (UINT32 col = 0; col < pixelWidth; col++) {
			fwrite(pRow + 2, 1, 1, fp);
			fwrite(pRow + 1, 1, 1, fp);
			fwrite(pRow + 0, 1, 1, fp);

			pRow += 4;
		}
		if (bytePadding) fwrite(&padding, 1, bytePadding, fp);
	}

	view.context->Unmap(view.bmpTexture, 0);
	fclose(fp);
}
