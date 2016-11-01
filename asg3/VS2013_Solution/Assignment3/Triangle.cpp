#include "Triangle.h"



Triangle::Triangle()
{
	objectType = TRIANGLE;
}


Triangle::~Triangle()
{
}

Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi):
	GeometricObject(v1,amb, dif, spe, shi), v1(v1), v2(v2), v3(v3)
{
	objectType = TRIANGLE;
}
