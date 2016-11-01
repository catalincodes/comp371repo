#pragma once
#include <vector>
#include <string>
#include "GenericObject.h"

typedef std::vector<GenericObject*>* ObjectListPtr;
class Parser
{
	ObjectListPtr objectList;
	std::string sceneFileName;
	
public:
	Parser();
	Parser(ObjectListPtr objectList, std::string sceneFileName);
	~Parser();
	bool testFile();

	void setFileName(std::string newFileName) { sceneFileName = newFileName; }
	void setTargetList(ObjectListPtr targetList) { objectList = targetList; }

	ObjectListPtr getTargetList() { return objectList; }
	std::string getFileName() const { return sceneFileName; }

	bool execute();
};

