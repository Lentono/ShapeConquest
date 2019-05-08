#pragma once
#include "GameObject.h"
#include "../Graphics Pipeline/Texture.h"

class TexturedObject :
	public GameObject
{
public:
	TexturedObject(std::shared_ptr<Texture> texture, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc);
	virtual ~TexturedObject();
	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;

	void SetTexture(const std::shared_ptr<Texture>& newTexture) { _texture = newTexture; }

private:
	std::shared_ptr<Texture> _texture;
};

