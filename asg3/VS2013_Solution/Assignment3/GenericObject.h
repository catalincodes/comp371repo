#pragma once
#include "..\glm\glm.hpp"

enum ObjectType {CAMERA, LIGHT, PLANE, SPHERE, TRIANGLE };

class GenericObject
{
protected:
	ObjectType objectType;
private:
	glm::vec3 pos;
public:
	//CONSTRUCTORS & DESTRUCTORS
	GenericObject();
	GenericObject(glm::vec3 pos);
	virtual ~GenericObject() = 0;

	//GETTERS & SETTERS
	virtual void setPos(glm::vec3 newPos) { pos = newPos; }
	virtual ObjectType getObjectType() { return objectType; }
	virtual glm::vec3 getPos(void) const { return pos; }
};

inline GenericObject::~GenericObject() {} // used to provide GenericObject with a default destructor for when necessary