
#include "GameWindow.h"
#include "Graphics Pipeline/Renderer.h"
#include "Managers/InputManager.h"
#include <ctime>
#include "Utilities/AssetFactory.h"
#include "Graphics Pipeline/BufferTypes.h"
#include "Scenes/StandardGameScene.h"
#include "Scenes/MenuScene.h"
#include "Graphics Pipeline/PostProcessingEffects/BlurPostProcess.h"
#include "Graphics Pipeline/PostProcessingEffects/NebulaPostProcess.h"
#include "PauseScene.h"
#include <thread>
#include "AudioManager.h"

GameWindow::GameWindow()
{
	_renderer = std::make_unique<Renderer>();
}


GameWindow::~GameWindow() = default;

void GameWindow::Run()
{
	srand(time(nullptr));
	//AudioManager::Initialize();
	OnLoad();	
	InputManager::SetWindowHandle(_renderer->GetWindow());
	InputManager::InitializeXInputGamePads();
	//InputManager::InitializeDirectInputGamePads();
	double deltaTime = 0;
	MSG msg = { nullptr };
	//std::thread t([](){AudioManager::PlaySound("Assets/Models/MusicMono.wav");});
	// Time when game is started
	double previousTime = clock();
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			const double currentTime = clock();
			deltaTime = (currentTime - previousTime) / CLOCKS_PER_SEC;
			InputManager::Update();
			OnUpdateFrame(deltaTime);
			OnRenderFrame(deltaTime);
			previousTime = currentTime;
		}
	}
	//t.join();
	Shutdown();
}

void GameWindow::OnLoad()
{
	_ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	_ScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	_renderer->Initialize(_ScreenWidth, _ScreenHeight, _ApplicationName);

	float bounds_x = _renderer->GetScreenBoundsX();
	float bounds_y = _renderer->GetScreenBoundsY();

	SceneManager::Init(_ScreenWidth, _ScreenHeight, bounds_x, bounds_y);

	const std::shared_ptr<Shader> testShader = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/DefaultVS.cso"}, {ShaderTypes::Pixel, "../x64/Debug/TestPinkPixel.cso"} }, { {ShaderTypes::Vertex, sizeof(DefaultConstantBuffer)} }, InputLayoutTypes::PositionTexcoordNormal);
	_renderer->AddShader(ShaderEnum::TestPink, testShader);

	const std::shared_ptr<Shader> textureShader = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/DefaultVS.cso"}, {ShaderTypes::Pixel, "../x64/Debug/TexturePS.cso"} }, { {ShaderTypes::Vertex, sizeof(DefaultConstantBuffer)} }, InputLayoutTypes::PositionTexcoordNormal);
	_renderer->AddShader(ShaderEnum::DefaultTextured, textureShader);

	const std::shared_ptr<Shader> particleSystemShader = AssetFactory::CreateShaderCSO({
		{ShaderTypes::Vertex, "../x64/Debug/ParticleSysVS.cso"}, {ShaderTypes::Geometry, "../x64/Debug/ParticleSysGS.cso"}, {ShaderTypes::Pixel, "../x64/Debug/ParticleSysPS.cso"} },
		{ {ShaderTypes::Vertex, sizeof(ParticleSystemConstantBuffer)}, {ShaderTypes::Geometry, sizeof(DefaultConstantBuffer)} }, InputLayoutTypes::PositionInstancePosition);

	_renderer->AddShader(ShaderEnum::ParticleSystem, particleSystemShader);



	const auto galaxyShader = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/PPdefaultVertex.cso"}, {ShaderTypes::Geometry, "../x64/Debug/PPdefaultGeom.cso"},
		{ShaderTypes::Pixel, "../x64/Debug/GalaxyPPpixel.cso"} }, { {ShaderTypes::Pixel, sizeof(NebularCB)} }, InputLayoutTypes::None);

	_renderer->AddShader(ShaderEnum::GalaxyPostProcess, galaxyShader);


	_renderer->AddPostProcessingEffect(std::make_shared<NebulaPostProcess>(AssetFactory::CreateRenderTarget(), galaxyShader));


	const auto blurShader = AssetFactory::CreateShaderCSO({ {ShaderTypes::Vertex, "../x64/Debug/PPdefaultVertex.cso"}, {ShaderTypes::Geometry, "../x64/Debug/PPdefaultGeom.cso"},
		{ShaderTypes::Pixel, "../x64/Debug/PPEblurPixel.cso"} }, { {ShaderTypes::Pixel, sizeof(BlurCB)} }, InputLayoutTypes::None);

	_renderer->AddShader(ShaderEnum::BlurPostProcess, blurShader);

	_renderer->AddPostProcessingEffect(std::make_shared<BlurPostProcess>(AssetFactory::CreateRenderTarget(), blurShader));

#pragma region Default runtime geometry

	vector<SimpleVertex> verts;
	SimpleVertex s{};
	s.position = { 0, 0, 0 };
	s.normal = { 0,0,-1 };
	s.texCoord = { 0,0 };
	verts.push_back(s);
	s.position = { 1,-1,0 };
	s.texCoord = { 1,1 };
	verts.push_back(s);
	s.position = { 1,0,0 };
	s.texCoord = { 1,0 };
	verts.push_back(s);
	s.position = { 0,-1,0 };
	s.texCoord = { 0,1 };
	verts.push_back(s);

	const vector<short> indices =
	{
		3,1,0,
		2
	};

	const auto geom = AssetFactory::CreateGeometry<SimpleVertex>(verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ResourceManager::AddGeometry("Quad", geom);

#pragma endregion 

#pragma region scenes

	AudioManager::Initialize();
	//SceneManager::AddScene(std::make_shared<StandardGameScene>(_ScreenWidth, _ScreenHeight, bounds_x, bounds_y), "GameScene");
	SceneManager::AddScene(std::make_shared<MenuScene>(_ScreenWidth, _ScreenHeight, bounds_x, bounds_y), "MainMenu");
	SceneManager::AddScene(std::make_shared<PauseScene>(_ScreenWidth, _ScreenHeight, bounds_x, bounds_y), "PauseScene");
	SceneManager::ChangeScene("MainMenu");
#pragma endregion 
}
// May need to pass new screen width and screen height to this function
void GameWindow::OnScreenResize() const
{
	SceneManager::OnScreenResize(_ScreenWidth, _ScreenHeight, _screenBoundsX, _screenBoundsY);
}

void GameWindow::OnUpdateFrame(double deltaTime) const
{
	SceneManager::OnUpdateFrame(deltaTime);
}

void GameWindow::OnRenderFrame(float dt) const
{
	SceneManager::OnRenderFrame(_renderer, dt);
}

void GameWindow::Shutdown() const
{
	SceneManager::Shutdown();
}
