#include "Camera.h"
#include <math.h>


Camera::Camera()
{}

Camera::Camera(glm::vec3 pos, double fov, double focalLen, double aspect):
	GenericObject(pos), fov(fov), focalLen(focalLen), aspect(aspect)
{}


Camera::~Camera()
{}

void Camera::getImageSize(double & width, double & height)
{
	double fovY = fov;
	double fovX = aspect*fov;
	width = tan(fovX);
	height = tan(fovY);
}
