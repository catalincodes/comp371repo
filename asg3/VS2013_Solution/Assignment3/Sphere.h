#pragma once
#include "GeometricObject.h"
class Sphere :
	public GeometricObject
{
private:
	double radius;
public:
	//constructors and destructors
	Sphere();
	Sphere(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi, double radius);
	virtual ~Sphere();

	//getters and setters
	double getRadius() const { return radius; }
	void setRadius(double newRadius) { radius = newRadius; }

};

