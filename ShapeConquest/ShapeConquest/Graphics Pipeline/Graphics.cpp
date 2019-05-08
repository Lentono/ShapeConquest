#include "Graphics.h"
#include <glm/fwd.hpp>


Graphics::Graphics()
{
}


Graphics::~Graphics()
{
	CleanUp();
}

void Graphics::Initialise(int width, int height, const std::string& windowName)
{
	_width = width;
	_height = height;
#ifdef DIRECT_X
	_hInstance = GetModuleHandle(nullptr);
	InitWindow(windowName);
	InitDirectX();

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1;
	samplerDesc.BorderColor[1] = 1;
	samplerDesc.BorderColor[2] = 1;
	samplerDesc.BorderColor[3] = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_device3D->CreateSamplerState(&samplerDesc, &_defaultSampleState);

#endif
}

void Graphics::SwapBuffer() const
{
#ifdef DIRECT_X
	_swapChain->Present(0, 0);
#endif

}

void Graphics::RenderText(const TextDescription& td)
{
	_deferredTextBatch.push_back(td);
}

void Graphics::RenderTextBatch()
{
	for(const auto& td : _deferredTextBatch)
	{
		const string text = td.Text();
		std::wstring wText(text.length(), L' ');

		std::copy(text.begin(), text.end(), wText.begin());

		glm::vec2 scale = td.Scale() / _fontSize;
		IDWriteTextLayout* textLayout;
		_writeFactory->CreateTextLayout(wText.c_str(), static_cast<glm::uint32>(wText.length()), _textFormat,_width, 1, &textLayout);

		ID2D1SolidColorBrush* colourBrush;
		auto colour = td.Colour();
		_deviceContext2D->CreateSolidColorBrush(D2D1::ColorF(colour.x, colour.y, colour.z, colour.w), &colourBrush);

		_deviceContext2D->SaveDrawingState(_stateBlock);
		_deviceContext2D->BeginDraw();
		const auto pos = td.ScreenPosition();


		_deviceContext2D->SetTransform(D2D1::Matrix3x2F::Scale(scale.x, scale.y) * D2D1::Matrix3x2F::Translation(pos.x, _height - pos.y) * D2D1::Matrix3x2F::Identity());
		//_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		_deviceContext2D->DrawTextLayout({ 0,0 }, textLayout, colourBrush);
		HRESULT hr = _deviceContext2D->EndDraw();
		_deviceContext2D->RestoreDrawingState(_stateBlock);

		textLayout->Release();
		colourBrush->Release();
	}

	_deferredTextBatch.clear();
}

#ifdef DIRECT_X
void Graphics::InitWindow(const std::string& windowName)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, static_cast<LPCTSTR>(IDI_WINLOGO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, static_cast<LPCTSTR>(IDI_WINLOGO));
	if (!RegisterClassEx(&wcex))
	{
		std::cout << "Failed To register class" << std::endl;
		return;
	}
	//CreateWindow
	RECT rc = { 0, 0, _width, _height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow("TutorialWindowClass", windowName.c_str(), WS_OVERLAPPEDWINDOW,
		0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, _hInstance,
		nullptr);
	if (!_hWnd)
	{
		std::cout << "Failed to create window" << std::endl;
		return;
		//throw ErrorHandler("Failed to create window", __LINE__, __FILE__);
	}
	SetWindowPos(_hWnd, nullptr, 0, 0, _width, _height, SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SW_MAXIMIZE);
	ShowWindow(_hWnd, true);
}

