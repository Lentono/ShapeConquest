#include "AssetFactory.h"

bool AssetFactory::_initialized = false;
std::shared_ptr<Graphics> AssetFactory::_graphics = nullptr;


void AssetFactory::Initialize(std::shared_ptr<Graphics> graphics)
{
	_initialized = true;
	_graphics = graphics;
}

void AssetFactory::CleanUp()
{
	_graphics = nullptr;
}

std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> AssetFactory::LoadObjData_VTN(const std::string& fileName)
{
	std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> output;
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		std::cout << "Couldn't find file " << fileName << std::endl;

		return output;
	}

	//obj load used from here : https://stackoverflow.com/questions/21954789/directx11-how-to-load-an-obj-file
	char cmd[256] = { 0 };
	std::vector<glm::vec3> positions{};
	std::vector<glm::vec2> texCoords{};
	std::vector<glm::vec3> normals{};
	while (stream)
	{
		stream >> cmd;
		if (0 == std::strcmp(cmd, "#"))
		{
			continue;
		}
		if (0 == strcmp(cmd, "v")) // vertex (_position)
		{
			float x, y, z;
			stream >> x >> y >> z;
			positions.emplace_back(x, y, z);
		}
		else if (0 == strcmp(cmd, "vt")) // texture
		{
			float x, y;
			stream >> x >> y;
			texCoords.emplace_back(x, y);
		}
		else if (0 == strcmp(cmd, "vn")) // Normal
		{
			float x, y, z;
			stream >> x >> y >> z;
			normals.emplace_back(x, y, z);
		}
		else if (0 == strcmp(cmd, "f")) // defining a face
		{
			std::tuple<glm::vec3, glm::vec2, glm::vec3> vertex;
			unsigned int val;
			for (int i = 0; i < 3; i++)
			{
				stream >> val;
				val--;
				std::get<0>(vertex) = positions[val];
				stream.ignore();

				stream >> val;
				val--;
				std::get<1>(vertex) = texCoords[val];
				stream.ignore();

				stream >> val;
				val--;
				std::get<2>(vertex) = normals[val];
				stream.ignore();

				output.push_back(vertex);
			}
		}
	}
	stream.close();
	return output;
}



std::shared_ptr<RenderTarget> AssetFactory::CreateRenderTarget(const int width, const int height, const std::vector<std::shared_ptr<Texture>>& targetTextures,
	const std::vector<glm::vec4>&  clearColours, bool writeToDepthStencil, bool clearDepthStencil)
{
	std::vector<glm::vec4> tempCols = clearColours;
	//fill rest of clear colours with default clear colour
	for (unsigned int i = clearColours.size(); i < static_cast<unsigned int>(targetTextures.size()); i++)
	{
		tempCols.emplace_back(0.39f, 0.58f, 0.93f, 0);
	}
	auto blendState = CreateBlendState();
	auto depthState = CreateDepthState(clearDepthStencil);
	auto depthStencil = CreateDepthStencil(width, height);

	return std::make_shared<RenderTarget>(targetTextures, depthStencil, blendState, depthState, tempCols, clearDepthStencil);


}

std::shared_ptr<RenderTarget> AssetFactory::CreateRenderTarget()
{
	std::shared_ptr<Texture> bufferTexture = CreateEmptyTexture(_graphics->Width(), _graphics->Height());
	return CreateRenderTarget(_graphics->Width(), _graphics->Height(), { bufferTexture }, {}, true, true);
}

std::shared_ptr<RenderTarget> AssetFactory::CreateRenderTargetNoClear(const std::vector<std::shared_ptr<Texture>>& targetTextures)
{
	auto blendState = CreateBlendState();
	auto depthState = CreateDepthState(true);
	auto depthStencil = CreateDepthStencil(_graphics->Width(), _graphics->Height());

	return std::make_shared<RenderTarget>(targetTextures, depthStencil, blendState, depthState, std::vector<glm::vec4>(), true, false);
}

