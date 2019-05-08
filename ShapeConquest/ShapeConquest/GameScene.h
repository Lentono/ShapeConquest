#pragma once
#include "Scenes/BaseScene.h"
#include "GameObjects/CollidableObject.h"
#include "GameObjects/Player.h"
#include "GameObjects/ParticleSystem.h"
#include "Bullet.h"
#include "BoxCollider.h"
#include "GameObjects/AI.h"
#include "Managers/ParticleManager.h"
#include "GameObjects/LivesBar.h"
#include "GameObjects/PowerUp.h"
#include "GameObjects/LabledObject.h"

class GameScene : public BaseScene
{
public:
	GameScene(const int screenWidth, const int screenHeight, const float screenBoundsX, const float screenBoundsY);
	~GameScene();

	virtual void OnLoad() override;

	void CollisionDetection();

	virtual void OnUpdateFrame(double deltaTime) override;

	virtual void OnRenderFrame(const std::shared_ptr<Renderer>& renderer, float dt) override;
	void GetPowerUpTextureName(PowerUpType powerUpType, ShootMode shootMode, std::string& powerUpTexture);

	const std::shared_ptr<ParticleManager> GetParticleSystemManager() const { return m_particleSystemManager; };

	//Recalculates the objects position inside the quadtree for collision purposes
	void RecalculateQuadTreePartitionLocation(const std::shared_ptr<CollidableObject>& collidableObject) const;

	void SpawnPowerUp();

	void SpawnAI();
	void SpawnAIAtPosition(const glm::vec3& position, const AIBehaviours behaviour = Undefined);

	void SpawnGravityBomb(const glm::vec3& position);

	void AddBullet(const glm::vec3 position, const glm::vec2 direction,const int playerID, const glm::vec4& playerColour);

	virtual void AddGameObject(const std::shared_ptr<Object>& gameObject) override;

	virtual void RemoveGameObject(const std::shared_ptr<Object>& gameObject) override;

	std::shared_ptr<Player> GetPlayerByID(const int playerID);

	virtual void Reset() override;


	virtual void Shutdown() override;
	void OnSwitchIn() override;
	void OnSwitchOut() override;

protected:

	std::function<void(const int)> m_createPlayerFunction;
	std::function<void(const int)> m_destroyPlayerFunction;

	float _totalTime = 0;
	int _difficultLevel = 0;
	std::vector<std::shared_ptr<CollidableObject>> _collidableObjects;
	std::vector<std::shared_ptr<Player>> _players;
	std::vector<std::shared_ptr<PlayerZone>> m_playerZones;
	std::vector<std::shared_ptr<PowerUp>> m_powerUps;
	std::vector<std::shared_ptr<AI>> _ai;
	std::vector<std::shared_ptr<ParticleSystem>> _particleSystem;
	std::vector<std::shared_ptr<Bullet>> _bullets;
	std::shared_ptr<BoxCollider> _QuadTree;
	std::unordered_map<int, std::shared_ptr<LivesBar>> _uiHealth;

	std::shared_ptr<LabledObject> _dificultyUI;

	std::shared_ptr<ParticleManager> m_particleSystemManager;

	float _spawnGridCellSize = 0.3f;
};