void Graphics::InitDirectX()
{
	HRESULT hr = 1;

	RECT rc;
	GetClientRect(_hWnd, &rc);

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Required to allow 2D device inclusion

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	//Create Device and Device context
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &device, &_featureLevel, &deviceContext);

		//if (hr == static_cast<HRESULT>(0x80070057L)) //E_INVALIDARG
		//{
		//	// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
		//	hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
		//		D3D11_SDK_VERSION, &_device, &_featureLevel, &_deviceContext);
		//}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to create device", __LINE__, __FILE__);
		return;
		//throw ErrorHandler("Failed to create Device", __LINE__, __FILE__);
	}

	//BEN: "OOOH IDK WHY THE CULL MODE IS WEIRD BECAUSE I SET IT (INSERT NO RASTERIZER STATE) REEEEEEEEEEEEEEEEEEEEE"

	D3D11_RASTERIZER_DESC rasterDescription;

	//Initialize raster description
	ZeroMemory(&rasterDescription, sizeof(rasterDescription));

	//Initialize and create our rasterizer so we can control how the polygons can be drawn and then bind it to our context
	rasterDescription = {
		D3D11_FILL_SOLID, //FillMode
		//D3D11_FILL_WIREFRAME,
		D3D11_CULL_FRONT, //CullMode
		false, //FrontCounterClockwise
		0, //DepthBias
		0.0f, //DepthBiasClamp
		0.0f, //SlopeScaledDepthBias
		true, //DepthClipEnable
		false, //ScissorEnable
		false, //MultisampleEnable
		false //AntialiasedLineEnable
	};

	const auto result = device->CreateRasterizerState(&rasterDescription, &m_rasterState);

	if (FAILED(result))
	{
		auto ripDied = true;
		return;
	}

	deviceContext->RSSetState(m_rasterState);

	_device3D = reinterpret_cast<ID3D11Device3*>(device);
	_deviceContext3D = reinterpret_cast<ID3D11DeviceContext3*>(deviceContext);


	IDXGIDevice3* dxgiDevice = nullptr;
	hr = _device3D->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(&dxgiDevice));
	if(FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to interpret device as dxgi device", __LINE__, __FILE__);
	}

	//Create 2d context
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
	
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), reinterpret_cast<void**>(&_factory2D));
		if (FAILED(hr))
		{
			OutputHandler::ErrorOut("Failed to create 2D Factory", __LINE__, __FILE__);
		}
		else
		{
			hr = _factory2D->CreateDevice(dxgiDevice, &_device2D);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create 2D Device", __LINE__, __FILE__);
			}
			else
			{
				hr = _device2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_deviceContext2D);
				if (FAILED(hr))
				{
					OutputHandler::ErrorOut("Failed to create 2D Device context", __LINE__, __FILE__);
				}
			}
		}
	}

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory4* dxgiFactory = nullptr;
	{
		if (dxgiDevice)
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory4), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to Obtain DXGI factory", __LINE__, __FILE__);
		return;
		//throw ErrorHandler("Failed to Obtain DXGI factory", __LINE__, __FILE__);
	}
	
	// Swap Chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _width;
	sd.BufferDesc.Height = _height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = FALSE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	IDXGISwapChain* swapChain = nullptr;
	hr = dxgiFactory->CreateSwapChain(_device3D, &sd, &swapChain);
	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to make swap chain", __LINE__, __FILE__);
	}
	hr = dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);

	dxgiFactory->Release();
	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to make window association", __LINE__, __FILE__);
	}
	hr = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), reinterpret_cast<void**>(&_swapChain));
	if(FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to reinterprit swap chain", __LINE__, __FILE__);
	}
	_swapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), reinterpret_cast<IUnknown**>(&_writeFactory));

	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to create Write Factory", __LINE__, __FILE__);
	}

	_fontSize = 50.0f * (_width / 800.0);
	_writeFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, _fontSize, L"en-UK", &_textFormat);
	_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	_factory2D->CreateDrawingStateBlock(&_stateBlock);

	/*hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_wicFactory));

	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to create WIC Factory", __LINE__, __FILE__);
	}*/

	swapChain->SetFullscreenState(true, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(_width);
	vp.Height = static_cast<FLOAT>(_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_deviceContext3D->RSSetViewports(1, &vp);
}

void Graphics::SetUp2DRenderTarget()
{
	//Connect 2D to  back buffer
	float dpiX, dpiY;
	_factory2D->GetDesktopDpi(&dpiX, &dpiY);

	IDXGISurface2* pDXGIBackBuffer = nullptr;
	HRESULT hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&pDXGIBackBuffer));

	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to get back buffer for IDXGISurface", __LINE__, __FILE__);
	}
	//_deviceContext2D->SetTarget(nullptr);
	hr = _deviceContext2D->CreateBitmapFromDxgiSurface(pDXGIBackBuffer, nullptr, &_2DTarget);

	if (FAILED(hr))
	{
		OutputHandler::ErrorOut("Failed to create bitmap for IDXGISurface", __LINE__, __FILE__);
	}
	pDXGIBackBuffer->Release();
	_deviceContext2D->SetTarget(_2DTarget);
	_deviceContext2D->SetDpi(dpiX, dpiY);
	_deviceContext2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

ID3D11Texture2D* Graphics::GetBackBuffer() const
{
	ID3D11Texture2D* pBackBuffer;
	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
	{
		std::cout << "Failed to get buffer from swap chain " << std::endl;
		return nullptr;
		//throw ErrorHandler("Failed to get buffer from swap chain ", __LINE__, __FILE__);
	}
	return pBackBuffer;
}
//Compiling of shaders copied from example tutorials from https://blogs.msdn.microsoft.com/chuckw/2013/09/20/directx-sdk-samples-catalog/
HRESULT Graphics::CompileShader(LPCWSTR const szFileName, LPCSTR const szEntryPoint, LPCSTR const szShaderModel, ID3DBlob** const ppBlobOut, ID3DBlob** const errorBlob)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, errorBlob);
	if (FAILED(hr))
	{
		if (*errorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>((*errorBlob)->GetBufferPointer()));
		}
		return hr;
	}

	return 1;
}
ID3D11Buffer* Graphics::CreateEmptyBuffer(unsigned int size, D3D11_USAGE usage, unsigned bindFlags, unsigned cpuAccessFlags) const
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.ByteWidth = size;
	bd.BindFlags = bindFlags;
	bd.CPUAccessFlags = cpuAccessFlags;
	ID3D11Buffer* buffer;
	const HRESULT hr = _device3D->CreateBuffer(&bd, nullptr, &buffer);

	if (FAILED(hr))
	{
		std::cout << "Failed to create buffer ";
		return nullptr;
		//throw ErrorHandler("Whoops, failed to create buffer ", __LINE__, __FILE__);
	}
	return buffer;
}
#endif

LRESULT CALLBACK WndProc(HWND const hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
void Graphics::CleanUp() const
{
#ifdef DIRECT_X
	if (_deviceContext3D)
	{
		_deviceContext3D->ClearState();
	}
	/*if (_wicFactory)
	{
		_wicFactory->Release();
	}*/
	if(_factory2D)
	{
		_factory2D->Release();
	}
	if(_textFormat)
	{
		_textFormat->Release();
	}
	if(_stateBlock)
	{
		_stateBlock->Release();
	}
	if(_writeFactory)
	{
		_writeFactory->Release();
	}
	if(_2DTarget)
	{
		_2DTarget->Release();
	}
	if (_swapChain)
	{
		_swapChain->Release();
	}
	if (_deviceContext2D)
	{
		_deviceContext2D->Release();
	}
	if(_device2D)
	{
		_device2D->Release();
	}
	if (_deviceContext3D)
	{
		_deviceContext3D->Release();
	}
	if (_device3D)
	{
		_device3D->Release();
	}
	DestroyWindow(_hWnd);
#endif
}

