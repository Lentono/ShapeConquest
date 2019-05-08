#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Texture.h"
#include "DepthStencil.h"
#include "BlendState.h"
#include "DepthState.h"
#include "Graphics.h"
//B.M
//
//Allows targeting of different buffers for rendering
//
class RenderTarget
{
public:
	RenderTarget(std::vector<std::shared_ptr<Texture>> targets, std::shared_ptr<DepthStencil> depthStencil, std::shared_ptr<BlendState> blendState,
		std::shared_ptr<DepthState> depthState, std::vector<glm::vec4> clearColours, bool clearDepthStencil, bool clearColour = true);
	virtual ~RenderTarget();

	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;
	RenderTarget(RenderTarget&&) = delete;
	RenderTarget& operator=(const RenderTarget&&) = delete;

	void Prep(std::shared_ptr<Graphics>& graphics);//Sent as current target to render to

	const std::shared_ptr<Texture>& GetOutput(int index = 0) { return _targets[index]; }

private:
	std::vector<std::shared_ptr<Texture>> _targets;
	std::shared_ptr<DepthStencil> _depthStencil;
	std::shared_ptr<DepthState> _depthState;
	std::shared_ptr<BlendState> _blendState;
	std::vector<glm::vec4> _clearColours;
	bool _clearDepthStencil;
	bool _clearColour;
};

