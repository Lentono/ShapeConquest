#pragma once
#include "Object.h"
#include <unordered_map>


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	A class for referrencing multple game objects for controling group logic.
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
class CompositeObject : public Object
{
public:
	CompositeObject();
	virtual ~CompositeObject();

	CompositeObject(const CompositeObject&) = delete;
	CompositeObject& operator=(const CompositeObject&) = delete;
	CompositeObject(CompositeObject&&) = delete;
	CompositeObject& operator=(const CompositeObject&&) = delete;
	
	void Update(const double dt) override;
	void Render(std::shared_ptr<Graphics>& graphics, std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera) override;


	const std::vector<std::shared_ptr<Object>>& GetSubjects()const { return _objects; }

protected:
	void AddObject(const string& refName, const std::shared_ptr<Object>& object);
	std::unordered_map<std::string, std::shared_ptr<Object>> _keyObjects;
	std::vector<std::shared_ptr<Object>> _objects;

};

