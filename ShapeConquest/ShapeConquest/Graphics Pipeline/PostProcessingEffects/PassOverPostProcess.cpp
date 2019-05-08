#include "PassOverPostProcess.h"
#include "../../Managers/ResourceManager.h"
#include <glm/gtc/matrix_transform.hpp>


PassOverPostProcess::PassOverPostProcess(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader)
	: PostProcessingEffect(renderTarget, shader)
{
}


PassOverPostProcess::~PassOverPostProcess()
{
}

void PassOverPostProcess::Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt)
{
	_renderTarget->Prep(graphics);
	_shader->Load(graphics);
	auto view = prevOutput->GetView();
	graphics->DeviceContext3D()->PSSetShaderResources(0, 1, &view);
	ID3D11SamplerState * pp_samplers = graphics->DefaultSampleState();
	graphics->DeviceContext3D()->PSSetSamplers(0, 1, &pp_samplers);
	ResourceManager::GetGeometry("PPEdefault")->Render(graphics);
}
