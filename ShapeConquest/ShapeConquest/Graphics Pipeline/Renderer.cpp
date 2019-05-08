#include "Renderer.h"
#include "PostProcessingEffects/PassOverPostProcess.h"
#include "../Managers/ResourceManager.h"


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
	AssetFactory::CleanUp();
	_backBuffer = nullptr;
	_graphics = nullptr;
}

void Renderer::Initialize(int width, int height, const std::string& windowName)
{
	_graphics = std::make_shared<Graphics>();
	_graphics->Initialise(width, height, windowName);
	AssetFactory::Initialize(_graphics);//Grants Access to the graphics for asset creation, needs to be done before any assets generated

	_shaders[ShaderEnum::Default] = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/DefaultVS.cso"}, //shouldn't need to do this so explicitly, but the app seems to be using the source files location as the default path
		{ShaderTypes::Pixel, "../x64/Debug/DefaultPS.cso"} }, { {ShaderTypes::Vertex, sizeof(DefaultConstantBuffer)} }, InputLayoutTypes::PositionTexcoordNormal);

	_shaders[ShaderEnum::PostProcessPassOver] = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/PPdefaultVertex.cso"}, {ShaderTypes::Geometry, "../x64/Debug/PPdefaultGeom.cso"},
		{ShaderTypes::Pixel, "../x64/Debug/TexturePS.cso"} }, { }, InputLayoutTypes::None);


#ifdef DIRECT_X
	//Make back buffer into a render target object
	ID3D11RenderTargetView* g_pRenderTargetView;
	auto back_buffer = _graphics->GetBackBuffer();
	_graphics->Device3D()->CreateRenderTargetView(back_buffer, nullptr, &g_pRenderTargetView);
	const std::vector<std::shared_ptr<Texture>> backBufferTextures = { std::make_shared<Texture>(back_buffer, nullptr, g_pRenderTargetView) };
	_backBuffer = AssetFactory::CreateRenderTarget(_graphics->Width(), _graphics->Height(), backBufferTextures, {}, true, true);
	//non UI default
	_targets["Default"] = AssetFactory::CreateRenderTarget();
	_outputToUiBuffer = std::make_shared<PassOverPostProcess>(AssetFactory::CreateRenderTarget(), _shaders[ShaderEnum::PostProcessPassOver]);
	//UI default
	_targets["UIDefault"] = AssetFactory::CreateRenderTargetNoClear({ _outputToUiBuffer->GetOutputBuffer() });

	_outputToBackBuffer = std::make_shared<PassOverPostProcess>(_backBuffer, _shaders[ShaderEnum::PostProcessPassOver]);

	_targetRenderOrder.push_back("Default");
	_targetRenderOrder.push_back("UIDefault");
	_graphics->SetUp2DRenderTarget();
#endif

	ResourceManager::AddGeometry("PPEdefault", std::make_shared<Geometry>(3, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	const glm::mat4 perspective = glm::perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	const glm::mat4 orthographic = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -100.0f, 100.0f);

	_screenBoundsY = glm::tan(45.0 / 2) * 5;
	_screenBoundsX = _screenBoundsY * (static_cast<float>(width) / static_cast<float>(height));

	_cameras["Default"] = std::make_shared<Camera>(glm::translate(glm::identity<glm::mat4>(), { 0,0,-5 }), perspective, orthographic);
}

void Renderer::Render(std::unordered_map<std::string, std::vector<std::shared_ptr<GameObject>>>& renderBatch, float dt)
{
	_currentShader = ShaderEnum::Default;
	_shaders[_currentShader]->Load(_graphics);
	std::shared_ptr<Texture> last = nullptr;
	for (const auto& t : _targetRenderOrder)
	{
		_targets[t]->Prep(_graphics);
		for (const auto& object : renderBatch[t])
		{
			if (object->alive())
			{
				LoadShader(object->ShaderToUse());
				object->Render(_graphics, _shaders[_currentShader], _cameras[object->CameraReferrence()]);
			}
		}
		if(t == "Default")
		{
			last = _targets[t]->GetOutput();
			//cycle through post processing effect for just non ui
			RunPostProcessingStack(last, _defaultPPEStack,dt);
			_outputToUiBuffer->Run(last, _graphics, dt);
		}
		if(t == "UIDefault")
		{
			last = _targets[t]->GetOutput();			
		}
	}

	//cycle through post processing effect for just ui
	RunPostProcessingStack(last, _defaultUIPPEStack,dt);
	_outputToBackBuffer->Run(last, _graphics, dt);

	_graphics->RenderTextBatch();

	_shaders[_currentShader]->Unload(_graphics);
	_graphics->SwapBuffer();
}

