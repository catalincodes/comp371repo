#include "asgManager.h"



asgManager::asgManager()
{}


asgManager::~asgManager()
{
	delete image;
}

glm::vec3 asgManager::getGrid3DPos(int xpos, int ypos)
{
	Camera* camera = objHolder.getCamera();

	if (camera) {
		double x, y;

		double f = camera->getFocalLen();
		double a = camera->getAspectRatio();
		double fov = camera->getFov();

		double interpolant = -1.0 + (2.0*((double)xpos / ((double)imgWidth - 1.0)));
		x = interpolant * f * a * tan((fov / 2.0)*3.1415 / 180);

		interpolant = -1.0 + (2.0*((double)ypos / ((double)imgHeight - 1.0)));
		y = interpolant * f * tan((fov / 2.0)*3.1415 / 180);

		return glm::vec3(x, y, -f);
	}
	else {
		return glm::vec3(0.0f);
	}
}


void asgManager::execute()
{
	std::string filename;
	std::cout << "Input file name: ";
	std::cin >> filename;

	//ObjectHolder objHolder;
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
	
	cam->getImageSize(imgWidth, imgHeight);
	std::cout << "width = " << imgWidth << " height = " << imgHeight << std::endl;

	image = new cimg_library::CImg<double>(imgWidth, imgHeight, 0, 3, 0);

	out = std::ofstream("o_" + filename);
	for (int ypos = 0; ypos < imgHeight;++ypos) {
		for (int xpos = 0; xpos < imgWidth;++xpos) {
			glm::vec3 result = getGrid3DPos(xpos, ypos);
			double x = result.x;
			double y = result.y;
			double z = result.z;
			out << "(" << x << ", " << y << ", " << z << ")   ";
		}
		out << std::endl;
	}
	out.close();
}
