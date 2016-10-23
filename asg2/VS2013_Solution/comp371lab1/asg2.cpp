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

// NON-CALLBACK FUNCTIONS:

// generates the spline 
void generateSpline();
// converts the list of lines into a vector of points
void generateSplinePointsVector();
// returns a vec3 point given a parameter (u) and a control matrix
glm::vec3 catmullRomFunc(GLfloat u, const glm::mat4& controlMatrix);
// general subDivisionAlgorithm implementation
void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, const glm::mat4& currentControlMatrix);
// resets the program
void resetApp();
// renders the spline
void renderSpline(CatGLEngine& engine);
// returns the intial position
glm::vec3 getInitialPosition();

// GLOBAL VARIABLES

struct Line
{
	glm::vec3 start;
	glm::vec3 end;

	Line() {}
	Line(glm::vec3 s, glm::vec3 e):
		start(s), end(e) { }

};

GLfloat triangleA[] =
//     VERTICES					COLOR (red)
{
	0.0f, 0.0f,  0.0f,		1.0f, 0.0f, 0.0f,
	0.0f, 20.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	40.0f, 10.0f, 0.0f,		1.0f, 0.0f, 0.0f,
};


vector<Line> listLines;

DataContainer* userPoints = nullptr;
DataContainer* splinePoints = nullptr;
vector<GLfloat>* triangle = nullptr;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

bool booya = false;
bool isSplineGenerated = false;
bool needsUpdate = false;

enum State { INPUT_DATA, RENDER_SPLINE, ANIMATE };
State currentState = INPUT_DATA;
enum RenderMode {LINE_RENDERING, POINT_RENDERING};
RenderMode renderMode = LINE_RENDERING;
glm::mat4 projection_matrix;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && currentState == INPUT_DATA) 
	{
		//std::cout << "Moving to RENDER_SPLINE" << std::endl;
		currentState = RENDER_SPLINE;
	}
	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && currentState == RENDER_SPLINE) {
		std::cout << "Commencing animation ... " << std::endl;
		currentState = ANIMATE;
	}

	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		resetApp(); // state change embedded in the method
	}
		
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		renderMode = POINT_RENDERING;
		std::cout << "Point rendering" << std::endl;
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		renderMode = LINE_RENDERING ;
		std::cout << "Line rendering" << std::endl;
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (userPoints != nullptr && currentState == INPUT_DATA) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			std::cout << "Point added ( " << x << ", " << y << " )" << std::endl;
			GLfloat xPos = (GLfloat)x;
			GLfloat yPos = (GLfloat)y;
			userPoints->addData(xPos, yPos);
			needsUpdate = true;
			//std::cout << "adding data " << userPoints->getData().size() << endl;
		}
	}

}

void windows_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	// Update the Projection matrix after a window resize event
	projection_matrix = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f, -1.0f, 1.0f);
}



