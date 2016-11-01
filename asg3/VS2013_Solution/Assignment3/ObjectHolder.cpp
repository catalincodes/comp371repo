#include "ObjectHolder.h"

ObjectHolder::ObjectHolder():
	hasCamera(false), hasLightSrc(false), goodState(false)
{
	objectList = new std::vector<GenericObject*>;
}

ObjectHolder::~ObjectHolder()
{
	clearList();
	delete objectList;
}

ObjectHolder::ObjectHolder(std::vector<GenericObject*>* givenObjectList):
	objectList(givenObjectList)
{
	verifyState();
}

bool ObjectHolder::clearList()
{
	for (GenericObject* object : *objectList)
		delete object;
	
	objectList->clear();
	objectList->resize(0);
	
	hasCamera = false;
	hasLightSrc = false;
	goodState = false;

	if (objectList->size()==0)
		return true;
	return false;
	
}

bool ObjectHolder::verifyState()
{
	for (GenericObject* object : *objectList) {
		if (object->getObjectType() == CAMERA) {
			if (hasCamera == false)
				hasCamera = true;
			else
				goodState = false;
		}
		if (object->getObjectType() == LIGHT) {
			if (hasLightSrc == false)
				hasLightSrc = true;
		}
	}
	if (hasCamera == false || hasLightSrc == false)
		goodState = false;
	else
		goodState = true;
	return goodState;
}

void ObjectHolder::addCamera(glm::vec3 pos, double fov, double focalLen, double aspect)
{
	Camera* newCamera = new Camera(pos, fov, focalLen, aspect);
	objectList->push_back(newCamera);
	verifyState();
}

void ObjectHolder::addLight(glm::vec3 pos, glm::vec3 color)
{
	Light* newLight = new Light(pos, color);
	objectList->push_back(newLight);
	verifyState();
}

void ObjectHolder::addPlane(glm::vec3 pos, glm::vec3 nor, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi)
{
	Plane* newPlane = new Plane(pos, nor, amb, dif, spe, shi);
	objectList->push_back(newPlane);
	verifyState();
}

void ObjectHolder::addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi)
{
	Triangle* newTriangle = new Triangle(v1, v2, v3, amb, dif, spe, shi);
	objectList->push_back(newTriangle);
	verifyState();
}

void ObjectHolder::addSphere(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi, double radius)
{
	Sphere* newSphere = new Sphere(pos, amb, dif, spe, shi, radius);
	objectList->push_back(newSphere);
	verifyState();
}


