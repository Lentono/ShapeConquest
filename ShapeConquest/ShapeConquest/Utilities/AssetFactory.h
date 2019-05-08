#pragma once
#include <memory>
#include <string>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <fstream>
#include <glm/glm.hpp>
#include <tuple>
#include <iostream>


#include "../Graphics Pipeline/Graphics.h"
#include "../Graphics Pipeline/Texture.h"
#include "../Graphics Pipeline/DepthStencil.h"
#include "../Graphics Pipeline/DepthState.h"
#include "../Graphics Pipeline/RenderTarget.h"
#include "../Graphics Pipeline/Geometry.h"
#include "../Graphics Pipeline/Shader.h"
#include "../Graphics Pipeline/BufferTypes.h"

#include "OutputHandler.h"

//B.M
//
//Just makes objects requiring access to the graphics api
//
class AssetFactory
{
public:
	static void Initialize(std::shared_ptr<Graphics> graphics);
	static void CleanUp();

	static std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> LoadObjData_VTN(const std::string & fileName);

	static std::shared_ptr<RenderTarget> CreateRenderTarget(const int width, const int height, const std::vector<std::shared_ptr<Texture>>& targetTextures,
		const std::vector<glm::vec4>& clearColours, bool writeToDepthStencil, bool clearDepthStencil);
	static std::shared_ptr<RenderTarget> CreateRenderTarget();
	static std::shared_ptr<RenderTarget> CreateRenderTargetNoClear(const std::vector<std::shared_ptr<Texture>>& targetTextures);

	static std::shared_ptr<BlendState> CreateBlendState(const std::vector<float>& blendFactors = { 1,1,1,1 }, const unsigned sampleMask = 0xffffff);
	static std::shared_ptr<Texture> LoadTexture(const wchar_t* fileName);
	static std::shared_ptr<Texture> CreateEmptyTexture(int width, int height);
	static std::shared_ptr<DepthStencil> CreateDepthStencil(int width, int height);
	static std::shared_ptr<DepthState> CreateDepthState(bool depthEnable);
	template<class VertexBufferType>
	static std::shared_ptr<Geometry> CreateGeometry(std::vector<VertexBufferType> vertices, std::vector<short> indices, unsigned int drawTopology);
	template<class VertexBufferType, class InstanceBufferType>
	static std::shared_ptr<Geometry> CreateGeometry(std::vector<VertexBufferType> vertices, std::vector<InstanceBufferType> instancePositions, std::vector<short> indices, const bool updateable, unsigned int drawTopology);
	static std::shared_ptr<Geometry> LoadGeometry_VTN(const std::string& fileName);
	static void GetBytes(
		const std::string& file, size_t& len, char*& ret);

	static std::shared_ptr<Shader> CreateShader(const std::map<ShaderTypes, std::tuple<LPCWSTR, LPCSTR>>& filesAndMethods, std::map<ShaderTypes, unsigned> constantBufferSizes, const InputLayoutTypes& inputLayoutType = InputLayoutTypes::PositionTexcoordNormal);
	static std::shared_ptr<Shader> CreateShader(const std::map<ShaderTypes, std::tuple<LPCWSTR, LPCSTR>>& filesAndMethods, std::map<ShaderTypes, unsigned> constantBufferSizes, const std::vector<InputElementTypes>& inputElements = { InputElementTypes::Position, InputElementTypes::Texcoord, InputElementTypes::Normal});

private:
	static void ConstructInputElementVector(const InputLayoutTypes& inputLayoutType, std::vector<InputElementTypes>& inputElements);
	static void ConstructInputElementDescription(const std::vector<InputElementTypes>& inputElementTypes, std::vector<D3D11_INPUT_ELEMENT_DESC> &layoutDescription);

private:
	static bool _initialized;
	static std::shared_ptr<Graphics> _graphics;

#ifdef DIRECT_X
public:
	static std::shared_ptr<Shader> CreateShaderCSO(const std::map<ShaderTypes, std::string>& files, const std::map<ShaderTypes, unsigned>& constantBufferSizes, const InputLayoutTypes& inputLayoutType = InputLayoutTypes::PositionTexcoordNormal);
	static std::shared_ptr<Shader> CreateShaderCSO(const std::map<ShaderTypes, std::string>& files, const std::map<ShaderTypes, unsigned>& constantBufferSizes, const std::vector<InputElementTypes>& inputElements = { InputElementTypes::Position, InputElementTypes::Texcoord, InputElementTypes::Normal });
private:
#endif
};

template<class VertexBufferType>
std::shared_ptr<Geometry> AssetFactory::CreateGeometry(std::vector<VertexBufferType> vertices, std::vector<short> indices, unsigned int drawTopology)
{
#ifdef DIRECT_X
	auto vertexBuffer = _graphics->CreateBuffer<VertexBufferType>(vertices, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);
	auto indexBuffer = indices.empty() ? nullptr : _graphics->CreateBuffer(indices, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0);
	return std::make_shared<Geometry>(vertexBuffer, indexBuffer, vertices.size(), indices.size(), sizeof(VertexBufferType), static_cast<D3D_PRIMITIVE_TOPOLOGY>(drawTopology));
#endif
}

template<class VertexBufferType, class InstanceBufferType>
std::shared_ptr<Geometry> AssetFactory::CreateGeometry(std::vector<VertexBufferType> vertices, std::vector<InstanceBufferType> instancePositions,
	std::vector<short> indices, const bool updateable, unsigned drawTopology) 
{
#ifdef DIRECT_X
	auto vertexBuffer = _graphics->CreateBuffer<VertexBufferType>(vertices, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);
	auto indexBuffer = indices.empty() ? nullptr : _graphics->CreateBuffer(indices, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0);
	auto instanceBuffer = _graphics->CreateBuffer<InstanceBufferType>(instancePositions, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0);

	return std::make_shared<Geometry>(vertexBuffer, instanceBuffer, indexBuffer, vertices.size(), instancePositions.size(), indices.size(), 
		sizeof(VertexBufferType), sizeof(InstanceBufferType), updateable, static_cast<D3D_PRIMITIVE_TOPOLOGY>(drawTopology));
#endif
}
