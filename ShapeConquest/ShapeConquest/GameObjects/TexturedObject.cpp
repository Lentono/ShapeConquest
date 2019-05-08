#include "TexturedObject.h"



TexturedObject::TexturedObject(std::shared_ptr<Texture> texture, glm::vec3 position, glm::quat rotation, glm::vec3 scale, std::shared_ptr<Geometry> geometry, std::shared_ptr<RenderDescription> rendDesc)
	:GameObject(position, rotation, scale, geometry, rendDesc), _texture(std::move(texture))
{
}


TexturedObject::~TexturedObject()
{
}

void TexturedObject::Update(const double dt)
{
}

void TexturedObject::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
	if(_texture)
	{
		auto view = _texture->GetView();
		graphics->DeviceContext3D()->PSSetShaderResources(0, 1, &view);
		ID3D11SamplerState * pp_samplers = graphics->DefaultSampleState();
		graphics->DeviceContext3D()->PSSetSamplers(0, 1, &pp_samplers);
	}
	GameObject::Render(graphics, shader, camera);
}
