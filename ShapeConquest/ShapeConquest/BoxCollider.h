#pragma once
#include <vector>
#include "GameObjects/GameObject.h"
#include "GameObjects/CollidableObject.h"

class BoxCollider : public enable_shared_from_this<BoxCollider>
{
public:
	BoxCollider(float width, float height, const glm::vec3& centre, float minSize);
	~BoxCollider();

	glm::vec3 getCentre() const { return _Centre; }

	float getWidth() const { return _Width; }
	float getHeight() const { return _Height; }

	void HandleCollision();
	void Remove(const std::shared_ptr<Object>& toRemove);
	void Add(const std::shared_ptr<CollidableObject>& toAdd);
private:
	glm::vec3 _Centre;
	float _Width, _Height;
	std::vector<std::shared_ptr<Object>> _Objects;
	std::vector<std::shared_ptr<BoxCollider>> _SubColliders;
};

