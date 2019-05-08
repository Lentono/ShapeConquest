#pragma once

#include "GameObjects/MoveableObject.h"

class Bullet : public MoveableObject
{
public:
	Bullet(const int playerID, const glm::vec2 directionVector, bool screenBound, float radius, const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	~Bullet();

	float getDamage() const { return _Damage; }
	void setDamage(const float damage) { _Damage = damage; }

	int getPlayerID() const { return _OwningPlayerID; }
	void setPlayerID(const int playerID) { _OwningPlayerID = playerID; }

	void StorePlayerColour(const glm::vec4& playerColour) { m_playerColour = playerColour; };

	void OnCollide(const CollisionData& collisionData) override;
	void Update(const double dt) override;



	virtual void ScreenBound(const glm::vec3& collisionVector) override;

protected:
	glm::vec3 m_invertDirection;
	glm::vec4 m_playerColour;
	float _Damage = 100.0f;
	float _bulletSpeed = 5.0f;
	int _OwningPlayerID = -1;
};

