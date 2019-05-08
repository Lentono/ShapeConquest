#include "GameObject.h"
#include "../BoxCollider.h"

using namespace glm;


GameObject::GameObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale,glm::vec4 color, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: _Position(position), _Rotation(rotation), _Scale(scale),_Colour(color), _geometry(std::move(geometry)), _rendDesc(std::move(rendDesc))
{
	_RotationMatrix = glm::mat4(1.0);
}
GameObject::GameObject(glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: _Position(position), _Rotation(rotation), _Scale(scale), _geometry(std::move(geometry)), _rendDesc(std::move(rendDesc))
{
	_RotationMatrix = glm::mat4(1.0);
}

GameObject::~GameObject()
= default;

void GameObject::Update(const double dt)
{
	// Add generic update behavior here
}

void GameObject::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
	//glm::quat quaternion = glm::quat(_Rotation);
	glm::mat4 _rotate = glm::mat4_cast(_Rotation);
	_RotationMatrix = _RotationMatrix * _rotate;
	_Rotation = glm::quat(1.0, 0, 0, 0);
	//rotation *= glm::rotate(glm::mat4(1.0), _Rotation.z, glm::vec3(0, 0, 1));
	//rotation *= glm::rotate(glm::mat4(1.0), _Rotation.y, glm::vec3(0, 1, 0));
	//glm::rotate(rotation, _Rotation.z, glm::vec3(0, 0, 1));
	const auto model = translate(glm::mat4(1.0), _Position)  * _RotationMatrix *glm::scale(glm::mat4(1.0), _Scale);
	DefaultConstantBuffer cb{ glm::transpose(model), glm::transpose(camera->View()), glm::transpose(camera->Projection(ProjectionType())), _Colour };
	shader->UpdateBuffer(ShaderTypes::Vertex, &cb, graphics);
	_geometry->Prep(graphics);
	_geometry->Render(graphics);
}

void GameObject::setPosition(const glm::vec3& position)
{
	_Position = position;
}

const glm::vec3& GameObject::getPosition() const
{
	return _Position;
}

void GameObject::setRotation(const glm::quat& rotation)
{
	_Rotation = rotation;
}

const glm::quat& GameObject::getRotation() const
{
	return _Rotation;
}

void GameObject::setScale(const glm::vec3& scale)
{
	_Scale = scale;
}

const glm::vec3& GameObject::getScale() const
{
	return _Scale;
}

void GameObject::AddCollisionCell(const std::shared_ptr<BoxCollider>& newCollisionCell)
{
	_CollisionCells.push_back(newCollisionCell);
}

void GameObject::RemoveFromCollisionCells()
{
	for(auto &i: _CollisionCells)
		i->Remove(shared_from_this());
	_CollisionCells.clear();
}
