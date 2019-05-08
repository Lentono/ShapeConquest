#include "LabledObject.h"


LabledObject::LabledObject(glm::vec4 textColour, std::string text, glm::vec4 objectColour, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	: GameObject(position, rotation, scale, geometry, rendDesc), _text(text, glm::vec2(position.x + 5, position.y), textColour, glm::vec3(1,1,1), scale.x )
{
	_Colour = objectColour;
	float smaller = (_Scale.x < _Scale.y ? _Scale.x : _Scale.y) * 0.7;
	_text.SetScale({ smaller,smaller });
}

void LabledObject::Update(const double dt)
{
	GameObject::Update(dt);
}

void LabledObject::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader,
	std::shared_ptr<Camera> camera)
{
	GameObject::Render(graphics, shader, camera);
	graphics->RenderText(_text);
}

void LabledObject::EditText(const std::string& newText)
{
	_text.SetText(newText);
}

LabledObject::~LabledObject()
{
}
