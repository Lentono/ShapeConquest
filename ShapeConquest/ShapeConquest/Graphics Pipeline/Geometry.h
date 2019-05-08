#pragma once
#include "APIDeffinition.h"

#include <vector>
#include <iostream>
#include "Graphics.h"
#include <memory>

#ifdef DIRECT_X
#include <d3d11_1.h>
#endif
//B.M
//
//Represents vertex buffers on graphics card
//
class Geometry
{
public:
	Geometry();
	Geometry(int vertexCount, D3D_PRIMITIVE_TOPOLOGY drawTopology);
	virtual ~Geometry();
	
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	Geometry(Geometry&&) = delete;
	Geometry& operator=(const Geometry&&) = delete;

	void Prep(std::shared_ptr<Graphics>& graphics) const;//Makes sure the graphics card loads the right buffer slots
	void Render(std::shared_ptr<Graphics>& graphics) const;//Tell the graphics card to render
	
private:
	unsigned int _vertexCount = 0;
	unsigned int _instanceCount = 0;
	unsigned int _maxInstanceCount = 0;
	unsigned int _indexCount = 0;
	unsigned int _vertexStructSize = 0;
	unsigned int _instanceStructSize = 0;
	bool _isInstanced = false;
	bool _updateable = false;

#ifdef DIRECT_X

public:
	Geometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, int vertexCount, int indexCount, int vertexStructSize, D3D_PRIMITIVE_TOPOLOGY drawTopology);
	Geometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* instanceBuffer, ID3D11Buffer* indexBuffer, const int vertexCount, const int instanceCount,
		const int indexCount, const int vertexStructSize, const int instanceStructSize, const bool updateable, const D3D_PRIMITIVE_TOPOLOGY drawTopology);

	void Prep(ID3D11DeviceContext* deviceContext) const;
	void Render(ID3D11DeviceContext* deviceContext) const;
	template<class InstanceStruct> void UpdateInstancedBuffer(InstanceStruct instances[], UINT numInstances, ID3D11DeviceContext *deviceContext); 

private:

	ID3D11Buffer* _vertexBuffer = nullptr;
	ID3D11Buffer* _instanceBuffer = nullptr;
	ID3D11Buffer* _indexBuffer = nullptr;
	D3D_PRIMITIVE_TOPOLOGY _drawTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
#endif
};

#ifdef DIRECT_X
template<class InstanceStruct>
void Geometry::UpdateInstancedBuffer(InstanceStruct instances[], UINT numInstances, ID3D11DeviceContext* deviceContext)
{
	if (!_updateable)
	{
		std::cout << "Tried to update a buffer not prepped for updating " << std::endl;
		return;
		//throw ErrorHandler("Tried to update a buffer not prepped for updating ", __LINE__, __FILE__);
	}
	if (numInstances > _maxInstanceCount)
	{
		std::cout << "Tried to update a buffer not prepped for updating " << std::endl;
		return;
		//throw ErrorHandler("Tried to update with a size larger than original buffer size, this will not load all the data and will start reading memory outside of the buffer, ie bad. ", __LINE__, __FILE__);
	}
	//Code taken from stack overflow thread found here : https://stackoverflow.com/questions/26453773/direct3d-c-api-how-to-update-vertex-buffer
	_instanceCount = numInstances;
	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	UINT size = _instanceCount * sizeof(InstanceStruct);
	memcpy(resource.pData, instances, size);
	deviceContext->Unmap(_instanceBuffer, 0);
}
#endif