std::shared_ptr<BlendState> AssetFactory::CreateBlendState(const std::vector<float>& blendFactors, const unsigned int sampleMask)
{
#ifdef DIRECT_X
	D3D11_BLEND_DESC blend_desc{};
	ZeroMemory(&blend_desc, sizeof(blend_desc));
	//blend_desc.AlphaToCoverageEnable = false;
	//blend_desc.IndependentBlendEnable = false;
	blend_desc.RenderTarget[0].BlendEnable = false;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	/*blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;*/
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* blendState;
	_graphics->Device3D()->CreateBlendState(&blend_desc, &blendState);
	return std::make_shared<BlendState>(blendState, blendFactors, sampleMask);
#endif // DIRECT_X

}

std::shared_ptr<Texture> AssetFactory::LoadTexture(const wchar_t* fileName)
{
#ifdef DIRECT_X
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	//DirectX::CreateDDSTextureFromFile(_graphics->Device3D(), fileName, &texture, &textureView);
	DirectX::CreateWICTextureFromFile(_graphics->Device3D(), fileName, &texture, &textureView);

	return std::make_shared<Texture>(texture, textureView);
#endif
}

std::shared_ptr<Texture> AssetFactory::CreateEmptyTexture(const int width, const int height)
{
#ifdef DIRECT_X
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	ID3D11Texture2D* texture;
	HRESULT hr = _graphics->Device3D()->CreateTexture2D(&texDesc, nullptr, &texture);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Failed to create texture", nullptr, MB_ICONEXCLAMATION);
	}

	ID3D11ShaderResourceView* textureView;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc{};
	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;

	hr = _graphics->Device3D()->CreateShaderResourceView(texture, &shaderResourceDesc, &textureView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Failed to create texture", nullptr, MB_ICONEXCLAMATION);
	}

	ID3D11RenderTargetView* textureTarget;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = _graphics->Device3D()->CreateRenderTargetView(texture, &renderTargetViewDesc, &textureTarget);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Failed to create texture", nullptr, MB_ICONEXCLAMATION);
	}


	return std::make_shared<Texture>(texture, textureView, textureTarget);
#endif
}

std::shared_ptr<DepthStencil> AssetFactory::CreateDepthStencil(const int width, const int height)
{
#ifdef DIRECT_X
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D* g_pDepthStencil = nullptr;
	auto hr = _graphics->Device3D()->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
	if (FAILED(hr))
	{
		std::cout << "Failed to create depth stencil texture" << std::endl;
		return nullptr;
		//throw ErrorHandler("Failed to depth stencil texture", __LINE__, __FILE__);
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	struct ID3D11DepthStencilView* stencilView = nullptr;
	hr = _graphics->Device3D()->CreateDepthStencilView(g_pDepthStencil, &descDSV, &stencilView);
	if (FAILED(hr))
	{
		std::cout << "Failed to create depth stencil view" << std::endl;
		return nullptr;
		//throw ErrorHandler("Failed to create depth stencil view", __LINE__, __FILE__);
	}


	ID3D11ShaderResourceView* textureView;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc{};
	shaderResourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;

	hr = _graphics->Device3D()->CreateShaderResourceView(g_pDepthStencil, &shaderResourceDesc, &textureView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, "Failed to create texture", nullptr, MB_ICONEXCLAMATION);
	}
	return std::make_shared<DepthStencil>(g_pDepthStencil, stencilView, textureView);
#endif
}

std::shared_ptr<DepthState> AssetFactory::CreateDepthState(const bool depthEnable)
{
#ifdef DIRECT_X
	//Depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = depthEnable;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	_graphics->Device3D()->CreateDepthStencilState(&dsDesc, &pDSState);
	return std::make_shared<DepthState>(pDSState);
#endif

}

std::shared_ptr<Geometry> AssetFactory::LoadGeometry_VTN(const std::string& fileName)
{
	auto data = LoadObjData_VTN(fileName);
	if (data.empty())
	{
		std::cout << "Failed to load data from file '" << fileName << "'" << std::endl;
		return nullptr;
	}

	//TODO:: Maybe make changeable?
	std::vector<SimpleVertex> vertices;
	for (auto& d : data)
	{
		SimpleVertex s = { std::get<0>(d), std::get<1>(d), std::get<2>(d) };
		vertices.push_back(s);
	}
	return CreateGeometry<SimpleVertex>(vertices, {}, 4);
}

