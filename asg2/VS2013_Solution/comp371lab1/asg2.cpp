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

glm::vec3 catmullRomFunc(GLfloat u, glm::mat4 controlMatrix);
void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, glm::mat4 currentControlMatrix);

typedef unsigned char uchar; // uchar = unsigned char (0..255)

DataContainer* container = nullptr;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

bool booya = false;


enum State { INPUT_DATA, RENDER_SPLINE, ANIMATE };
State currentState = INPUT_DATA;
bool needsUpdate = false;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && currentState == INPUT_DATA) 
	{
		currentState = RENDER_SPLINE;
	}
	
	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		
		container->clearData();
		currentState = INPUT_DATA;
		needsUpdate = true;
	}
		
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
				engine.sendData(container->getData(), GL_DYNAMIC_DRAW);
			}
			engine.renderVBO(GL_POINTS, 0, container->getData().size() / 6);
			break;
		}

		case RENDER_SPLINE:
		{
			if (container->getData().size() >= 4 && booya == false) {
				//generateSpline();
				booya = true;
			}
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
/*
void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, glm::mat4 currentControlMatrix)
{
	GLfloat uMid = (GLfloat)((u0 + u1) / 2.0f);
	glm::vec3 x0 = catmullRomFunc(u0, currentControlMatrix);
}

*/

glm::vec3 catmullRomFunc(GLfloat u, glm::mat4 controlMatrix)
{
	vector<GLfloat> v = container->getData();

	//GLfloat u = 1;
	GLfloat u_2 = pow(u, 2);
	GLfloat u_3 = pow(u, 3);

	GLfloat s = 0.5; //tension parameter

	// result of multiplying the input [u^3, u^2, u^1, 1] by the basis matrix 
	GLfloat b1 = u_3*(0 - s) + u_2 * (2 * s) + u * (0 - s);
	GLfloat b2 = u_3*(2 - s) + u_2 * (s - 3) + 1;
	GLfloat b3 = u_3*(s - 2) + u_2 * (3 - 2 * s) + u*s;
	GLfloat b4 = u_3*(s) + u_2*(0 - s);
	
	// multiply the vector from the previous calculation [b1, b2, b3, b4] by the control matrix
	GLfloat x = b1*v.at(0) + b2*v.at(6) + b3*v.at(12) + b4*v.at(18);
	GLfloat y = b1*v.at(1) + b2*v.at(7) + b3*v.at(13) + b4*v.at(19);
	GLfloat z = b1*v.at(2) + b2*v.at(8) + b3*v.at(14) + b4*v.at(20);

	/*
	glm::mat4 control = glm::mat4(
		glm::vec4(v.at(0), v.at(1), v.at(2), 0),
		glm::vec4(v.at(6), v.at(7), v.at(8), 0),
		glm::vec4(v.at(12), v.at(13), v.at(14), 0),
		glm::vec4(v.at(18), v.at(19), v.at(20), 0)
		);
	
	std::cout << "CONTROL:" << std::endl;
	std::cout << control[0][0] << " " << control[0][1] << " " << control[0][2] << " " << control[0][3] << std::endl;
	std::cout << control[1][0] << " " << control[1][1] << " " << control[1][2] << " " << control[1][3] << std::endl;
	std::cout << control[2][0] << " " << control[2][1] << " " << control[2][2] << " " << control[2][3] << std::endl;
	std::cout << control[3][0] << " " << control[3][1] << " " << control[3][2] << " " << control[3][3] << std::endl;

//	GLfloat s = 0.5;
	glm::mat4 basis = glm::mat4(
		glm::vec4(-s, 2 - s, s - 2, s),
		glm::vec4(2 * s, s - 3, 3 - 2 * s, -s),
		glm::vec4(-s, 0, s, 0),
		glm::vec4(0, 1, 0, 0)
		);
	
	std::cout << "BASIS:" << std::endl;
	std::cout << basis[0][0] << " " << basis[0][1] << " " << basis[0][2] << " " << basis[0][3] << std::endl;
	std::cout << basis[1][0] << " " << basis[1][1] << " " << basis[1][2] << " " << basis[1][3] << std::endl;
	std::cout << basis[2][0] << " " << basis[2][1] << " " << basis[2][2] << " " << basis[2][3] << std::endl;
	std::cout << basis[3][0] << " " << basis[3][1] << " " << basis[3][2] << " " << basis[3][3] << std::endl;

	
	//GLfloat u = 0;
	glm::vec4 input = glm::vec4(pow(u, 3), pow(u, 2), u, 1);
	std::cout << "INPUT:" << std::endl;
	std::cout << input[0] << " " << input[1] << " " << input[2] << " " << input[3] << std::endl;
	
	glm::vec4 result = input * basis * control;
	*/
	std::cout << "RESULT:" << std::endl;
	std::cout << "x = " << x << " y = " << y << " z = " << z << std::endl;

	return glm::vec3(x, y, z);
}