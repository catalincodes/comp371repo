#pragma once
#include <vector>
#include <glew.h>

using std::vector;

class DataContainer
{
public:
	DataContainer();
	~DataContainer();

	void addData(GLfloat x, GLfloat y);
	vector<GLfloat> getData();
private:
	vector<GLfloat> container;
};

