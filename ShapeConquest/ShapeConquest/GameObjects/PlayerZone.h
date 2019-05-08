#pragma once
#include "CollidableObject.h"
#include "Player.h"
#include "PowerUp.h"

#include "../Utilities/AssetFactory.h"
#include "../Managers/ResourceManager.h"

class PlayerZone : public CollidableObject
{
public:
	PlayerZone(const float radius, const glm::vec3 position, const glm::vec3 scale, const glm::vec4 colour, const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<RenderDescription>& rendDesc);
	PlayerZone(const PlayerZone& other) = delete;
	PlayerZone(PlayerZone&& other) noexcept = delete;
	~PlayerZone();

	PlayerZone& operator = (const PlayerZone& other) = delete;
	PlayerZone& operator = (PlayerZone&& other) noexcept = delete;

	int GetOwnedPlayerID() const;
	void SetOwnedPlayerID(const int playerID);

	void AddPowerUp(const std::shared_ptr<PowerUp>& powerUp);
	const std::shared_ptr<PowerUp>& GetPowerUpAt(const unsigned int at) const;
	const std::vector<shared_ptr<PowerUp>>& GetAllPowerUps() const;
	bool UseAndGetPowerUp(const PowerUpType& powerUpType, std::shared_ptr<PowerUp>& powerUp);
	bool UsePowerUp(const PowerUpType& powerUpType);
	bool HasPowerUp(const PowerUpType& powerUpType, std::string& texture);


	void DropAllPowerUps();

	void ChangeToColourDefault();

	//Check if collision with owning player happened, if we take add the powerup they are currently holding to the vector
	void OnCollide(const CollisionData& collisionData) override;

	//Check if the player is alive, if they die we can release powerups, also apply any affects?
	void Update(const double dt) override;

private:
	//bool al
	int m_ownedPlayerID;
	glm::vec4 m_colourDefault;
	std::vector<shared_ptr<PowerUp>> m_powerUps;
};

