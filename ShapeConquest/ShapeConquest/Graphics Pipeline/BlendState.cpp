#include "BlendState.h"
#include "Graphics.h"


BlendState::BlendState()
{
}


BlendState::~BlendState()
{
	if (_blendState)
	{
		_blendState->Release();
	}
}

void BlendState::Load(const std::shared_ptr<Graphics>& graphics)
{
#ifdef DIRECT_X
	graphics->DeviceContext3D()->OMSetBlendState(_blendState, &_blendFactors[0], _sampleMask);
#endif
}

#ifdef DIRECT_X
BlendState::BlendState(ID3D11BlendState* blend_state, std::vector<float> blendFactors, unsigned int sampleMask)
	:_blendState(blend_state), _blendFactors(std::move(blendFactors)), _sampleMask(sampleMask)
{

}
#endif // DIRECT_X

