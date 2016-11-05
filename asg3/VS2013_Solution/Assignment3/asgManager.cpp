#include "asgManager.h"



asgManager::asgManager()
{}


asgManager::~asgManager()
{
	delete image;
	for (Line* line: primaryLines) {
		delete line;
	}
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

glm::vec3 asgManager::getDirection(glm::vec3 origin, glm::vec3 destination)
{
	glm::vec3 direction(0.0f);
	
	direction.x = destination.x - origin.x;
	direction.y = destination.y - origin.y;
	direction.z = destination.z - origin.z;
	
	return direction;
}

glm::vec3 asgManager::testIntersectionWithSphere(glm::vec3 origin, glm::vec3 direction, Sphere * sphere)
{
	direction = glm::normalize(direction);

	glm::vec3 center = sphere->getPos();
	double radius = sphere->getRadius();
	
	// a = x_d^2 + y_d^2 + z_d^2
	double a = 1.0;
	
	// b = 2*(x_d(x_o - x_c) + y_d(y_o - y_c) + z_d(z_o - z_c))
	double b = 
		2 * (
			direction.x * (origin.x - center.x) +
			direction.y * (origin.y - center.y) +
			direction.z * (origin.z - center.z)
		);


	// c = (x_o-x_c)^2 + (y_o-y_c)^2 + (z_o-z_c)^2 - r^2
	double c =
		pow((origin.x - center.x), 2) +
		pow((origin.y - center.y), 2) +
		pow((origin.z - center.z), 2) -
		pow(radius, 2);

	double delta = pow(b, 2) - 4.0*c;
	double t1, t2;
	double* min;
	if (delta < 0) {
		return glm::vec3(0.0f);
	}
	else if (delta == 0) {
		t1 = -b / 2.0;
		t2 = t1;
		min = &t1;
	}
	else if (delta >0) {
		t1 = (-b + sqrt(delta)) / 2.0;
		t2 = (-b - sqrt(delta)) / 2.0;
		if (t1 > t2)
			min = &t2;
		else
			min = &t1;
	}
	return sphere->getAmb();
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

	image = new cimg_library::CImg<double>(imgWidth, imgHeight, 1, 3, 0);

	glm::vec3 origin = cam->getPos();
	for (int ypos = 0; ypos < imgHeight;++ypos) {
		for (int xpos = 0; xpos < imgWidth;++xpos) {
			glm::vec3 directionVector = getGrid3DPos(xpos, ypos);
			std::vector<GenericObject*>* objList = objHolder.getObjectList();
			for (unsigned int i = 0; i < objList->size();++i) {
				// check if the objectType is neither Camera nor Light
				if ((*objList)[i]->getObjectType() != CAMERA && (*objList)[i]->getObjectType() != LIGHT) {
					
					// is it a Sphere?
					if ((*objList)[i]->getObjectType() == SPHERE) {
						glm::vec3 tgt = testIntersectionWithSphere(origin, directionVector, (Sphere*)((*objList)[i]));
						if (tgt != glm::vec3(0.0))
						{
							const float color[] = { tgt.x,tgt.y,tgt.z};
							image->draw_point(xpos, ypos, 0, color);
						}
					} // IF SPHERE


				} // IF NOT light OR camera

			} // for OBJECTS
			//primaryLines.push_back(new Line(glm::vec3(0.0f), result));
		}
	}
	std::cout << "image made" << std::endl;
	image->save("render.bmp");

	cimg_library::CImgDisplay main_disp(*image, "Render");
	while (!main_disp.is_closed())
		main_disp.wait();

	
}
