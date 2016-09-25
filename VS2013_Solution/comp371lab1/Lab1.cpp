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
#include "..\CImg\CImg.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

//using namespace std;

typedef unsigned char uchar; // uchar = unsigned char (0..255)

// Image dimensions
const int IMG_HEIGHT = 360;
const int IMG_WIDTH = 360;


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 0.2f;

glm::vec3 triangle_scale = glm::vec3 (1.0f);
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);

UINT* genArray(cimg_library::CImg<USHORT>& baseImage, cimg_library::CImg<USHORT>& hMapImage)
{
	//Array Size. Format = XYZRGB, hence why we multiply the nr of pixels by 6
	const UINT IMG_ARRAY_SIZE = IMG_WIDTH * IMG_WIDTH * 6;
	
	//Aliases for the 3 channels: red, green and blue
	const int RED = 0;
	const int GREEN = 1;
	const int BLUE = 2;

	//Allocate new array
	UINT* img_array = new UINT[IMG_ARRAY_SIZE];
	
	//Process image and build array data
	/*
	Format:
	
	-------------------------------------------------------------
	| X POSITION | Y POSITION | Z POSITION | RED | GREEN | BLUE |
	|   FROM     |    FROM    |    FROM    |    FROM            |
	| BASE_IMAGE | BASE_IMAGE | HMAP_IMAGE |  BASE_IMAGE        |
	-------------------------------------------------------------
	*/
	UINT counter = 0;
	for (UINT row = 80; row < 81; ++row) {
		for (UINT col = 0; col < baseImage._width; ++col) {
			
			img_array[counter++] = col;										//X
			img_array[counter++] = row;										//Y
			img_array[counter++] = *(hMapImage.data(col, row, 0, RED));		//Z

			img_array[counter++] = *(hMapImage.data(col, row, 0, RED));		//R
			img_array[counter++] = *(hMapImage.data(col, row, 0, GREEN));	//G
			img_array[counter++] = *(hMapImage.data(col, row, 0, BLUE));	//B

		}
	}

	return img_array;

}

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

// Creates a STATIC CImg data container from a given BMP file and return the container
cimg_library::CImg<USHORT> loadImage(std::string filename)
{
	cimg_library::cimg::exception_mode(0); //Enabling quiet exception mode
	cimg_library::CImg<USHORT> img;
	try {
		img = cimg_library::CImg<USHORT>(filename.c_str()); //"org_image.bmp"
	}
	catch (cimg_library::CImgException &e) {
		std::cout << "Error = " << e.what() << std::endl;
	}
	return img;
}

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

}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();

	// Register error callback function
	glfwSetErrorCallback(error_callback);

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_DEPTH_BITS, 24);

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
	//glEnable(GL_DEPTH_TEST);

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

	// ****************************************
	// * Build and compile our shader program *
	// ****************************************

	// ===== Read Shaders from file =====

	// 1) Read the Vertex Shader code from the file
	std::string vertex_shader_path = "vertex.shader";
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (std::getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// 2) Read the Fragment Shader code from the file
	std::string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// 3) Create Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);

	// 3.1) Check for compile time errors for Vertex Shader
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// 4) Create Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);

	// 4.1) Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// 5) Link shaders. Created shaderProgram
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// 5.1) Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// 6) Free up memory
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);


	// 7) Use created shaderProgram
	glUseProgram(shaderProgram);

	cimg_library::CImg<USHORT> img = loadImage("org_image.bmp");
	cimg_library::CImg<USHORT> hmap = loadImage("height_map.bmp");
	
	UINT* img_array = genArray(img, hmap);


	// ****************************************
	// * Load Vertices for Triangle			  *
	// ****************************************

	// 1) Create Array of Vertices
	GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f,  // Top
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
	};

	GLuint VAO, VBO;

	// Generate Vertex Array Object
	glGenVertexArrays(1, &VAO);

	// Generate Vertex Buffer Object
	glGenBuffers(1, &VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

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
								const GLvoid * pointer);	first component of the first					(GLvoid*)0
															generic vertex attribute in the array
	*/
	
	glEnableVertexAttribArray(0);

	// unbind the buffer and the array
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), //camera positioned here
			glm::vec3(0.0f, 0.0f, 0.0f), //looks at origin
			glm::vec3(0.0f, 1.0f, 0.0f)); //up vector

		glm::mat4 projection_matrix;
		projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);


		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}