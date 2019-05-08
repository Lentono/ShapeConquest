#include "Camera.h"


Camera::Camera(glm::mat4 view, glm::mat4 perspective, glm::mat4 orthographic)
	: _view(view), _perspective(perspective), _orthographic(orthographic)
{
}

Camera::~Camera()
{
}

void Camera::SetView(glm::mat4 view)
{
	_view = view;
}

void Camera::SetProjection(Types type, glm::mat4 projection)
{
	if (type == Types::Perspective)
	{
		_perspective = projection;
		return;
	}
	_orthographic = projection;
}
