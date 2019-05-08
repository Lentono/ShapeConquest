#include "PostProcessingEffect.h"



PostProcessingEffect::PostProcessingEffect(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader)
	: _renderTarget(std::move(renderTarget)), _shader(std::move(shader))
{
}


PostProcessingEffect::~PostProcessingEffect()
{
}


