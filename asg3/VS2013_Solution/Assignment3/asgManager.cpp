#include "asgManager.h"
#include "utils.h"


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

		interpolant = 1.0 - (2.0*((double)ypos / ((double)imgHeight - 1.0)));
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

glm::vec3 asgManager::testIntersectionWithTriangle(glm::vec3 origin, glm::vec3 direction, Triangle* triangle)
{
	glm::vec3 v1 = triangle->getV1();
	glm::vec3 v2 = triangle->getV2();
	glm::vec3 v3 = triangle->getV3();

	
	// normal = cross product between v1v3 and v1v2
	glm::vec3 normal= glm::cross(v3 - v1, v2 - v1);
	double areaTriangle = normal.length() / 2.0; //we'll need it later for barycentric coordinates

	//normalize the normal
	normal = glm::normalize(normal);

	// triangle is located in plan ax+by+cz+d=0. N=<a,b,c>. The d is by doing dot product between N and any point residing on this plane, so any vertex
	double d = glm::dot(normal, v1);

	
	/*
	Two equations: 
	(1) P_i = origin + t*direction (where P_i is the intersection point)
		and 
	(2) a(P_i.x)+b(P_i.y)+c(P_i.z)+d=0

	Solving for t
	
	*/

	double eqnTop = glm::dot(normal, origin) + d;
	double eqnBot = glm::dot(normal, direction);

	if (fabs(eqnBot) < 0.000001) {
		return glm::vec3(0.0f);
	} // they are parallel
	
	double t = eqnTop / eqnBot; // parameter t, which gives our position of the target
	
	if (normal.z < 0 && eqnBot < 0) // if normal is pointing the same direction as direction vector (in the negative z direction) then our N will result in a negative t although the triangle is in front of us
		t = -t;

	if (t < 0) {
		return glm::vec3(0.0f); // triangle is behind us
	}
	
	// intersection of the ray and the plane on which the triangle resides
	glm::vec3 P = origin + glm::vec3 (t*direction.x, t*direction.y, t*direction.z);
	std::cout << "P=" << P.x << " " << P.y << " " << P.z << std::endl;
	//BARYCENTRIC COORDINATES:

	glm::vec3 v1P = P - v1;
	glm::vec3 v1v2 = v2 - v1;
	double area1 = getMagnitude(glm::cross(v1P, v1v2))/ 2.0;
	double alpha = area1 / areaTriangle;

	glm::vec3 v3P = P - v3;
	glm::vec3 v3v1 = v1 - v3;
	double area2 = getMagnitude(glm::cross(v3P, v3v1)) / 2.0;
	double beta = area2 / areaTriangle;
	
	double gamma = 1 - alpha - beta;

	std::cout << "alpha = " << alpha << std::endl;
	std::cout << "beta = " << beta << std::endl;
	std::cout << "gamma = " << gamma << std::endl;
	
	//double gg = glm::cross((v2 - v3), (P - v3)).length() / areaTriangle;
	
	//check if it is in triangle
	if (
		(alpha > 0) &&
		(beta > 0) &&
		(gamma > 0)
		)
	{
		return triangle->getAmb();
	}
	else {
		return glm::vec3(0.0f);
	}

}

glm::vec3 asgManager::testIntersectionWithPlane(glm::vec3 origin, glm::vec3 direction, Plane * plane)
{
	// equation: t = (pos-origin) . n / direction . n
	double t = glm::dot(plane->getPos(), plane->getNormal()) / glm::dot(direction,plane->getNormal());
	if (t > 0 && t < 50000) {
		std::cout << "t =" << t << std::endl;
		return plane->getAmb();
	}
	return glm::vec3(0.0f);
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

	//debug
	//a=830, b=430
	int a, b;
	std:: cin >> a >> b;
	glm::vec3 directionVector = getGrid3DPos(a, b);
	std::cout << "x=" << directionVector.x << "y=" << directionVector.y << "z=" << directionVector.z << std::endl;
	std::vector<GenericObject*>* objList = objHolder.getObjectList();
	
	for (GenericObject* object : *objList) {
		if (object->getObjectType() == TRIANGLE) {
			std::cout << "found triangle " << std::endl;
			Triangle *t = (Triangle*)object;
			glm::vec3 aa= testIntersectionWithTriangle(origin, directionVector, t);
			std::cout << "Got IT!" << aa.x << " " << aa.y << " " << aa.z << std::endl;
		}
	}
	//end debug

	/*
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
						if (tgt != glm::vec3(0.0)) {
							const float color[] = { tgt.x,tgt.y,tgt.z };
							image->draw_point(xpos, ypos, 0, color);
						}
					} // IF SPHERE

					  // is it a Triangle?
					if ((*objList)[i]->getObjectType() == TRIANGLE) {
						glm::vec3 tgt = testIntersectionWithTriangle(origin, directionVector, (Triangle*)(*objList)[i]);
						if (tgt != glm::vec3(0.0)) {
							const float color[] = { tgt.x,tgt.y,tgt.z };
							image->draw_point(xpos, ypos, 0, color);
						}


						// is it a Triangle?
						if ((*objList)[i]->getObjectType() == PLANE) {
							glm::vec3 tgt = testIntersectionWithPlane(origin, directionVector, (Plane*)(*objList)[i]);
							if (tgt != glm::vec3(0.0)) {
								const float color[] = { tgt.x,tgt.y,tgt.z };
								image->draw_point(xpos, ypos, 0, color);
							}
						}


					} // IF NOT light OR camera

				} // for OBJECTS
				//primaryLines.push_back(new Line(glm::vec3(0.0f), result));
			}
		}
	}
	std::cout << "image made" << std::endl;
	image->save("render.bmp");

	cimg_library::CImgDisplay main_disp(*image, "Render");
	while (!main_disp.is_closed())
		main_disp.wait();
	*/
	
}
