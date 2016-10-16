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
#include "Shader.h"

//using namespace std;

typedef unsigned char uchar; // uchar = unsigned char (0..255)

// Image dimensions
const int IMG_HEIGHT = 360;
const int IMG_WIDTH = 360;

DataContainer* container = nullptr;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 100.00f;

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


// Is called whenever an error is encountered
void error_callback(int error, const char* description)
{
	std::cerr << "Error #" << error << " = " << description;
}

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

std::vector<GLuint> genIndicesVector(GLuint numCols, GLuint numRows)
{
	std::vector<GLuint> indVector;
	for (int j = 0;	j < (numRows - 1); ++j) 
	{
		for (int i = 0; i < numCols; ++i)
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
	std::cout << "Starting GLFW context, OpenGL 3.1" << std::endl;
	// Init GLFW
	glfwInit();

	// Register error callback function
	glfwSetErrorCallback(error_callback);

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//V_SYNC - enabled
	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Create Shader Program from a vertex shader file and a fragment file

	Shader* shaderObject = new Shader("vertex.shader", "fragment.shader");
	GLuint shaderProgram = shaderObject->getShaderProgram();
	glUseProgram(shaderProgram);
	
	
	//normalizeImageArray(img_array, imgArraySize, -1.0f, 1.0f, 0.0f, 360.0f);
		
	std::vector<GLuint> indicesVector = genIndicesVector(360, 360);
	//testIndVector(indicesVector);

	// ****************************************
	// * Load Vertices for Triangle			  *
	// ****************************************

	// 1) Create Array of Vertices
	
	container = new DataContainer();
	container->addData(100.0f, 100.0f);

	GLuint VAO, VBO, EBO;

	// Generate Vertex Array Object
	glGenVertexArrays(1, &VAO);

	// Generate Vertex Buffer Object
	glGenBuffers(1, &VBO);

	// Generate Element Buffer Object
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, container->getData().size() * sizeof(GLfloat), container->getData().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	/*
	void glVertexAttribPointer(	GLuint index,				generic vertex attrib to be modified			0

								GLint size,					nr of components per generic vertex attrib		3

								GLenum type,				data type of each component in the array		GL_FLOAT

								GLboolean normalized,		specifies whether fixed-point data values
															should be normalized (GL_TRUE) or converted
															directly as fixed-point values (GL_FALSE)
															when they are accessed.							GL_FALSE

								GLsizei stride,				Specifies the byte offset between				3 * sizeof(GLfloat)
															consecutive generic vertex attributes.
															const 

								GLvoid * pointer);			first component of the first							(GLvoid*)0
															generic vertex attribute in the array
								*/
	

	// Bind the Element Buffer Object and use it to send the indices vector
	
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesVector.size() * sizeof(GLuint), indicesVector.data(), GL_STATIC_DRAW);*/

	// unbind the buffer and the array

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		//glClear(GL_COLOR_BUFFER_BIT);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, container->getData().size() / 6);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, indicesVector.size() * 3, GL_UNSIGNED_INT, (void*)0);
		
		glBindVertexArray(0);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 800.0f)+camera_translation, //camera positioned here
			glm::vec3(180.0f, 180.0f, 0.0f), //looks at origin
			glm::vec3(0.0f, 1.0f, 0.0f)); //up vector

		glPointSize(20);
		glm::mat4 projection_matrix;
		projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);


		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	
	// Terminate GLFW, clearing any resources allocated by GLFW.

	delete container;
	delete shaderObject;
	glfwTerminate();
	return 0;
}