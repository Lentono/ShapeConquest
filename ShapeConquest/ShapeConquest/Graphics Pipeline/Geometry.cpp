#include "Geometry.h"
#include "Graphics.h"


Geometry::Geometry()
{
}

Geometry::Geometry(int vertexCount, D3D_PRIMITIVE_TOPOLOGY drawTopology)
	:_vertexCount(vertexCount), _drawTopology(drawTopology)
{
}


Geometry::~Geometry()
{
#ifdef DIRECT_X
	try
	{
		if (_vertexBuffer)
		{
			_vertexBuffer->Release();
		}
		if (_instanceBuffer)
		{
			_instanceBuffer->Release();
		}
		if (_indexBuffer)
		{
			_indexBuffer->Release();
		}
	}
	catch (const std::exception&)
	{

	}
#endif
}

void Geometry::Prep(std::shared_ptr<Graphics>& graphics) const
{
#ifdef DIRECT_X
	Prep(graphics->DeviceContext3D());
#endif
}

void Geometry::Render(std::shared_ptr<Graphics>& graphics) const
{
#ifdef DIRECT_X
	Render(graphics->DeviceContext3D());
#endif
}

#ifdef DIRECT_X

Geometry::Geometry(ID3D11Buffer * const vertexBuffer, ID3D11Buffer * const indexBuffer, const int vertexCount, const int indexCount, const int vertexStructSize, const D3D_PRIMITIVE_TOPOLOGY drawTopology)
	: _vertexBuffer(vertexBuffer), _instanceBuffer(nullptr), _indexBuffer(indexBuffer), _vertexCount(vertexCount),
	_instanceCount(0), _maxInstanceCount(0), _indexCount(indexCount), _vertexStructSize(vertexStructSize), _instanceStructSize(0), _updateable(false),
	_drawTopology(drawTopology)
{
	_isInstanced = false;
}

Geometry::Geometry(ID3D11Buffer * const vertexBuffer, ID3D11Buffer * const instanceBuffer, ID3D11Buffer * const indexBuffer, const int vertexCount, const int instanceCount,
	const int indexCount, const int vertexStructSize, const int instanceStructSize, const bool updateable, const D3D_PRIMITIVE_TOPOLOGY drawTopology)
	: _vertexBuffer(vertexBuffer), _instanceBuffer(instanceBuffer), _indexBuffer(indexBuffer), _vertexCount(vertexCount), _instanceCount(instanceCount), _maxInstanceCount(instanceCount),
	_indexCount(indexCount), _vertexStructSize(vertexStructSize), _instanceStructSize(instanceStructSize), _updateable(updateable), _drawTopology(drawTopology)
{
	_isInstanced = true;
}

void Geometry::Prep(ID3D11DeviceContext* deviceContext) const
{
	if (!_isInstanced)
	{
		const UINT stride = _vertexStructSize;
		const UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	}
	else
	{
		std::vector<UINT> stride;
		stride.push_back(_vertexStructSize);
		stride.push_back(_instanceStructSize);
		std::vector<UINT> offset;
		offset.push_back(0);
		offset.push_back(0);
		std::vector < ID3D11Buffer*> bufferPointers;
		bufferPointers.push_back(_vertexBuffer);
		bufferPointers.push_back(_instanceBuffer);
		deviceContext->IASetVertexBuffers(0, 2, bufferPointers.data(), stride.data(), offset.data());
	}
	if (_indexCount > 0)
	{
		deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
	deviceContext->IASetPrimitiveTopology(_drawTopology);
}

void Geometry::Render(ID3D11DeviceContext* deviceContext) const
{
	if (!_isInstanced)
	{
		if (_indexCount > 0)
		{
			deviceContext->DrawIndexed(_indexCount, 0, 0);
		}
		else
		{
			deviceContext->Draw(_vertexCount, 0);
		}
	}
	else
	{
		if (_indexCount > 0)
		{
			deviceContext->DrawIndexedInstanced(_indexCount, _instanceCount, 0, 0, 0);
		}
		else
		{
			deviceContext->DrawInstanced(_vertexCount, _instanceCount, 0, 0);
		}
	}
}
#endif // DIRECT_X


