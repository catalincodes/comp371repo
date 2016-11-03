#include "ObjectHolder.h"
#include <iostream>

ObjectHolder::ObjectHolder():
	hasCamera(false), hasLightSrc(false), goodState(false)
{
	objectList = new std::vector<GenericObject*>;
	lightSrc = new std::vector<Light*>;
}

ObjectHolder::~ObjectHolder()
{
	clearList();
	delete objectList;
}

ObjectHolder::ObjectHolder(std::vector<GenericObject*>* givenObjectList):
	objectList(givenObjectList)
{
	lightSrc = new std::vector<Light*>;
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
	//std::cout << "Verifying things..." << std::endl;
	goodState = true; //assume good state

	//reset Camera, Plane and Light src flags to FALSE
	hasCamera = false; 
	hasPlane = false;
	hasLightSrc = false;

	//reset Camera pointer and lightSrc vector of pointers
	camera = nullptr;
	lightSrc->clear();
	lightSrc->resize(0);

	//std::cout << "Cleared lists." << std::endl;
	for (GenericObject* object : *objectList) {
		
		//we can only have one camera
		if (object->getObjectType() == CAMERA) {
			if (hasCamera == false) {
				//std::cout << "found a camera and saving the pointer" << std::endl;
				hasCamera = true;
				camera = (Camera*)object;
			}
			else {
				//std::cout << "too many cameras!!!!!!" << std::endl;
				goodState = false;
			}
				
		}

		// we only have one plane
		if (object->getObjectType() == PLANE) {
			//std::cout << "found a plane" << std::endl;
			if (hasPlane == false)
				hasPlane = true;
			else {
				//std::cout << "too many planes!!!!!!" << std::endl;
				goodState = false;
			}
				
		}

		// we must have a light source
		if (object->getObjectType() == LIGHT) {
			//std::cout << "found a light and adding it to the list" << std::endl;
			lightSrc->push_back((Light*)object);
			if (hasLightSrc == false)
				hasLightSrc = true;
		}
	}

	// while we do note care if we have a plane, we do need to have a light source and a camera
	if (hasCamera == false || hasLightSrc == false)
		goodState = false;
	
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


