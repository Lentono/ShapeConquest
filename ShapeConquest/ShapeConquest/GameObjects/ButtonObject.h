#pragma once
#include "LabledObject.h"
#include "../Managers/InputManager.h"

enum ButtonNeighbors
{
	Up,
	Down,
	Left,
	Right
};


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	Class that makes up a button array
//	Buttons reference their neighbors in the spacial direction relative to themselves
//	There should be one dominant button in the array (you'll get an error if there is more than one, and they won't update if there is none)
//	The dominance is transfered when the user moves to select a button in a direction that the dominant has a neighbor
//	The button has a function pointer that can be set to a void f(void) function, this is called when the button is dominant and the a button is pressed
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
class ButtonObject :
	public LabledObject
{
public:

	ButtonObject(bool dominant, glm::vec4 textColour, glm::vec4 objectColour, const std::string & text, glm::vec3 position, glm::quat rotation, glm::vec3 scale, const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<RenderDescription>& rendDesc);
	void TryTransferDominance(ButtonNeighbors dir);
	virtual ~ButtonObject();
	void SetOnClick(void(*onClick)());
	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader,
		std::shared_ptr<Camera> camera) override;
	void AddNeighbor(ButtonNeighbors dir, const std::weak_ptr<ButtonObject>& button);

private:
	std::map<ButtonNeighbors, std::weak_ptr<ButtonObject>> _neighbors;
	bool _dominant = false;
	bool _domDelay = false;
	void(*OnClick) (void) = nullptr;
	bool _onClickSet = false;
};