void AssetFactory::GetBytes(const std::string& file, size_t& len, char*& ret)
{
	//Taken from here : http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array
	std::ifstream reader(file.c_str(), ifstream::in | ifstream::binary);
	if(!reader.is_open())
	{
		OutputHandler::ErrorOut("Could not find file " + file);
		
		len = 0;
		ret = nullptr;
		return;
	}
	reader.seekg(0, std::ios::end);
	len = size_t(reader.tellg());
	ret = new char[len];
	reader.seekg(0, std::ios::beg);
	reader.read(&ret[0], len);
	reader.close();
}

#ifdef DIRECT_X

std::shared_ptr<Shader> AssetFactory::CreateShaderCSO(const std::map<ShaderTypes, std::string>& files, const std::map<ShaderTypes, unsigned>& constantBufferSizes, const InputLayoutTypes& inputLayoutType) {
	std::vector<InputElementTypes> inputElements;

	ConstructInputElementVector(inputLayoutType, inputElements);

	return CreateShaderCSO(files, constantBufferSizes, inputElements);
}

std::shared_ptr<Shader> AssetFactory::CreateShaderCSO(const std::map<ShaderTypes, std::string>& files, const std::map<ShaderTypes, unsigned int>& constantBufferSizes, const std::vector<InputElementTypes>& inputElements)
{
	ID3D11VertexShader* vertex = nullptr;
	ID3D11GeometryShader* geometry = nullptr;
	ID3D11HullShader* hull = nullptr;
	ID3D11DomainShader* domain = nullptr;
	ID3D11PixelShader* pixel = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	std::map<ShaderTypes, ID3D11Buffer*> shaderConstantBuffers;
	HRESULT hr;

	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

	ConstructInputElementDescription(inputElements, layout);

	for (const auto& file : files)
	{
		const ShaderTypes type = file.first;
		switch (type)
		{
		case ShaderTypes::Vertex:
		{
			size_t len;
			char* ret;
			GetBytes(file.second, len, ret);
			hr = _graphics->Device3D()->CreateVertexShader(ret, len, nullptr, &vertex);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create vertex shader", __LINE__, __FILE__);
				return nullptr;
			}
			if (!layout.empty())
			{
				hr = _graphics->Device3D()->CreateInputLayout(&layout[0], layout.size(), ret, len, &inputLayout);
				if (FAILED(hr))
				{
					OutputHandler::ErrorOut("Failed to create input layout", __LINE__, __FILE__);
					return nullptr;
				}
			}
			break;
		}
		case ShaderTypes::Geometry:
		{
			size_t len;
			char* ret;
			GetBytes(file.second, len, ret);
			hr = _graphics->Device3D()->CreateGeometryShader(ret, len, nullptr, &geometry);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create geometry shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Hull:
		{
			size_t len;
			char* ret;
			GetBytes(file.second, len, ret);
			hr = _graphics->Device3D()->CreateHullShader(ret, len, nullptr, &hull);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create hull shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Domain:
		{
			size_t len;
			char* ret;
			GetBytes(file.second, len, ret);
			hr = _graphics->Device3D()->CreateDomainShader(ret, len, nullptr, &domain);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create domain shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Pixel:
		{
			size_t len;
			char* ret;
			GetBytes(file.second, len, ret);
			hr = _graphics->Device3D()->CreatePixelShader(ret, len, nullptr, &pixel);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create pixel shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		default:
		{
			OutputHandler::ErrorOut("Tried to load a shader not coded for!!!", __LINE__, __FILE__);
			break;
		}
		}
	}

	for (const auto& cb : constantBufferSizes)
	{
		ShaderTypes type = cb.first;

		shaderConstantBuffers[type] = _graphics->CreateEmptyBuffer(cb.second, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0);

	}

	return std::make_shared<Shader>(vertex, geometry, hull, domain, pixel, shaderConstantBuffers, inputLayout);
}
#endif

std::shared_ptr<Shader> AssetFactory::CreateShader(const std::map<ShaderTypes, std::tuple<LPCWSTR, LPCSTR>>& filesAndMethods, std::map<ShaderTypes, unsigned> constantBufferSizes, const InputLayoutTypes& inputLayoutType) {
	std::vector<InputElementTypes> inputElements;
	
	ConstructInputElementVector(inputLayoutType, inputElements);
	
	return CreateShader(filesAndMethods, constantBufferSizes, inputElements);
}

std::shared_ptr<Shader> AssetFactory::CreateShader(const std::map<ShaderTypes, std::tuple<LPCWSTR, LPCSTR>>& filesAndMethods, std::map<ShaderTypes, unsigned int> constantBufferSizes, const std::vector<InputElementTypes>& inputElements)
{
#ifdef DIRECT_X
	ID3DBlob* pBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr;

	ID3D11VertexShader* vertex = nullptr;
	ID3D11GeometryShader* geometry = nullptr;
	ID3D11HullShader* hull = nullptr;
	ID3D11DomainShader* domain = nullptr;
	ID3D11PixelShader* pixel = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	std::map<ShaderTypes, ID3D11Buffer*> shaderConstantBuffers;

	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

	ConstructInputElementDescription(inputElements, layout);

	for (const auto& fileMethod : filesAndMethods)
	{
		ShaderTypes type = fileMethod.first;
		switch (type)
		{
		case ShaderTypes::Vertex:
		{
			hr = _graphics->CompileShader(std::get<0>(fileMethod.second), std::get<1>(fileMethod.second), "vs_4_0", &pBlob, &errorBlob);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to compile vertex shader, " + std::string(static_cast<char*>(errorBlob->GetBufferPointer())), __LINE__, __FILE__);
				return nullptr;
			}
			hr = _graphics->Device3D()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &vertex);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create vertex shader", __LINE__, __FILE__);
				return nullptr;
			}
			if (!layout.empty())
			{
				hr = _graphics->Device3D()->CreateInputLayout(&layout[0], layout.size(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &inputLayout);
				if (FAILED(hr))
				{
					OutputHandler::ErrorOut("Failed to create input layout", __LINE__, __FILE__);
					return nullptr;
				}
			}
			break;
		}
		case ShaderTypes::Geometry:
		{
			hr = _graphics->CompileShader(std::get<0>(fileMethod.second), std::get<1>(fileMethod.second), "gs_4_0", &pBlob, &errorBlob);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to compile geometry shader, " + std::string(static_cast<char*>(errorBlob->GetBufferPointer())), __LINE__, __FILE__);
				return nullptr;
			}
			hr = _graphics->Device3D()->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &geometry);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create geometry shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Hull:
		{
			hr = _graphics->CompileShader(std::get<0>(fileMethod.second), std::get<1>(fileMethod.second), "hs_4_0", &pBlob, &errorBlob);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to compile hull shader, " + std::string(static_cast<char*>(errorBlob->GetBufferPointer())), __LINE__, __FILE__);
				return nullptr;
			}
			hr = _graphics->Device3D()->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &hull);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create hull shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Domain:
		{
			hr = _graphics->CompileShader(std::get<0>(fileMethod.second), std::get<1>(fileMethod.second), "ds_4_0", &pBlob, &errorBlob);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to compile domain shader, " + std::string(static_cast<char*>(errorBlob->GetBufferPointer())), __LINE__, __FILE__);
				return nullptr;
			}
			hr = _graphics->Device3D()->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &hull);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create domain shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		case ShaderTypes::Pixel:
		{
			hr = _graphics->CompileShader(std::get<0>(fileMethod.second), std::get<1>(fileMethod.second), "ps_4_0", &pBlob, &errorBlob);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to compile pixel shader, " + std::string(static_cast<char*>(errorBlob->GetBufferPointer())), __LINE__, __FILE__);
				return nullptr;
			}
			hr = _graphics->Device3D()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pixel);
			if (FAILED(hr))
			{
				OutputHandler::ErrorOut("Failed to create pixel shader", __LINE__, __FILE__);
				return nullptr;
			}
			break;
		}
		default:
		{
			OutputHandler::ErrorOut("Tried to load a shader not coded for!!!", __LINE__, __FILE__);
			break;
		}
		}
	}

	for (const auto& cb : constantBufferSizes)
	{
		ShaderTypes type = cb.first;

		shaderConstantBuffers[type] = _graphics->CreateEmptyBuffer(cb.second, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0);

	}
	return std::make_shared<Shader>(vertex, geometry, hull, domain, pixel, shaderConstantBuffers, inputLayout);

