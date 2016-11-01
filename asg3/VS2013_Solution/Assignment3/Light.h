#pragma once
#include "GenericObject.h"
class Light :
	public GenericObject
{
private:
	glm::vec3 color;
public:
	//constructors and destrucors
	Light();
	Light(glm::vec3 pos, glm::vec3 color);
	virtual ~Light();

	//getters and setters
	virtual glm::vec3 getColor() const { return color; }
	virtual void setColor(glm::vec3 newColor) { color = newColor; }
};

