#pragma once
#include "CollidableObject.h"
class MoveableObject : public CollidableObject
{
public:
	MoveableObject(bool screenBound,float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	MoveableObject(bool screenBound, float radius, float movementSpeed, glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	virtual ~MoveableObject();

	virtual void Update(const double dt) override;


	float getMovementSpeed() const { return _MovementSpeed;}
	void setMovementSpeed(float movementSpeed) { _MovementSpeed = movementSpeed; }

	void setVelocity(const glm::vec3& velocity) { _velocity = velocity; }
	glm::vec3 getVelocity() const { return _velocity; }

	virtual void ScreenBound(const glm::vec3& collisionVector) override;

protected:
	// Value should be set from value stored in config file
	float _MovementSpeed = 0;
	glm::vec3 _velocity;
	glm::vec3 _oldPosition = glm::vec3(0.0);
};
