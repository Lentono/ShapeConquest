#pragma once
#include <memory>
#include "../Texture.h"
#include "../Graphics.h"
#include "../RenderTarget.h"
#include "../Shader.h"

class PostProcessingEffect
{
public:
	PostProcessingEffect(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader);
	virtual ~PostProcessingEffect();

	virtual void Run(std::shared_ptr<Texture> prevOutput, std::shared_ptr<Graphics> graphics, float dt) = 0;

	const std::shared_ptr<Texture>& GetOutputBuffer() const { return _renderTarget->GetOutput(); }

protected:
	std::shared_ptr<RenderTarget> _renderTarget;
	std::shared_ptr<Shader> _shader;

};

