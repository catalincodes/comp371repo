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

Parser::Parser()
{}

Parser::Parser(ObjectListPtr objectList, std::string sceneFileName):
	objectList(objectList), sceneFileName(sceneFileName)
{}


Parser::~Parser()
{}

bool Parser::execute()
{
	return false;
}