// The MAIN function, from here we start the application and run the game loop
int main()
{
	CatGLEngine engine;
	if (engine.initGL(WIDTH, HEIGHT, key_callback, mouse_button_callback, windows_resize_callback) == -1)
		return -1;
	userPoints = new DataContainer();
	
	triangle = new vector<GLfloat>();
	for (int i = 0; i < 3*6;++i) {
		triangle->push_back(triangleA[i]);
	}

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
		
		
	
		switch (currentState) {
		case INPUT_DATA:
		{
			// update GPU data
			glPointSize(5);
			if (needsUpdate == true) {
				engine.sendData(userPoints->getData(), GL_DYNAMIC_DRAW);
			}
			engine.renderVBO(GL_POINTS, 0, userPoints->getData().size() / 6);
			break;
		}

		case RENDER_SPLINE:
		{
			
			// generate the spline
			if (isSplineGenerated == false) {
				generateSpline();
			}
			// if we did not generate ths spline reset the program to INPUT_DATA
			if (isSplineGenerated == false) /*still*/ {
				resetApp();
				break;
			}
			// else, send the data to the GPU
			else {
				engine.sendData(splinePoints->getData(), GL_STATIC_DRAW);
			}

			renderSpline(engine);
			
			break;
		}

		case ANIMATE:
		{
			renderSpline(engine);
			engine.sendTriangleData(*triangle, GL_STATIC_DRAW);
			engine.renderTriangle(GL_TRIANGLES, 0, 3);
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

	if (userPoints != nullptr) 
		delete userPoints;
	if (splinePoints != nullptr)
		delete splinePoints;

	glfwTerminate();
	return 0;
}

// general subDivisionAlgorithm implementation
void subDivisionAlgo(GLfloat u0, GLfloat u1, GLfloat maxLineLength, const glm::mat4& currentControlMatrix)
{
//	std::cout << "subDivAlgo method called for " << u0 << " and "<< u1 << "." << std::endl;
	GLfloat uMid = (GLfloat)((u0 + u1) / 2.0f);
	glm::vec3 x0 = catmullRomFunc(u0, currentControlMatrix);
	glm::vec3 x1 = catmullRomFunc(u1, currentControlMatrix);

	GLfloat distance = glm::distance(x0, x1);
//	std::cout << "Back in subDivAlgo. Distance = " << distance << std::endl;
//	std::cout << "Comparing to the maxLineLength " << maxLineLength << "...";


	if (abs(distance) > maxLineLength) {
//		std::cout << "I'm going to subdivide again with the midpoint " << uMid << "." << std::endl;
		
//		std::cout << "Calling subdivision from " << u0 << " and " << uMid << ":" << std::endl;
		subDivisionAlgo(u0, uMid, maxLineLength, currentControlMatrix);
//		std::cout << "Calling subdivision from " << uMid << " and " << u1 << ":" << std::endl;
		subDivisionAlgo(uMid, u1, maxLineLength, currentControlMatrix);
	}
	else {
//		std::cout << "And we are good... I have the following two points to place in a container: " << std::endl;
//		std::cout << "x0 (" << x0.x << ", " << x0.y << ", " << x0.z << ") + ";
//		std::cout << "x1 (" << x1.x << ", " << x1.y << ", " << x1.z << ")" << std::endl;
		listLines.push_back(Line(x0, x1));
	}

}


// returns a vec3 point given a parameter (u) and a control matrix
glm::vec3 catmullRomFunc(GLfloat u, const glm::mat4& controlMatrix)
{
	//std::cout << "CatMull calculation for : " << u << "." << std::endl;

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


	//std::cout << "RESULT:" << std::endl;
	//std::cout << "x = " << x << " y = " << y << " z = " << z << std::endl;

	return glm::vec3(x, y, z);
}

void generateSpline()
{
	vector<GLfloat> v = userPoints->getData();
	unsigned int numPoints = v.size() / 6;
	
	std::cout << "Generating spline... ";
	
	if (numPoints >= 4) {

		for (unsigned int i = 0; i < (numPoints-3); ++i) {
			
			glm::mat4 ccm = glm::mat4(
				glm::vec4(v[i*6 + 0],  v[i*6 + 1],  v[i*6 + 2], 0),
				glm::vec4(v[i*6 + 6],  v[i*6 + 7],  v[i&6 + 8], 0),
				glm::vec4(v[i*6 + 12], v[i*6 + 13], v[i*6 + 14], 0),
				glm::vec4(v[i*6 + 18], v[i*6 + 19], v[i*6 + 20], 0)
				);

			subDivisionAlgo(0.0f, 1.0f, 30.0f, ccm);

		}

		generateSplinePointsVector();
		
		isSplineGenerated = true;
	}

	std::cout << "done" << std::endl;
}

void generateSplinePointsVector()
{
	if (listLines.size() == 0)
		return;
	splinePoints = new DataContainer;

	//add the start points of all the lines
	for (Line currentLine : listLines) {
		splinePoints->addData(currentLine.start.x, currentLine.start.y);
	}

	//add the end point of the last line in the list of lines
	Line* lastLine = &(listLines.at(listLines.size() - 1));
	splinePoints->addData(lastLine->end.x, lastLine->end.y);
}

void resetApp()
{
	if (userPoints != nullptr) {
		delete userPoints;
		userPoints = new DataContainer;
	}
	if (splinePoints != nullptr) {
		delete splinePoints;
		splinePoints = nullptr;
		isSplineGenerated = false;
	}
	if (listLines.size() != 0) {
		listLines.clear();
		listLines.resize(0);
	}
	if (currentState != INPUT_DATA) {
		std::cout << "Moving back to INPUT_DATA" << std::endl;
		currentState = INPUT_DATA;
	}
	needsUpdate = true;
}

void renderSpline(CatGLEngine& engine)
{
	// assuming we successfully generated the spline, render it
	if (isSplineGenerated) {
		if (renderMode == LINE_RENDERING) {
			glPointSize(1);
			engine.renderVBO(GL_LINE_STRIP, 0, splinePoints->getData().size() / 6);
			//std::cout << "L" << std::endl;
		}
		else if (renderMode == POINT_RENDERING) {
			glPointSize(3);
			engine.renderVBO(GL_POINTS, 0, splinePoints->getData().size() / 6);
			//std::cout << "P" << std::endl;
		}
	}
}

glm::vec3 getInitialPosition()
{
	return glm::vec3(
		splinePoints->getData().at(0), //x
		splinePoints->getData().at(1), //y
		splinePoints->getData().at(2)  //z
		);
}
