#pragma once
#include "GenericObject.h"
class Camera :
	public GenericObject
{
private:
	double fov;
	double focalLen;
	double aspect;
public:
	//constructors and destructors
	Camera();
	Camera(glm::vec3 pos, double fov, double focalLen, double aspect);
	virtual ~Camera();

	//getters
	double getFov() const { return fov; }
	double getFocalLen() const { return focalLen; }
	double getAspectRatio() const { return aspect; }
	void getImageSize(double &width, double &height);

	//setters
	void setFov(double newFov) { fov = newFov; }
	void setFocalLen(double newFL) { focalLen = newFL; }
	void setAspectRatio(double newAspect) { aspect = newAspect; }

	
};

