#include "DepthStencil.h"
#include "Graphics.h"


DepthStencil::DepthStencil()
{
}


DepthStencil::~DepthStencil()
{
	try
	{
		if (_depthStencilView)
		{
			_depthStencilView->Release();
		}
	}
	catch (const std::exception&)
	{

	}
}

void DepthStencil::Clear(std::shared_ptr<Graphics>& graphics)
{
#ifdef DIRECT_X
	graphics->DeviceContext3D()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
#endif

}

#ifdef DIRECT_X
DepthStencil::DepthStencil(ID3D11Texture2D* const depthStencilBuffer, ID3D11DepthStencilView* const depthStencilView, ID3D11ShaderResourceView* const textureView)
	: Texture(depthStencilBuffer, textureView), _depthStencilView(depthStencilView)
{
}
#endif // DIRECT_X

