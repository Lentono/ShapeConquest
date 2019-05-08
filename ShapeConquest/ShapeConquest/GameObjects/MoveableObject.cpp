#include "MoveableObject.h"



MoveableObject::MoveableObject(bool screenBound, float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: CollidableObject(screenBound, radius, position, rotation, scale,color, geometry, rendDesc) 
{
	_velocity = glm::vec3(0.0);
}


MoveableObject::MoveableObject(bool screenBound, float radius, float movementSpeed, glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color,
	std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: CollidableObject(screenBound, radius, position, rotation, scale,color, geometry, rendDesc), _MovementSpeed(movementSpeed)
{
	_velocity = glm::vec3(0.0);
}

MoveableObject::~MoveableObject()
{
}

void MoveableObject::Update(const double dt)
{
	CollidableObject::Update(dt);
	moved(false);
}

void MoveableObject::ScreenBound(const glm::vec3& collisionVector)
{
	_Position = _oldPosition ;
	_velocity = glm::reflect(_velocity, collisionVector);
	_Position += glm::normalize(collisionVector) * 0.01f;
}
