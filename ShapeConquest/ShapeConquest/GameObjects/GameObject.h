#pragma once
#include <string>
#include <vector>
#include <utility>
#include "../Graphics Pipeline/Geometry.h"
#include "../Graphics Pipeline/Graphics.h"
#include "../Graphics Pipeline/Camera.h"
#include "../Graphics Pipeline/Shader.h"
#include "../Graphics Pipeline/ShaderEnum.h"
#include "../Graphics Pipeline/RenderDescription.h"
#include "../Graphics Pipeline/BufferTypes.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Object.h"

class BoxCollider;

//B.M
//
//Just a very basic implementation, Needs more added
//
class GameObject : public Object
{
public:
	GameObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec4 color,std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	GameObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	virtual ~GameObject();
	const std::vector<std::string>& RenderTargets() const { return _rendDesc->RenderTargets(); }
	ShaderEnum ShaderToUse() const { return _rendDesc->ShaderToUse(); }
	const std::shared_ptr<Geometry>& GetGeometry()const { return _geometry; }
	Camera::Types ProjectionType() const { return _rendDesc->ProjectionType(); }
	const std::string& CameraReferrence()const { return _rendDesc->CameraReferrence(); }

	virtual void Update(const double dt);
	virtual void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera);

	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition() const;

	void setColour(const glm::vec4& colour) { _Colour = colour; }
	glm::vec4 getColour() const { return _Colour; }

	void setRotation(const glm::quat& rotation);
	const glm::quat& getRotation() const;

	void setScale(const glm::vec3& scale);
	const glm::vec3& getScale() const;

	void moved(const bool state) { _moved = state; }
	bool moved() const { return _moved; }

	std::shared_ptr<Geometry> getGeometry() { return _geometry; }
	void setGeometry(const std::shared_ptr<Geometry>& geometry) { _geometry = geometry; }

	void AddCollisionCell(const std::shared_ptr<BoxCollider>& newCollisionCell);
	void RemoveFromCollisionCells();
protected:
	void SetGeometry(std::shared_ptr<Geometry> geometry) { _geometry = geometry; }

protected:
	glm::vec3 _Position;
	glm::mat4 _RotationMatrix;
	glm::quat _Rotation;
	glm::vec3 _Scale;
	glm::vec3 _Direction;
	glm::vec4 _Colour = glm::vec4(0,0,0,1.0);
private:
	std::shared_ptr<Geometry> _geometry;
	std::shared_ptr<RenderDescription> _rendDesc;
	bool _moved = false;
	std::vector<std::shared_ptr<BoxCollider>>  _CollisionCells;
};
