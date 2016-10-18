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

glm::vec3 catmullRomFunc(GLfloat u, const glm::mat4& controlMatrix);
void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, const glm::mat4& currentControlMatrix);
struct Line
{
	glm::vec3 start;
	glm::vec3 end;

	Line() {}
	Line(glm::vec3 s, glm::vec3 e):
		start(s), end(e) { }

};

vector<Line> listLines;

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
			vector<GLfloat> v = container->getData();
			if (container->getData().size() >= 4 && booya == false) {
				
				int i = 0;
				glm::mat4 ccm = glm::mat4(
					glm::vec4(v[i],        v[i+1],       v[i+2],           0),
					glm::vec4(v[i+6],      v[i+7],       v[i+8],           0),
					glm::vec4(v[i+12],     v[i+13]      ,v[i+14],          0),
					glm::vec4(v[i+18],     v[i+19]      ,v[i+20],          0)
					);

				subDivisionAlgo(0.0f, 1.0f, 2.0f, ccm);

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

void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, const glm::mat4& currentControlMatrix)
{
	std::cout << "subDivAlgo method called for " << u0 << " and "<< u1 << "." << std::endl;
	GLfloat uMid = (GLfloat)((u0 + u1) / 2.0f);
	glm::vec3 x0 = catmullRomFunc(u0, currentControlMatrix);
	glm::vec3 x1 = catmullRomFunc(u1, currentControlMatrix);

	GLfloat distance = glm::distance(x0, x1);
	std::cout << "Back in subDivAlgo. Distance = " << distance << std::endl;
	std::cout << "Comparing to the maxLineLength " << maxLineLength << "...";


	if (abs(distance) > maxLineLength) {
		std::cout << "I'm going to subdivide again with the midpoint " << uMid << "." << std::endl;
		
		std::cout << "Calling subdivision from " << u0 << " and " << uMid << ":" << std::endl;
		subDivisionAlgo(u0, uMid, maxLineLength, currentControlMatrix);
		std::cout << "Calling subdivision from " << uMid << " and " << u1 << ":" << std::endl;
		subDivisionAlgo(uMid, u1, maxLineLength, currentControlMatrix);
	}
	else {
		std::cout << "And we are good... I have the following two points to place in a container: " << std::endl;
		std::cout << "x0 (" << x0.x << ", " << x0.y << ", " << x0.z << ") + ";
		std::cout << "x1 (" << x1.x << ", " << x1.y << ", " << x1.z << ")" << std::endl;
		listLines.push_back(Line(x0, x1));
	}

}



glm::vec3 catmullRomFunc(GLfloat u, const glm::mat4& controlMatrix)
{
	std::cout << "CatMull calculation for : " << u << "." << std::endl;

	/* 
		[0][0]	[0][1]	[0][2]	[0][3]
		[1][0]	[1][1]	[1][2]	[1][3]
		[2][0]	[2][1]	[2][2]	[2][3]
		[3][0]	[3][1]	[3][2]	[3][3]
	*/
	
	GLfloat u_2 = pow(u, 2);
	GLfloat u_3 = pow(u, 3);

	GLfloat s = 0.5; //tension parameter
	glm::mat4 basis = glm::mat4(
		glm::vec4(-s    , 2 - s , s - 2     , s  ),
		glm::vec4(2 * s , s - 3 , 3 - 2 * s , -s ), 
		glm::vec4(-s    , 0     , s         , 0  ),
		glm::vec4(0     , 1     , 0         , 0  )
		);

	// result of multiplying the input [u^3, u^2, u^1, 1] by the basis matrix 
	GLfloat b1 = u_3 * basis[0][0]   +
				 u_2 * basis[1][0] +
				 u   * basis[2][0]   +
				 1   * basis[3][0];

	GLfloat b2 = u_3 * basis[0][1] +
				 u_2 * basis[1][1] +
				 u   * basis[2][1] +
				 1   * basis[3][1];

	GLfloat b3 = u_3 * basis[0][2] +
				 u_2 * basis[1][2] +
				 u   * basis[2][2] +
				 1   * basis[3][2];

	GLfloat b4 = u_3 * basis[0][3] +
				 u_2 * basis[1][3] +
				 u   * basis[2][3] +
				 1   * basis[3][3];


	// multiply the vector from the previous calculation [b1, b2, b3, b4] by the control matrix
	GLfloat x = b1 * controlMatrix[0][0] + 
				b2 * controlMatrix[1][0] +
				b3 * controlMatrix[2][0] + 
				b4 * controlMatrix[3][0];
	
	GLfloat y = b1 * controlMatrix[0][1] +
				b2 * controlMatrix[1][1] +
				b3 * controlMatrix[2][1] +
				b4 * controlMatrix[3][1];

	GLfloat z = b1 * controlMatrix[0][2] +
				b2 * controlMatrix[1][2] +
				b3 * controlMatrix[2][2] +
				b4 * controlMatrix[3][2];


	std::cout << "RESULT:" << std::endl;
	std::cout << "x = " << x << " y = " << y << " z = " << z << std::endl;

	return glm::vec3(x, y, z);
}