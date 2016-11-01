#include "Light.h"



Light::Light()
{}

Light::Light(glm::vec3 pos, glm::vec3 color) :
	GenericObject(pos), color(color)
{}


Light::~Light()
{}
