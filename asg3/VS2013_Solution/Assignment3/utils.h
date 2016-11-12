#include "..\glm\glm.hpp"

double getMagnitude(glm::vec3 inputVec){
	double temp = (double)powf(inputVec.x, 2.0) + powf(inputVec.y, 2.0) + powf(inputVec.z, 2.0);
	return (double)sqrtf(temp);
}