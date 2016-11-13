#pragma once

#include "Parser.h"
#include "ObjectHolder.h"
#include <fstream>
#include "..\CImg\CImg.h"
#include <iostream>

const double MAX_FAR = std::numeric_limits<double>::max();

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

	glm::vec3 testIntersectionWithSphere(const glm::vec3 origin , glm::vec3 direction, Sphere* sphere, double& distance);
	glm::vec3 testIntersectionWithTriangle(const glm::vec3 origin , glm::vec3 direction, Triangle* triangle, double& distance);
	glm::vec3 testIntersectionWithPlane(const glm::vec3 origin, glm::vec3 direction, Plane* plane, double& distance);

public:
	
	asgManager();
	~asgManager();

	void execute();
};

