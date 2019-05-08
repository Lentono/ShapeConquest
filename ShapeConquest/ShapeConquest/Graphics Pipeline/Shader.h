#pragma once
#include "APIDeffinition.h"
#include <vector>
#include <memory>
#include "Graphics.h"
#include <map>

#ifdef DIRECT_X
#include <d3d11.h>
#endif

enum class InputLayoutTypes 
{
	None,
	Position,
	PositionTexcoordNormal,
	PositionTexcoordNormalBinormalTangent,
	PositionInstancePosition,
};

enum class InputElementTypes
{
	Position,
	Texcoord,
	Normal,
	Binormal,
	Tangent,
	InstancePosition,
	InstanceMatrix
};

enum class ShaderTypes
{
	Vertex,
	Geometry,
	Hull,
	Domain,
	Pixel
	//If you add another type to this be sure to update the constructor and load methods
};

class Shader
{
public:
	Shader();
	virtual ~Shader();
	
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&&) = delete;

	void UpdateBuffer(ShaderTypes type, void* newBuffer, const std::shared_ptr<Graphics>& graphics);
	void Load(const std::shared_ptr<Graphics>& graphics);
	void Unload(const std::shared_ptr<Graphics>& graphics) const;

#ifdef DIRECT_X
public:
	Shader(ID3D11VertexShader * const vertexShader, ID3D11PixelShader * const pixelShader,
	       std::map<ShaderTypes, ID3D11Buffer*> constantBuffers, ID3D11InputLayout* const inputLayout);
	Shader(ID3D11VertexShader * const vertexShader, ID3D11GeometryShader* geometryShader, ID3D11PixelShader * const pixelShader, const std::map<ShaderTypes, ID3D11Buffer*>& constantBuffers,
		ID3D11InputLayout* const inputLayout);
	Shader(ID3D11VertexShader* vertexShader, ID3D11HullShader* hullShader, ID3D11DomainShader* domainShader, ID3D11PixelShader* pixelShader, const std::map<ShaderTypes, 
		ID3D11Buffer*>& constantBuffers, ID3D11InputLayout* inputLayout);
	Shader(ID3D11VertexShader* vertexShader, ID3D11GeometryShader* geometryShader, ID3D11HullShader* hullShader, ID3D11DomainShader* domainShader, ID3D11PixelShader* pixelShader,
	       const std::map<ShaderTypes, ID3D11Buffer*>& constantBuffers, ID3D11InputLayout* inputLayout);
private:
	
	ID3D11VertexShader* _vertexShader = nullptr;
	ID3D11GeometryShader* _geometryShader = nullptr;
	ID3D11HullShader* _hullShader = nullptr;
	ID3D11DomainShader* _domainShader = nullptr;
	ID3D11PixelShader* _pixelShader = nullptr;
	std::map<ShaderTypes, ID3D11Buffer*> _shaderConstantBuffers;
	ID3D11InputLayout* _inputLayout;
#endif

};



