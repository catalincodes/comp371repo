#include "DataContainer.h"



DataContainer::DataContainer()
{}


DataContainer::~DataContainer()
{}

void DataContainer::addData(GLfloat x, GLfloat y)
{
	containerData.push_back(x);
	containerData.push_back(y);
	containerData.push_back(0.0f);

	containerData.push_back(0.0f);
	containerData.push_back(0.5f);
	containerData.push_back(0.5f);
}

vector<GLfloat> DataContainer::getData()
{
	return containerData;
}

bool DataContainer::clearData()
{
	containerData.clear();
	containerData.resize(0);
	if (containerData.size() != 0)
	{
		return false;
	}
	return true;	
}
