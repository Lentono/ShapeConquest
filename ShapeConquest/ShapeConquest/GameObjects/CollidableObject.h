#pragma once
#include "GameObject.h"
#include "../GlobalStructures.h"

class CollidableObject;


struct CollisionData {
	std::shared_ptr<CollidableObject> CollidedObjects[2];
	glm::vec3 CollisionPoint;
	glm::vec3 ContactNormal;
};

enum class EntityCategory
{
	AI = 1,
	PowerUp = 2,
	Player = 4,
	Bullet = 8,
	Bomb = 16,
	PlayerZone = 32,
	Default = 64
};

class CollidableObject : public GameObject
{
public:

	CollidableObject(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	virtual ~CollidableObject();

	virtual void Update(const double dt) override;
	virtual void OnCollide(const CollisionData& collisionData) = 0;
	virtual void ScreenBound(const glm::vec3& collisionVector);

	float getRadius() const { return _Radius; }
	void setRadius(const float radius) { _Radius = radius; }

	bool getScreenBound() const { return _screenBound; }
	void setScreenBound(const bool state) { _screenBound = state; }

	short getEntityType() const { return _entityType; }
	short getCollisionMask() const { return _collisionMask; }
	const std::shared_ptr<GlobalStructures::SpawnCell>& GetSpawnCell() const;
	void SetSpawnCell(const std::shared_ptr<GlobalStructures::SpawnCell>& spawnCell);

protected:
	bool _collided = false;
	bool _screenBound = true;

	short _entityType = static_cast<glm::uint16>(EntityCategory::Default);
	short _collisionMask;
private:
	float _Radius;

	std::shared_ptr<GlobalStructures::SpawnCell> m_spawnCell;
};
