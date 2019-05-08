#include "CompositeObject.h"



CompositeObject::CompositeObject()
{
}


CompositeObject::~CompositeObject()
{
}

void CompositeObject::Update(const double dt)
{
}

void CompositeObject::Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader,
	std::shared_ptr<Camera> camera)
{
}

void CompositeObject::AddObject(const string& refName, const std::shared_ptr<Object>& object)
{
	auto find = _keyObjects.find(refName);
	if(find != _keyObjects.end())
	{
		OutputHandler::ErrorOut("Tried to make a subject object with the same name as a pre existing subject!, BAD! BAD PROGRAMER!", __LINE__, __FILE__);
		return;
	}
	_objects.push_back(object);
	_keyObjects[refName] = object;
}
