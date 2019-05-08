#include "RenderTarget.h"
#include <utility>

RenderTarget::RenderTarget(std::vector<std::shared_ptr<Texture>> targets, std::shared_ptr<DepthStencil> depthStencil, 
	std::shared_ptr<BlendState> blendState, std::shared_ptr<DepthState> depthState, std::vector<glm::vec4> clearColours, bool clearDepthStencil, bool clearColour)
	:_targets(std::move(targets)), _depthStencil(std::move(depthStencil)), _depthState(std::move(depthState)), 
	_blendState(std::move(blendState)), _clearColours( std::move(clearColours)), _clearDepthStencil(clearDepthStencil), _clearColour(clearColour)
{
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Prep(std::shared_ptr<Graphics>& graphics)
{
	_depthState->SetState(graphics);
	if(_clearDepthStencil)
	{
		_depthStencil->Clear(graphics);
	}
	_blendState->Load(graphics);

#ifdef DIRECT_X
	std::vector<ID3D11RenderTargetView*> targets;
	auto deviceContext = graphics->DeviceContext3D();
	for (unsigned int i = 0; i < _targets.size(); i++)
	{
		ID3D11RenderTargetView* const render_target = _targets[i]->GetRenderTarget();
		if (_clearColour)
		{
			glm::vec4 clear_colour = _clearColours[i];
			float data[]{ clear_colour.x, clear_colour.y,clear_colour.z,clear_colour.w };
			deviceContext->ClearRenderTargetView(render_target, data);
		}
		targets.push_back(render_target);
	}
	deviceContext->OMSetRenderTargets(static_cast<UINT>(targets.size()), targets.data(), _depthStencil->DepthStencilView());
#endif

}
