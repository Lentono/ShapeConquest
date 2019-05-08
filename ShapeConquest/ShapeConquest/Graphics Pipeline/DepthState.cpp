#include "DepthState.h"



DepthState::DepthState()
{
}


DepthState::~DepthState()
{
	if(_depthStencilState)
	{
		_depthStencilState->Release();
	}
}

void DepthState::SetState(std::shared_ptr<Graphics>& graphics) const
{
#ifdef DIRECT_X
	graphics->DeviceContext3D()->OMSetDepthStencilState(_depthStencilState, 1);
#endif

}

#ifdef DIRECT_X
DepthState::DepthState(ID3D11DepthStencilState* const depthStencilState)
	: _depthStencilState(depthStencilState)
{
}
#endif
