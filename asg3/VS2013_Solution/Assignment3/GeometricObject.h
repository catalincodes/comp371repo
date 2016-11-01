#pragma once
#include "GenericObject.h"

class GeometricObject :
	public GenericObject
{
private:
	glm::vec3 amb;
	glm::vec3 dif;
	glm::vec3 spe;
	double shi;
public:

	GeometricObject();
	GeometricObject(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, double shi);
	virtual ~GeometricObject() = 0;

	//getters
	virtual glm::vec3 getAmb() const { return amb; };
	virtual glm::vec3 getDif() const { return dif; };
	virtual glm::vec3 getSpe() const { return spe; };
	virtual double getShi() const { return shi; };

	virtual void setAmb(glm::vec3 newAmb) { amb = newAmb; }
	virtual void setDif(glm::vec3 newDif) { dif = newDif; }
	virtual void setSpe(glm::vec3 newSpe) { spe = newSpe; }
	virtual void setShi(double newShi) { shi = newShi; }

	
};

inline GeometricObject::~GeometricObject() {} // used to provide GeometricObject with a default destructor for when necessary

