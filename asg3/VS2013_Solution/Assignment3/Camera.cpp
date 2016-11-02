#include "Camera.h"
#include <math.h>


Camera::Camera()
{
	objectType = CAMERA;
}

Camera::Camera(glm::vec3 pos, double fov, double focalLen, double aspect):
	GenericObject(pos), fov(fov), focalLen(focalLen), aspect(aspect)
{
	objectType = CAMERA;
}


Camera::~Camera()
{}

void Camera::getImageSize(int & width, int & height)
{
	double fovR = fov * 3.1415 / 180.0;
	
	// equation:	tan(fov/2) = (h/2) / f   ==>   h=2 * f * tan(fov/2)
	height = (int)(2 * focalLen * tan(fovR / 2.0));
	width = (int)(aspect *  height);
}