#endif

}

void AssetFactory::ConstructInputElementVector(const InputLayoutTypes& inputLayoutType, std::vector<InputElementTypes>& inputElements)
{
	switch (inputLayoutType)
	{
	case InputLayoutTypes::None:
		inputElements = {};
		return;
	case InputLayoutTypes::Position:
		inputElements.emplace_back(InputElementTypes::Position);
		break;
	case InputLayoutTypes::PositionTexcoordNormal:
		inputElements.emplace_back(InputElementTypes::Position);
		inputElements.emplace_back(InputElementTypes::Texcoord);
		inputElements.emplace_back(InputElementTypes::Normal);
		break;
	case InputLayoutTypes::PositionTexcoordNormalBinormalTangent:
		inputElements.emplace_back(InputElementTypes::Position);
		inputElements.emplace_back(InputElementTypes::Texcoord);
		inputElements.emplace_back(InputElementTypes::Normal);
		inputElements.emplace_back(InputElementTypes::Binormal);
		inputElements.emplace_back(InputElementTypes::Tangent);
		break;
	case InputLayoutTypes::PositionInstancePosition:
		inputElements.emplace_back(InputElementTypes::Position);
		inputElements.emplace_back(InputElementTypes::InstancePosition);
		break;
	}
}

void AssetFactory::ConstructInputElementDescription(const std::vector<InputElementTypes>& inputElementTypes, std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDescription) 
{
	UINT positionSemanticIndex = 0;
	UINT texcoordSemanticIndex = 0;
	UINT normalSemanticIndex = 0;
	UINT binormalSemanticIndex = 0;
	UINT tangentSemanticIndex = 0;
	UINT instancePositionSemanticIndex = 0;
	UINT instanceMatrixSemanticIndex = 0;

	for (auto& element : inputElementTypes)
	{
		switch (element)
		{
		case InputElementTypes::Position:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "POSITION", positionSemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			positionSemanticIndex++;
			continue;
		case InputElementTypes::Texcoord:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "TEXCOORD", texcoordSemanticIndex, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			texcoordSemanticIndex++;
			continue;
		case InputElementTypes::Normal:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "NORMAL", normalSemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			normalSemanticIndex++;
			continue;
		case InputElementTypes::Binormal:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "BINORMAL", binormalSemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			binormalSemanticIndex++;
			continue;
		case InputElementTypes::Tangent:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "TANGENT", tangentSemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			tangentSemanticIndex++;
			continue;
		case InputElementTypes::InstancePosition:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "INSTANCEPOSITION", instancePositionSemanticIndex, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			instancePositionSemanticIndex++;
			continue;
		case InputElementTypes::InstanceMatrix:
			layoutDescription.emplace_back();
			layoutDescription.back() = { "INSTANCEMATRIX", instanceMatrixSemanticIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			instanceMatrixSemanticIndex++;
			layoutDescription.emplace_back();
			layoutDescription.back() = { "INSTANCEMATRIX", instanceMatrixSemanticIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			instanceMatrixSemanticIndex++;
			layoutDescription.emplace_back();
			layoutDescription.back() = { "INSTANCEMATRIX", instanceMatrixSemanticIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			instanceMatrixSemanticIndex++;
			layoutDescription.emplace_back();
			layoutDescription.back() = { "INSTANCEMATRIX", instanceMatrixSemanticIndex, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
			instanceMatrixSemanticIndex++;
			continue;
		}
	}
}


