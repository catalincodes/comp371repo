#include "Light.h"



Light::Light()
{
	objectType = LIGHT;
}

Light::Light(glm::vec3 pos, glm::vec3 color) :
	GenericObject(pos), color(color)
{
	objectType = LIGHT;
}


Light::~Light()
{}
