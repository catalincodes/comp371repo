#include "Parser.h"
#include <fstream>



bool Parser::testFile()
{
	bool isGood = true;
	if (sceneFileName == "")
		return false;
	std::ifstream inputStream(sceneFileName.c_str());
	int i;
	if (inputStream >> i)
		isGood = true;
	else
		isGood = false;
	inputStream.close();
	return isGood;
}

void Parser::readCamera(std::ifstream& inputStream)
{
	glm::vec3 pos;
	double fov, focalLen, aspect;
	std::string buffer;

	inputStream >> buffer;
	if (buffer == "pos:")
		inputStream >> pos.x >> pos.y >> pos.z;

	inputStream >> buffer;
	if (buffer == "fov:")
		inputStream >> fov;

	inputStream >> buffer;
	if (buffer == "f:")
		inputStream >> focalLen;

	inputStream >> buffer;
	if (buffer == "a:")
		inputStream >> aspect;

	objectList->addCamera(pos, fov, focalLen, aspect);
}

void Parser::readLight(std::ifstream& inputStream)
{
	glm::vec3 pos;
	glm::vec3 color;
	std::string buffer;

	inputStream >> buffer;
	if (buffer == "pos:")
		inputStream >> pos.x >> pos.y >> pos.z;

	inputStream >> buffer;
	if (buffer == "col:")
		inputStream >> color.x >> color.y >> color.z;

	objectList->addLight(pos, color);
}

void Parser::readPlane(std::ifstream& inputStream)
{
	glm::vec3 pos, normal, amb, dif, spe;
	double shi;
	std::string buffer;

	inputStream >> buffer;
	if (buffer == "pos:")
		inputStream >> pos.x >> pos.y >> pos.z;

	inputStream >> buffer;
	if (buffer == "amb:")
		inputStream >> amb.x >> amb.y >> amb.z;

	inputStream >> buffer;
	if (buffer == "dif:")
		inputStream >> dif.x >> dif.y >> dif.z;

	inputStream >> buffer;
	if (buffer == "spe:")
		inputStream >> spe.x >> spe.y >> spe.z;

	inputStream >> buffer;
	if (buffer == "shi:")
		inputStream >> shi;

	objectList->addPlane(pos, normal, amb, dif, spe, shi);
}

void Parser::readTriangle(std::ifstream& inputStream)
{
	glm::vec3 v1, v2, v3, amb, dif, spe;
	double shi;
	std::string buffer;

	inputStream >> buffer;
	if (buffer == "v1:")
		inputStream >> v1.x >> v1.y >> v1.z;

	inputStream >> buffer;
	if (buffer == "v2:")
		inputStream >> v2.x >> v2.y >> v2.z;

	inputStream >> buffer;
	if (buffer == "v3:")
		inputStream >> v3.x >> v3.y >> v3.z;

	inputStream >> buffer;
	if (buffer == "amb:")
		inputStream >> amb.x >> amb.y >> amb.z;

	inputStream >> buffer;
	if (buffer == "dif:")
		inputStream >> dif.x >> dif.y >> dif.z;

	inputStream >> buffer;
	if (buffer == "spe:")
		inputStream >> spe.x >> spe.y >> spe.z;

	inputStream >> buffer;
	if (buffer == "shi:")
		inputStream >> shi;

	objectList->addTriangle(v1, v2, v3, amb, dif, spe, shi);
}

void Parser::readSphere(std::ifstream& inputStream)
{
	glm::vec3 pos, amb, dif, spe;
	double radius, shi;
	std::string buffer;

	inputStream >> buffer;
	if (buffer == "pos:")
		inputStream >> pos.x >> pos.y >> pos.z;

	inputStream >> buffer;
	if (buffer == "rad:")
		inputStream >> radius;

	inputStream >> buffer;
	if (buffer == "amb:")
		inputStream >> amb.x >> amb.y >> amb.z;

	inputStream >> buffer;
	if (buffer == "dif:")
		inputStream >> dif.x >> dif.y >> dif.z;

	inputStream >> buffer;
	if (buffer == "spe:")
		inputStream >> spe.x >> spe.y >> spe.z;

	inputStream >> buffer;
	if (buffer == "shi:")
		inputStream >> shi;

	objectList->addSphere(pos, amb, dif, spe, shi, radius);
}

Parser::Parser()
{}

Parser::Parser(ObjectHolder* objectList, std::string sceneFileName) :
	objectList(objectList), sceneFileName(sceneFileName)
{}


Parser::~Parser()
{}

bool Parser::execute()
{
	if (testFile()==false)
		return false;
	
	if (objectList == nullptr)
		return false;

	std::ifstream inputStream(sceneFileName);
	
	//read Nr Objects
	int nrObjects = 0;
	inputStream >> nrObjects;

	for (int i = 0; ((i < nrObjects) && (std::ios::end == false));++i) {
		std::string objTypeStr;
		inputStream >> objTypeStr;
		if (objTypeStr == "camera") {
			readCamera(inputStream);
		}
		else if (objTypeStr == "light") {
			readLight(inputStream);
		}
		else if (objTypeStr == "plane") {
			readPlane(inputStream);
		}
		else if (objTypeStr == "sphere") {
			readSphere(inputStream);
		}
		else if (objTypeStr == "triangle") {
			readTriangle(inputStream);
		}
	}
}
