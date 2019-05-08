#pragma once


#include "GameObjects/CollidableObject.h"

class GravityBomb :	public CollidableObject
{
public:
	GravityBomb(bool screenBound,float radius, glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc = {});
	~GravityBomb();

	virtual void Update(const double dt) override;


	virtual void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;


	virtual void OnCollide(const CollisionData& collisionData) override;

	float _GravityPull = 4.0f;
	bool _Triggered = false;
	float time = 0;
	glm::vec3 _PreviousScaleDelta;

};