void Renderer::AddRenderTarget(const std::string& name, std::shared_ptr<RenderTarget> target)
{
	if (_targets.find(name) != _targets.end() || name == "Default")
	{
		std::cout << "Tried to add a target with name of pre-existing target" << std::endl;
		return;
	}
	_targets[name] = target;
	std::vector<string> newInsert = { name };
	_targetRenderOrder.insert(_targetRenderOrder.end() - 1, newInsert.begin(), newInsert.end());
}

void Renderer::RemoveRenderTarget(const std::string& name)
{
	const auto toRemove = _targets.find(name);
	if (toRemove != _targets.end())
	{
		_targets.erase(toRemove);
	}
}

void Renderer::AddShader(ShaderEnum name, std::shared_ptr<Shader> shader)
{
	if (_shaders.find(name) != _shaders.end())
	{
		OutputHandler::ErrorOut("Tried to add a shader with name of pre-existing shader", __LINE__, __FILE__);
		return;
	}
	_shaders[name] = shader;
}

void Renderer::RemoveShader(ShaderEnum name)
{
	const auto toRemove = _shaders.find(name);
	if (toRemove != _shaders.end())
	{
		_shaders.erase(toRemove);
	}
}

void Renderer::SetTargetOrder(const std::string& targetName, int orderNumber, bool ui)
{
	if (targetName == "Default" || targetName == "UIDefault")
	{
		OutputHandler::ErrorOut("Tried to change the render target order of Default, DON'T DO THAT!", __LINE__, __FILE__);
		return;
	}
	bool found = false;
	for (unsigned int i = 0; i < _targetRenderOrder.size(); i++)
	{
		if (_targetRenderOrder[i] == targetName)
		{
			_targetRenderOrder.erase(_targetRenderOrder.begin() + i);
			found = true;
			break;
		}
	}
	if (!found)
	{
		OutputHandler::ErrorOut("Tried to change order of render target that has not been made, DON'T DO THAT!", __LINE__, __FILE__);
		return;
	}
	if (ui && orderNumber >= _uiDefaultTargetOrderIndex)
	{
		OutputHandler::ErrorOut("Tried to change order of render target to be beyond the back buffer, I am putting it to the last before the back buffer, so there!", __LINE__, __FILE__);
		orderNumber = _uiDefaultTargetOrderIndex;
	}
	if (!ui && orderNumber >= _defaultTargetOrderIndex)
	{

		orderNumber = _defaultTargetOrderIndex - 1;
	}
	std::vector<std::string> toAdd = { targetName };
	_targetRenderOrder.insert(_targetRenderOrder.begin() + orderNumber, toAdd.begin(), toAdd.end());
}

void Renderer::RunPostProcessingStack(std::shared_ptr<Texture>& previousOutput, const std::vector<shared_ptr<PostProcessingEffect>> effectStack, float dt) const
{
	for(const auto& ppe : effectStack)
	{
		ppe->Run(previousOutput, _graphics, dt);
		previousOutput = ppe->GetOutputBuffer();
	}
}

void Renderer::AddPostProcessingEffect(std::shared_ptr<PostProcessingEffect> effect, bool effectsUI)
{
	if(effectsUI)
	{
		_defaultUIPPEStack.push_back(effect);
	}
	else
	{
		_defaultPPEStack.push_back(effect);
	}
}

void Renderer::LoadShader(ShaderEnum name)
{
	if (name == _currentShader)
	{
		return;
	}
	_shaders[_currentShader]->Unload(_graphics);
	_currentShader = name;
	_shaders[_currentShader]->Load(_graphics);
}
