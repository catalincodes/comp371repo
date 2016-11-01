#include "main.h"
#include <iostream>
#include "Plane.h"

int main()
{
	GenericObject* go = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), 0.0);
	Plane* pla = (Plane*)go;
	pla->getShi();
	return EXIT_SUCCESS;
}