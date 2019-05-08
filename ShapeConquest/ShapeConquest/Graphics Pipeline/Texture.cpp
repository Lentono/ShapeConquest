#include "Texture.h"


Texture::Texture()
{
}


Texture::~Texture()
{
#ifdef DIRECT_X
	try
	{
		if (_textureView)
		{
			_textureView->Release();
		}
		if (_texture)
		{
			_texture->Release();
		}
		if (_renderTarget)
		{
			_renderTarget->Release();
		}
	}
	catch (const std::exception&)
	{

	}
#endif
}

#ifdef DIRECT_X
Texture::Texture(ID3D11Resource* const texture, ID3D11ShaderResourceView* const textureView) : _texture(texture),
_textureView(textureView), _renderTarget(nullptr)
{
}

Texture::Texture(ID3D11Resource* const texture, ID3D11ShaderResourceView* const textureView, ID3D11RenderTargetView* const renderTarget)
	: _texture(texture), _textureView(textureView), _renderTarget(renderTarget)
{
}
#endif // DIRECT_X

