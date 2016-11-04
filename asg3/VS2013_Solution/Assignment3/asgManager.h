#pragma once

#include "Parser.h"
#include "ObjectHolder.h"
#include <fstream>
#include "..\CImg\CImg.h"
#include <iostream>

class asgManager
{
	ObjectHolder objHolder;
	int imgWidth, imgHeight;
	cimg_library::CImg<double>* image;

	std::ofstream out;

	glm::vec3 getGrid3DPos(int xpos, int ypos);

public:
	
	asgManager();
	~asgManager();

	void execute();
};

