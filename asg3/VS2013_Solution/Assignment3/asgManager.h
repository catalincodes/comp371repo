#pragma once

#include "Parser.h"
#include "ObjectHolder.h"
#include <fstream>
#include "..\CImg\CImg.h"
#include <iostream>

struct Line
{
	Line() {}
	Line(glm::vec3 orig, glm::vec3 dir) { this->origin = orig; this->directionVector = dir; }
	
	glm::vec3 origin;
	glm::vec3 directionVector;
};

class asgManager
{
	ObjectHolder objHolder;

	std::vector<Line*> primaryLines;
	
	int imgWidth, imgHeight;	
	cimg_library::CImg<double>* image;

	std::ofstream out;

	glm::vec3 getGrid3DPos(int xpos, int ypos);
	glm::vec3 getDirection(glm::vec3 p1, glm::vec3 p2);

	glm::vec3 testIntersectionWithSphere(glm::vec3 origin, glm::vec3 direction, Sphere* sphere);
	glm::vec3 testIntersectionWithTriangle(glm::vec3 origin, glm::vec3 direction, Triangle* triangle);

	glm::vec3 testIntersectionWithPlane(glm::vec3 origin, glm::vec3 direction, Plane* plane);


public:
	
	asgManager();
	~asgManager();

	void execute();
};

