#pragma once
#include "../GameObjects/GameObject.h"
#include "../Graphics Pipeline/Renderer.h"
#include "../GameObjects/CollidableObject.h"
#include "../GameObjects/AI.h"
#include "../Utilities/AssetFactory.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/PowerUp.h"
#include "../Utilities/CollisionHandler.h"
#include "../GameObjects/ParticleSystem.h"
#include "../GameObjects/PowerUp.h"
#include "../GameObjects/PlayerZone.h"
#include <functional>
#include <glm/fwd.hpp>
#include "../Bullet.h"
#include "../GameScene.h"

class StandardGameScene : public GameScene
{
public:
	StandardGameScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY);
	~StandardGameScene();	

	virtual void OnLoad() override;

	virtual void OnUpdateFrame(double deltaTime) override;	

	virtual void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) override;

	virtual void Reset() override;

	virtual void Shutdown() override;

	virtual void AddGameObject(const std::shared_ptr<Object>& gameObject) override;
	void RemoveGameObject(const std::shared_ptr<Object>& gameObject) override;

	void OnSwitchIn() override;
	void OnSwitchOut() override;
private:

	float _PowerUpSpawnFrequency = 10.0f;
	float _TimeSincePowerUpSpawn = 1000.0f;
	float _AISpawnFrequency = 0.5;
	float _DefaultAISpawnFrequency = 0.5;
	float _TimeSinceAISpawn = 1000.0f;
};

