#pragma once
#include "GeometricObject.h"
class Triangle :
	public GeometricObject
{
private:
	glm::vec3 v1, v2, v3;
public:

	//constructors and destructors
	Triangle();
	virtual ~Triangle();
	Triangle(
		glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
		glm::vec3 amb, glm::vec3 dif, glm::vec3 spe,
		double shi);

	//getters 
	glm::vec3 getV1() const { return v1; }
	glm::vec3 getV2() const { return v2; }
	glm::vec3 getV3() const { return v3; }

	//setters
	void setV1(glm::vec3 newV1) { v1 = newV1; }
	void setV2(glm::vec3 newV2) { v1 = newV2; }
	void setV3(glm::vec3 newV3) { v1 = newV3; }
};

