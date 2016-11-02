#include "asgManager.h"



asgManager::asgManager()
{}


asgManager::~asgManager()
{}

void asgManager::execute()
{
	std::string filename;
	std::cout << "Input file name: ";
	std::cin >> filename;

	std::cout << "Reading file...";
	ObjectHolder objHolder;
	Parser parse(&objHolder, filename);
	parse.execute();
	std::cout << "done" << std::endl;
	std::cout << "Checking data integrity ... ";
	if (objHolder.getState() == true) {
		std::cout << "good" << std::endl;
	}
	else {
		std::cout << "bad!!!" << std::endl;
	}
}
