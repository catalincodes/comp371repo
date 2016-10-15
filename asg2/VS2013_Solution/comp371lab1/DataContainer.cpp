#include "DataContainer.h"



DataContainer::DataContainer()
{}


DataContainer::~DataContainer()
{}

void DataContainer::addData(GLfloat x, GLfloat y)
{
	container.push_back(x);
	container.push_back(y);
	container.push_back(0.0f);

	container.push_back(0.0f);
	container.push_back(0.5f);
	container.push_back(0.5f);
}

vector<GLfloat> DataContainer::getData()
{
	return container;
}
