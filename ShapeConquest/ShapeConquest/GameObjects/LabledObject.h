#pragma once
#include "GameObject.h"


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	Class that instructs the graphics to render text over its position
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
class LabledObject :
	public GameObject
{
public:
	virtual ~LabledObject();
	LabledObject(glm::vec4 textColour, std::string text, glm::vec4 objectColour, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader,
		std::shared_ptr<Camera> camera)  override;
	void EditText(const std::string& newText);
protected:
	TextDescription _text;
};

