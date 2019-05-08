#include "ButtonObject.h"
#include "../AudioManager.h"


ButtonObject::ButtonObject(bool dominant, glm::vec4 textColour, glm::vec4 objectColour, const std::string& text, glm::vec3 position, glm::quat rotation,
	glm::vec3 scale, const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<RenderDescription>& rendDesc)
	: LabledObject(textColour, text, objectColour, position, rotation, scale, geometry, rendDesc), _dominant(dominant)
{
}

void ButtonObject::TryTransferDominance(ButtonNeighbors dir)
{
	const auto neighbor = _neighbors.find(dir);
	if(neighbor != _neighbors.end())
	{
		const auto wNeigh = neighbor->second.lock();
		if(wNeigh)
		{
			wNeigh->_dominant = true;
			wNeigh->_domDelay = true;
			_dominant = false;

			AudioManager::PlaySound("Assets/Audio/MenuSound.wav", false);
			 //TODO:: make highlighted method

			_text.SetColour({ 0,0,0,0.7 }); //TODO:: make highlighted method

		}
	}
}

void ButtonObject::Update(const double dt)
{
	LabledObject::Update(dt);
	if(_dominant)
	{
		if (_domDelay)
		{
			_text.SetColour({ 1,1,1,0.7 });//TODO:: make a de-highlighted method
			_domDelay = false;
			return;
		}
		if(InputManager::QueryButton(0, Button::A) == ButtonState::Pressed)
		{
			if (_onClickSet)
			{
				OnClick();
			}
			return;
		}

		const float sensitivity = 0.5f;
		float x = InputManager::QueryLeftStickX(0);
		if(x > sensitivity || x < -sensitivity)
		{
			if(x > 0)
			{
				TryTransferDominance(Right);
			}
			else
			{
				TryTransferDominance(Left);
			}
		}
		else
		{
			float y = InputManager::QueryLeftStickY(0);
			if(y > sensitivity || y < -sensitivity)
			{
				if (y > 0)
				{
					TryTransferDominance(Up);
				}
				else
				{
					TryTransferDominance(Down);
				}
			}
		}
	}
}

void ButtonObject::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader,
	std::shared_ptr<Camera> camera)
{
	LabledObject::Render(graphics, shader, camera);
}

void ButtonObject::AddNeighbor(ButtonNeighbors dir, const std::weak_ptr<ButtonObject>& button)
{
	if(_neighbors.find(dir) == _neighbors.end())
	{
		_neighbors[dir] = button;
		if(button.lock()->_dominant && _dominant)
		{
			OutputHandler::ErrorOut("Added a dominant button to a button that was already dominant, preferencing this button...", __LINE__, __FILE__);
			button.lock()->_dominant = false;
		}
	}
	else
	{
		OutputHandler::ErrorOut("Tried to overwrite neighbor of button", __LINE__, __FILE__);
	}
}


ButtonObject::~ButtonObject()
= default;

void ButtonObject::SetOnClick(void(* onClick)())
{
	OnClick = onClick;
	_onClickSet = true;
}
