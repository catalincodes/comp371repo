#include "GeometricObject.h"



GeometricObject::GeometricObject()
{}

GeometricObject::GeometricObject(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi):
	GenericObject(pos), amb(amb), dif(dif), spe(spe), shi(shi)
{}

