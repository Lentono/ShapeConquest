#include "Shader.h"
#include <utility>


Shader::Shader()
{
}


Shader::~Shader()
{
	try
	{
		if (_vertexShader)
		{
			_vertexShader->Release();
		}
		if (_geometryShader)
		{
			_geometryShader->Release();
		}
		if (_hullShader)
		{
			_hullShader->Release();
		}
		if (_domainShader)
		{
			_domainShader->Release();
		}
		if (_pixelShader)
		{
			_pixelShader->Release();
		}
		for (const auto& cb : _shaderConstantBuffers)
		{
			if (cb.second)
			{
				cb.second->Release();
			}
		}
		if (_inputLayout)
		{
			_inputLayout->Release();
		}
	}
	catch (const std::exception&)
	{

	}
}

void Shader::UpdateBuffer(ShaderTypes type, void* newBuffer, const std::shared_ptr<Graphics>& graphics)
{
#ifdef DIRECT_X
	graphics->DeviceContext3D()->UpdateSubresource(_shaderConstantBuffers[type], 0, nullptr, newBuffer, 0, 0);
#endif // DIRECT_X
}

void Shader::Load(const std::shared_ptr<Graphics>& graphics)
{
#ifdef DIRECT_X
	auto deviceContext = graphics->DeviceContext3D();
	deviceContext->IASetInputLayout(_inputLayout);
	deviceContext->VSSetShader(_vertexShader, nullptr, 0);
	if (_vertexShader)
	{
		struct ID3D11Buffer* buffer = _shaderConstantBuffers[ShaderTypes::Vertex];
		if (buffer)
		{
			deviceContext->VSSetConstantBuffers(0, 1, &buffer);
		}
	}
	deviceContext->GSSetShader(_geometryShader, nullptr, 0);
	if (_geometryShader)
	{
		struct ID3D11Buffer* buffer = _shaderConstantBuffers[ShaderTypes::Geometry];
		if (buffer)
		{
			deviceContext->GSSetConstantBuffers(0, 1, &buffer);
		}
	}
	deviceContext->HSSetShader(_hullShader, nullptr, 0);
	if (_hullShader)
	{
		struct ID3D11Buffer* buffer = _shaderConstantBuffers[ShaderTypes::Hull];
		if (buffer)
		{
			deviceContext->HSSetConstantBuffers(0, 1, &buffer);
		}
	}
	deviceContext->DSSetShader(_domainShader, nullptr, 0);
	if (_domainShader)
	{
		struct ID3D11Buffer* buffer = _shaderConstantBuffers[ShaderTypes::Domain];
		if (buffer)
		{
			deviceContext->DSSetConstantBuffers(0, 1, &buffer);
		}
	}
	deviceContext->PSSetShader(_pixelShader, nullptr, 0);
	if (_pixelShader)
	{
		struct ID3D11Buffer* buffer = _shaderConstantBuffers[ShaderTypes::Pixel];
		if (buffer)
		{
			deviceContext->PSSetConstantBuffers(0, 1, &buffer);
		}
	}
#endif

}

void Shader::Unload(const std::shared_ptr<Graphics>& graphics) const
{
	auto deviceContext = graphics->DeviceContext3D();
	if (_inputLayout)
	{
		deviceContext->IASetInputLayout(nullptr);
	}
	if (_vertexShader)
	{
		deviceContext->VSSetShader(nullptr, nullptr, 0);
	}
	if (_geometryShader)
	{
		deviceContext->GSSetShader(nullptr, nullptr, 0);
	}
	if (_hullShader)
	{
		deviceContext->HSSetShader(nullptr, nullptr, 0);
	}
	if (_domainShader)
	{
		deviceContext->DSSetShader(nullptr, nullptr, 0);
	}
	if (_pixelShader)
	{
		deviceContext->PSSetShader(nullptr, nullptr, 0);
	}
}


Shader::Shader(ID3D11VertexShader* const vertexShader, ID3D11PixelShader* const pixelShader,
	std::map<ShaderTypes, ID3D11Buffer*> constantBuffers, ID3D11InputLayout* const inputLayout)
	: _vertexShader(vertexShader), _pixelShader(pixelShader), _shaderConstantBuffers(std::move(constantBuffers)), _inputLayout(inputLayout)

{
	for (int i = 0; i < 4; i++)
	{
		auto types = static_cast<ShaderTypes>(i);
		if (_shaderConstantBuffers.find(types) == _shaderConstantBuffers.end())
		{
			_shaderConstantBuffers[types] = nullptr;
		}
	}
}

Shader::Shader(ID3D11VertexShader* const vertexShader, ID3D11GeometryShader* geometryShader,
	ID3D11PixelShader* const pixelShader, const std::map<ShaderTypes, ID3D11Buffer*>& constantBuffers,
	ID3D11InputLayout* const inputLayout)
	: Shader(vertexShader, pixelShader, constantBuffers, inputLayout)
{
	_geometryShader = geometryShader;
}

Shader::Shader(ID3D11VertexShader* const vertexShader, ID3D11HullShader* hullShader, ID3D11DomainShader* domainShader,
	ID3D11PixelShader* const pixelShader, const std::map<ShaderTypes, ID3D11Buffer*>& constantBuffers,
	ID3D11InputLayout* const inputLayout)
	: Shader(vertexShader, pixelShader, constantBuffers, inputLayout)
{
	_hullShader = hullShader;
	_domainShader = domainShader;
}

Shader::Shader(ID3D11VertexShader* const vertexShader, ID3D11GeometryShader* geometryShader,
	ID3D11HullShader* hullShader, ID3D11DomainShader* domainShader, ID3D11PixelShader* const pixelShader,
	const std::map<ShaderTypes, ID3D11Buffer*>& constantBuffers, ID3D11InputLayout* const inputLayout)
	: Shader(vertexShader, geometryShader, pixelShader, constantBuffers, inputLayout)
{
	_hullShader = hullShader;
	_domainShader = domainShader;
}