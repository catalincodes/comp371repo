#include "Plane.h"



Plane::Plane()
{
	objectType = PLANE;
}

Plane::Plane(glm::vec3 pos, glm::vec3 nor, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi) :
	GeometricObject(pos, amb, dif, spe, shi), normal(nor)
{
	objectType = PLANE;
}


Plane::~Plane()
{}
