#pragma once
#include <vector>
#include <string>
#include "GenericObject.h"
#include "ObjectHolder.h"

typedef std::vector<GenericObject*>* ObjectListPtr;
class Parser
{
	//ObjectListPtr objectList;
	ObjectHolder* objectList;
	std::string sceneFileName;
	bool testFile();
	void readCamera(std::ifstream& inputStream);
	void readLight(std::ifstream& inputStream);
	void readPlane(std::ifstream& inputStream);
	void readTriangle(std::ifstream& inputStream);
	void readSphere(std::ifstream& inputStream);
public:
	Parser();
	Parser(ObjectHolder* objectList, std::string sceneFileName);
	~Parser();

	void setFileName(std::string newFileName) { sceneFileName = newFileName; }
	void setTargetList(ObjectHolder* targetList) { objectList = targetList; }

	ObjectHolder* getTargetList() { return objectList; }
	std::string getFileName() const { return sceneFileName; }

	bool execute();
};

