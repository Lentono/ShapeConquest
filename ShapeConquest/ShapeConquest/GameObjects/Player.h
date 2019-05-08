#pragma once
#include "../Managers/InputManager.h"
#include <string>
#include "MoveableObject.h"
//#include "PlayerZone.h"
//#include "PowerUp.h"

class PlayerZone;
class PowerUp;

class Player : public MoveableObject
{
public:
	Player(std::shared_ptr<int> uiHelght, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, bool screenBound, float radius, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	Player(std::shared_ptr<int> uiHelght, std::shared_ptr<int> score, std::shared_ptr<std::vector<wstring>> textureNames, bool screenBound, float radius, float movementSpeed, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	virtual ~Player();

	int getPlayerID() const { return _PlayerID; }
	void setPlayerID(const int id) { _PlayerID = id; }

	bool GetPlayerConnected() const { return m_playerConnected; }
	void SetPlayerConnected(const bool connected) { m_playerConnected = connected; }

	float getScores() const { return _Score; }
	void setScore(const float value) { _Score = value; }
	void addScore(const float delta) { _Score += delta; }

	bool PlayerInSafeZone() const { return m_playerInSafeZone; };

	void RestetPickupUi(const std::wstring& texName) const;

	float getHealth() const { return _Health; }
	void setHealth(const float health) { _Health = health; }

	bool getShielded() const { return _shielded; }
	void setShielded(const bool state) { _shielded = state; }

	void Damage(float damageAmount)	{_Health -= damageAmount;}

	const std::string& getPlayerName() const { return _PlayerName; }
	void setPlayerName(const std::string playerName) { _PlayerName = playerName; }

	virtual void Update(const double dt) override;
	void HandleInput(const double dt);

	virtual void OnCollide(const CollisionData& collisionData) override;


	virtual void ScreenBound(const glm::vec3& collisionVector) override;

	bool SetPowerUp(const std::shared_ptr<PowerUp>& powerUp);

	const std::shared_ptr<PlayerZone>& GetPlayerZone() const;
	void SetPlayerZone(const std::shared_ptr<PlayerZone>& playerZone);

	void DropPowerUp();
	void UpdateUIElement(const PowerUpType& powerUp);

	int GetScore()const { return _Score; };

protected:
	const std::shared_ptr<PowerUp>& GetPowerUp() const;

private:

	int _PlayerID;
	bool m_playerConnected;
	bool m_playerInSafeZone;
	bool m_playerHoldingPowerUp;

	bool _shielded = false;

	std::string _PlayerName;
	glm::vec3 _PreviousPosition;
	glm::vec3 _Forward;

	std::shared_ptr<PowerUp> m_powerUp;
	std::shared_ptr<PlayerZone> m_playerZone;

	float m_timeInSafeZone = 0.0f;
	float m_playerZoneTimeLimit = 2.0f;

	float _defaultMovementSpeed = 5.0f;

	float m_powerUpUseTime = 1.0f;
	float m_timeSincePowerUp = 0.0f;

	float m_timeSinceDropped = 0.0f;
	float m_timeSincePickup = 0.0f;

	float _Radius;
	float _Health = 100.0f;
	float _maxHealth = 100.0f;
	std::shared_ptr<int> _uiHealth;
	std::shared_ptr<int> _uiScore;
	std::shared_ptr<std::vector<std::wstring>> _uiPickUpTextureNames;
	float _Score = 0.0f;

	float _timeSinceLastShot = 0.0f;
	int _bulletsShot = 0;

	float _timeSinceShootModeChanged = 0.0f;
	float _shootPowerUpTime = 5.0f;

	float _timeSinceSpeedBoost = 0.0f;
	float _speedBoostTimeLimit = 5.0f;

	ShootMode _shootMode = ShootMode::SingleShot;
	ShootMode _defaultMode = ShootMode::SingleShot;
};
