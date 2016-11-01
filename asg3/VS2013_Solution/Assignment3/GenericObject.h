#pragma once
#include "..\glm\glm.hpp"

class GenericObject
{
	glm::vec3 pos;
public:
	//CONSTRUCTORS & DESTRUCTORS
	GenericObject();
	virtual ~GenericObject();

	//GETTERS & SETTERS
	virtual void setPos(glm::vec3) = 0;
	virtual glm::vec3 getPos(void) const { return pos; };
};

