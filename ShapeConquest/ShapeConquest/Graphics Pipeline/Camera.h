#pragma once
#include <glm/glm.hpp>
class Camera
{
public:
	enum Types
	{
		Perspective,
		Orthographic
	};

public:
	Camera(glm::mat4 view, glm::mat4 perspective, glm::mat4 orthographic);
	~Camera();
	void SetView(glm::mat4 view);
	void SetProjection(Types type, glm::mat4 projection);
	const glm::mat4& View()const { return _view; }
	const glm::mat4& Projection(Types type)const { return type == Perspective ? _perspective : _orthographic; }
private:
	glm::mat4 _view;
	glm::mat4 _perspective;
	glm::mat4 _orthographic;

	
};

