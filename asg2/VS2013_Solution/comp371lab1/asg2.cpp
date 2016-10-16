//HW1 Comp 371 Fall 2016
//Lab 1
//modified from http://learnopengl.com/

#include "stdafx.h"
#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <vector>
#include "DataContainer.h"
//#include "Shader.h"
#include "CatGLEngine.h"

//using namespace std;

typedef unsigned char uchar; // uchar = unsigned char (0..255)

DataContainer* container = nullptr;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;
const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 100.00f;

enum State { INPUT_DATA, RENDER_SPLINE, ANIMATE };
State currentState = INPUT_DATA;
bool needsUpdate = false;

glm::vec3 triangle_scale = glm::vec3 (1.0f);
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);


void normalizeImageArray(GLfloat* img_array, GLuint array_size, GLfloat newMin, GLfloat newMax, GLfloat curMin, GLfloat curMax)
{
	GLuint counter = 0;
	GLfloat scale = (curMax - curMin) / (newMax - newMin);
	GLfloat offset = newMin - curMin;

	do {
		
		img_array[counter++] = (img_array[counter] / scale) + offset;
		img_array[counter++] = (img_array[counter] / scale) + offset;
		img_array[counter++] = (img_array[counter] / scale) + offset;
		counter += 3; // skip the color attributes, until the next vector
	} while (counter < array_size);
}

void normalizeColor(std::vector<GLfloat> &img_array)
{
	GLfloat newMin = 0;
	GLfloat newMax = 1; 
	GLfloat curMin = 0;
	GLfloat curMax = 255;

	GLuint counter = 0;
	GLfloat scale = (curMax - curMin) / (newMax - newMin);
	GLfloat offset = newMin - curMin;

	for (unsigned int i = 0; i < img_array.size(); i += 6){
		img_array[i + 3] = (img_array[i + 3] / scale) + offset;
		img_array[i + 4] = (img_array[i + 4] / scale) + offset;
		img_array[i + 5] = (img_array[i + 5] / scale) + offset;
	}

}


/*
void testImage(cimg_library::CImg<USHORT>& baseImage, cimg_library::CImg<USHORT>& hMapImage)
{
	
	const UINT IMG_ARRAY_SIZE = IMG_WIDTH * IMG_WIDTH * 6;
	const int RED = 0;
	const int GREEN = 1;
	const int BLUE = 2;
	UINT* img_array = new UINT[IMG_ARRAY_SIZE];
	UINT counter = 0;
	try {

		for (UINT row = 0; row < baseImage._height; ++row) {
			for (UINT col = 0; col < baseImage._width; ++col) {
				//std::cout << "WRT in ARRAY at " << counter << "the tollowing (col):" << col << std::endl;
				img_array[counter++] = col;
				//std::cout << "WRT in ARRAY at " << counter << "the following (row):" << row << std::endl;
				img_array[counter++] = row;
				//std::cout << "WRT in ARRAY at " << counter << "the tollowing (hmp):" << *(hMapImage.data(col, row, 0, RED)) << std::endl;
				img_array[counter++] = *(hMapImage.data(col, row, 0, RED));
				//std::cout << "WRT in ARRAY at " << counter << "the tollowing (red):" << *(baseImage.data(col, row, 0, RED)) << std::endl;
				img_array[counter++] = *(hMapImage.data(col, row, 0, RED));
				//std::cout << "WRT in ARRAY at " << counter << "the tollowing (grn):" << *(baseImage.data(col, row, 0, GREEN)) << std::endl;
				img_array[counter++] = *(hMapImage.data(col, row, 0, GREEN));
				if (row == baseImage._height - 1)
				{
					std::cout << "WRT in ARRAY at " << counter << "the tollowing (blu):" << *(baseImage.data(col, row, 0, BLUE)) << std::endl;
				}
				img_array[counter++] = *(hMapImage.data(col, row, 0, BLUE));
			}
		}
	}
	catch (std::exception e) {
		std::cout << "Error : " << e.what() << std::endl;
	}

	delete[] img_array;
}
*/

/*
// Is called whenever an error is encountered
void error_callback(int error, const char* description)
{
	std::cerr << "Error #" << error << " = " << description;
}
*/

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		triangle_scale.x += TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		triangle_scale.x -= TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		triangle_scale.y += TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		triangle_scale.y -= TRIANGLE_MOVEMENT_STEP;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		camera_translation.x += CAMERA_PAN_STEP;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		camera_translation.x -= CAMERA_PAN_STEP;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera_translation.y -= CAMERA_PAN_STEP;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		camera_translation.y += CAMERA_PAN_STEP;

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (container != nullptr && currentState == INPUT_DATA) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			std::cout << "xpos = " << x << "\typos = " << y << std::endl;
			GLfloat xPos = (GLfloat)x;
			GLfloat yPos = (GLfloat)y;
			container->addData(xPos, yPos);
			needsUpdate = true;
			//std::cout << "adding data " << container->getData().size() << endl;
		}
	}

}


std::vector<GLuint> genIndicesVector(GLuint numCols, GLuint numRows)
{
	std::vector<GLuint> indVector;
	for (unsigned int j = 0;	j < (numRows - 1); ++j) 
	{
		for (unsigned int i = 0; i < numCols; ++i)
		{
			
			//TOP TRIANGLE
			indVector.push_back(i + j*numCols);
			indVector.push_back(i + j*numCols + numCols);
			indVector.push_back(i + j*numCols + 1);
			
			//BOTTOM TRIANGLE
			indVector.push_back(i + j*numCols+ 1);
			indVector.push_back(i + j*numCols + numCols);
			indVector.push_back(i + j*numCols + numCols + 1);
		}
	}
	
	return indVector;
}


void testIndVector(std::vector<GLuint> testVec)
{
	for (unsigned int i = 0; i < testVec.size(); i += 3){
		std::cout << testVec[i] << " " << testVec[i + 1] << " " << testVec[i + 2] << "." << std::endl;
	}
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	CatGLEngine engine;

	if (engine.initGL(WIDTH, HEIGHT, key_callback, mouse_button_callback) == -1)
		return -1;
		
	container = new DataContainer();
	container->addData(100.0f, 100.0f);
	container->addData(400.0f, 400.0f);
	

	engine.initVertexObjects();

	engine.sendData(container->getData(), GL_STATIC_DRAW);
		
	//GLuint VAO = engine.getVertexArrayObject();
	GLuint shaderProgram = engine.getShaderProgram();
	GLFWwindow* window = engine.getWindowPtr();

	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");

	// *********************
	// * G A M E   L O O P *
	// *********************
	while (!glfwWindowShouldClose(window)) {
		
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		//glClear(GL_COLOR_BUFFER_BIT);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(5);
	
		switch (currentState) {
		case INPUT_DATA:
		{
			// update GPU data

			if (needsUpdate == true) {
				engine.sendData(container->getData(), GL_STATIC_DRAW);
			}
			engine.renderVBO(GL_POINTS, 0, container->getData().size() / 6);
			break;
		}

		case RENDER_SPLINE:
		{
			break;
		}

		case ANIMATE:
		{
			break;
		}
		}





		


		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		
		
		glm::mat4 projection_matrix;
		//projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
		projection_matrix = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	
	// Terminate GLFW, clearing any resources allocated by GLFW.

	delete container;
	glfwTerminate();
	return 0;
}