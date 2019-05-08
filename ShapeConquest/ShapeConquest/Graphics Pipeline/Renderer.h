#pragma once
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <glm/gtc/matrix_transform.hpp>

#include "../GameObjects/GameObject.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "Shader.h"
#include "Camera.h"
#include "../Utilities/OutputHandler.h"
#include "ShaderEnum.h"
#include "BufferTypes.h"
#include "../Utilities/AssetFactory.h"
#include <unordered_map>
#include "PostProcessingEffects/PostProcessingEffect.h"

//B.M
//
//Renderer Class accepts game objects to batch render, batching so objects can target render targets in order
//
class Renderer
{
public:
	Renderer();
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&&) = delete;

	void Initialize(int width, int height, const std::string& windowName);//Set up graphics api and Asset generating class
	void Render(std::unordered_map<std::string, std::vector<std::shared_ptr<GameObject>>>& renderBatch, float dt);//Renders batch and swaps buffer, clears batch.
	void AddRenderTarget(const std::string& name, std::shared_ptr<RenderTarget> target);//add a new target to the available targets
	void RemoveRenderTarget(const std::string& name);//Remove a target to the available targets
	void AddShader(ShaderEnum name, std::shared_ptr<Shader> shader);
	void RemoveShader(ShaderEnum name);
	HWND GetWindow() const { return _graphics->GetWindow(); };
	float GetScreenBoundsX() const { return _screenBoundsX; }
	float GetScreenBoundsY() const { return _screenBoundsY; }
	void SetTargetOrder(const std::string& targetName, int orderNumber, bool ui = false);

	void RunPostProcessingStack(std::shared_ptr<Texture>& previousOutput, const std::vector<shared_ptr<PostProcessingEffect>> effectStack, float dt) const;
	void AddPostProcessingEffect(std::shared_ptr<PostProcessingEffect> effect, bool effectsUI = false);

private:

	void LoadShader(ShaderEnum name);

	std::shared_ptr<Graphics> _graphics;

	std::vector<std::string> _targetRenderOrder;
	std::unordered_map<std::string, std::shared_ptr<RenderTarget>> _targets;//Non default render targets

	std::vector<shared_ptr<PostProcessingEffect>> _defaultPPEStack;
	std::vector<shared_ptr<PostProcessingEffect>> _defaultUIPPEStack;

	std::shared_ptr<PostProcessingEffect> _outputToUiBuffer;
	std::shared_ptr<PostProcessingEffect> _outputToBackBuffer;
	std::shared_ptr<RenderTarget> _backBuffer;//Default target, the buffer that gets swapped 
	std::map<ShaderEnum, std::shared_ptr<Shader>> _shaders;
	std::map<std::string, std::shared_ptr<Camera>> _cameras;
	ShaderEnum _currentShader = ShaderEnum::Default;
	float _screenBoundsX, _screenBoundsY = 0;
	int _defaultTargetOrderIndex = 0;
	int _uiDefaultTargetOrderIndex = 1;
};

