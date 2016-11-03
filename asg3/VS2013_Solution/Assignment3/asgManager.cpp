#include "asgManager.h"



asgManager::asgManager()
{}


asgManager::~asgManager()
{}

void asgManager::execute()
{
	std::string filename;
	std::cout << "Input file name: ";
	std::cin >> filename;

	ObjectHolder objHolder;
	Parser parse(&objHolder, filename);
	
	std::cout << "Reading file...";
	parse.execute();
	std::cout << "done" << std::endl;
	
	std::cout << "Checking data integrity ... ";
	if (objHolder.getState() == true) {
		std::cout << "good" << std::endl;
	}
	else {
		std::cout << "bad!!!" << std::endl;
	}

	std::cout << "Image size: ";


	Camera* cam = objHolder.getCamera();
	int width = 0, height = 0;
	cam->getImageSize(width, height);
	std::cout << "width = " << width << " height = " << height << std::endl;
}
