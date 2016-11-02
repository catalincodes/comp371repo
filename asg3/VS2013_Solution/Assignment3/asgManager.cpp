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
	if (objHolder.getState() == true) {
		std::cout << "File loaded correctly and state is good!" << std::endl;
	}
	else {
		std::cout << "File not loaded correctly" << std::endl;
	}
	system("pause");
}
