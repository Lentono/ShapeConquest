#pragma once
#include <memory>
#include "../Graphics Pipeline/Graphics.h"
#include "../Graphics Pipeline/Shader.h"
#include "../Graphics Pipeline/Camera.h"


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	base object that has pure virtual render and update methods
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();


	virtual void Update(const double dt) = 0;
	virtual void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) = 0;


	void alive(const bool state) { _alive = state; }
	bool alive() const { return _alive; }

private:
	bool _alive = true;
};

