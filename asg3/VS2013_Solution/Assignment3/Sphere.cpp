#include "Sphere.h"



Sphere::Sphere()
{
	objectType = SPHERE;
}

Sphere::Sphere(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi, double radius):
	GeometricObject(pos, amb, dif, spe, shi), radius(radius)
{
	objectType = SPHERE;
}


Sphere::~Sphere()
{}
