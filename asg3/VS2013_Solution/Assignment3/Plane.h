#pragma once
#include "GeometricObject.h"
class Plane :
	public GeometricObject
{
private:
	glm::vec3 normal;
public:
	Plane();
	Plane(glm::vec3 pos, glm::vec3 nor, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi);
	virtual ~Plane();

	//getters and setters
	virtual glm::vec3 getNormal() const { return normal; }
	virtual void setNormal(glm::vec3 newNormal) { normal = newNormal; }
};

