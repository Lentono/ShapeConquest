#include "BlurPostProcess.h"
#include "../../Managers/ResourceManager.h"


BlurPostProcess::BlurPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader)
	: PostProcessingEffect(renderTarget, shader)
{
}


BlurPostProcess::~BlurPostProcess()
{
}

void BlurPostProcess::Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt)
{
	_renderTarget->Prep(graphics);
	_shader->Load(graphics);

	BlurCB cb;
	cb.dimensions = glm::vec2(graphics->Width(), graphics->Height());
	cb.blurDepth = 5;
	_shader->UpdateBuffer(ShaderTypes::Pixel, &cb, graphics);

	auto view = prevOutput->GetView();
	graphics->DeviceContext3D()->PSSetShaderResources(0, 1, &view);
	ID3D11SamplerState * pp_samplers = graphics->DefaultSampleState();
	graphics->DeviceContext3D()->PSSetSamplers(0, 1, &pp_samplers);
	ResourceManager::GetGeometry("PPEdefault")->Render(graphics);
}
