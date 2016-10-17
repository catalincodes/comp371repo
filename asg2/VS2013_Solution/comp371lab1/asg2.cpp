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
#include "CatGLEngine.h"


typedef unsigned char uchar; // uchar = unsigned char (0..255)

DataContainer* container = nullptr;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

enum State { INPUT_DATA, RENDER_SPLINE, ANIMATE };
State currentState = INPUT_DATA;
bool needsUpdate = false;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
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


// The MAIN function, from here we start the application and run the game loop
int main()
{
	CatGLEngine engine;

	if (engine.initGL(WIDTH, HEIGHT, key_callback, mouse_button_callback) == -1)
		return -1;
		
	container = new DataContainer();

	engine.initVertexObjects();
		
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
		
		glm::mat4 view_matrix;
				
		glm::mat4 projection_matrix;
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