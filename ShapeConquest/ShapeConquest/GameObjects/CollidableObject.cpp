#include "CollidableObject.h"



CollidableObject::CollidableObject(bool screenBound,float radius,  glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: GameObject(position, rotation, scale,color, geometry, rendDesc), _Radius(radius), _screenBound(screenBound)
{
}


CollidableObject::~CollidableObject()
{
}

void CollidableObject::Update(const double dt)
{
	GameObject::Update(dt);
}

void CollidableObject::ScreenBound(const glm::vec3& collisionVector)
{

}

const std::shared_ptr<GlobalStructures::SpawnCell>& CollidableObject::GetSpawnCell() const
{
	return m_spawnCell;
}

void CollidableObject::SetSpawnCell(const std::shared_ptr<GlobalStructures::SpawnCell>& spawnCell)
{
	m_spawnCell = spawnCell;
}

