#pragma once
#include "APIDeffinition.h"
#include <string>
#include <vector>
#include <iostream>

#ifdef DIRECT_X
//DirectX
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D2d1.lib")
#pragma comment(lib,"Dwrite.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <dxgi1_3.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <d2d1.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <d3dcompiler.h> 

//windows
#include <windows.h>
#endif


#include "../Utilities/OutputHandler.h"
#include "TextDescription.h"

//B.M
//
//Creates and managers interface with graphics api
//

class Graphics
{
public:
	Graphics();
	~Graphics();

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	Graphics(Graphics&&) = delete;
	Graphics& operator=(const Graphics&&) = delete;

	void Initialise(int x, int y, const std::string& windowName);//Creates window and directX context
	int Width()const { return _width; }
	int Height()const { return _height; }
	void SwapBuffer()const;
	void RenderText(const TextDescription& td);
	void RenderTextBatch();
	HWND GetWindow() const { return _hWnd; }

	ID3D11SamplerState* DefaultSampleState() const { return _defaultSampleState; }

private:
	int						_width = 0;
	int						_height = 0;
	std::vector<TextDescription> _deferredTextBatch;
	float _fontSize;
#ifdef DIRECT_X

public:
	ID3D11Device3* Device3D() const { return _device3D; }
	ID3D11DeviceContext3* DeviceContext3D() const { return _deviceContext3D; }
	ID2D1Device2* Device2D() const { return _device2D; }
	ID2D1DeviceContext2* DeviceContext2D() const { return _deviceContext2D; }
	ID3D11Texture2D* GetBackBuffer() const;//Get texture reference to the back buffer
	HRESULT CompileShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, ID3DBlob** errorBlob);
	void SetUp2DRenderTarget();
	ID3D11SamplerState* _defaultSampleState;

	ID3D11RasterizerState* GetRasterState() const { return m_rasterState; };
	void SetRasterState(ID3D11RasterizerState* rasterState) { m_rasterState = rasterState; };

	template<class DataType>
	ID3D11Buffer* CreateBuffer(std::vector<DataType> data, D3D11_USAGE usage, unsigned int bindFlags, unsigned int cpuAccessFlags);
	ID3D11Buffer* CreateEmptyBuffer(unsigned size, D3D11_USAGE usage, unsigned bindFlags, unsigned cpuAccessFlags) const;
private:
	void InitWindow(const std::string& windowName);
	void InitDirectX();
	void CleanUp() const;

	HINSTANCE				_hInstance = nullptr;
	HWND					_hWnd = nullptr;

	D3D_DRIVER_TYPE			_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device3*           _device3D = nullptr;
	ID3D11DeviceContext3*    _deviceContext3D = nullptr;
	IDXGISwapChain3*         _swapChain = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;

	ID2D1Factory3* _factory2D = nullptr;
	ID2D1Device2*			_device2D = nullptr;
	ID2D1DeviceContext2*		_deviceContext2D = nullptr;
	ID2D1Bitmap1*			_2DTarget = nullptr;

	IDWriteFactory3*		_writeFactory = nullptr;
	IDWriteTextFormat*		_textFormat = nullptr;
	ID2D1DrawingStateBlock1* _stateBlock = nullptr;
	//IWICImagingFactory2*		_wicFactory = nullptr;
#endif
};

#ifdef DIRECT_X
template <class DataType>
ID3D11Buffer* Graphics::CreateBuffer(std::vector<DataType> data, D3D11_USAGE usage, unsigned bindFlags, unsigned cpuAccessFlags)
{
	//Buffer Setup informed from example tutorials from https://blogs.msdn.microsoft.com/chuckw/2013/09/20/directx-sdk-samples-catalog/
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = sizeof(DataType) * data.size();
	bd.BindFlags = bindFlags;
	bd.CPUAccessFlags = cpuAccessFlags;
	ID3D11Buffer* buffer;
	HRESULT hr;
	if (!data.empty())
	{
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &data[0];
		hr = _device3D->CreateBuffer(&bd, &InitData, &buffer);
	}
	else
	{
		hr = _device3D->CreateBuffer(&bd, nullptr, &buffer);
	}
	if (FAILED(hr))
	{
		std::cout << "Whoops, failed to create buffer ";
		return nullptr;
		//throw ErrorHandler("Whoops, failed to create buffer ", __LINE__, __FILE__);
	}
	return buffer;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

