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
#include <vector>

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

GLfloat* genArray(cimg_library::CImg<USHORT>& baseImage, cimg_library::CImg<USHORT>& hMapImage, UINT &size)
{
	//Array Size. Format = XYZRGB, hence why we multiply the nr of pixels by 6
	const UINT IMG_ARRAY_SIZE = IMG_WIDTH * IMG_WIDTH * 6;
	
	//Aliases for the 3 channels: red, green and blue
	const int RED = 0;
	const int GREEN = 1;
	const int BLUE = 2;

	//Allocate new array
	GLfloat* img_array = new GLfloat[IMG_ARRAY_SIZE];
	size = IMG_ARRAY_SIZE;
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
	for (UINT row = 0; row < baseImage._height; ++row) {
		for (UINT col = 0; col < baseImage._width; ++col) {
			
			img_array[counter++] = (GLfloat)col;										//X
			img_array[counter++] = (GLfloat)row;										//Y
			img_array[counter++] = (GLfloat)*(hMapImage.data(col, row, 0, RED));		//Z

			img_array[counter++] = (GLfloat)*(baseImage.data(col, row, 0, RED));		//R
			img_array[counter++] = (GLfloat)*(baseImage.data(col, row, 0, GREEN));		//G
			img_array[counter++] = (GLfloat)*(baseImage.data(col, row, 0, BLUE));		//B

		}
	}

	return img_array;

}

void normalizeImageArray(GLfloat* img_array, UINT array_size, GLfloat newMin, GLfloat newMax, GLfloat curMin, GLfloat curMax)
{
	UINT counter = 0;
	GLfloat scale = (curMax - curMin) / (newMax - newMin);
	GLfloat offset = newMin - curMin;

	do {
		
		img_array[counter++] = (img_array[counter] / scale) + offset;
		img_array[counter++] = (img_array[counter] / scale) + offset;
		img_array[counter++] = (img_array[counter] / scale) + offset;
		counter += 3; // skip the color attributes, until the next vector
	} while (counter < array_size);
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

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

}

// reads ShaderCode from a file (i.e. vertex.shader, fragment.shader) and returns the code as a string
std::string readShaderCodeFromFile(std::string shader_path)
{
	std::string ShaderCode;

	std::ifstream ShaderStream(shader_path, std::ios::in);

	if (ShaderStream.is_open()) {
		std::string Line = "";
		while (std::getline(ShaderStream, Line))
			ShaderCode += "\n" + Line;
		ShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", shader_path.c_str());
		getchar();
		exit(-1);
	}

	return ShaderCode;
}

// creates and returns a pointer to a shader created from shader code (received as parameter). The shaderType must be mentioned as GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
GLuint createShader(std::string shaderCode, GLenum shaderType)
{
	const int VERTEX_TYPE = 1;
	const int FRAG_TYPE = 2;
	GLuint shader;

	shader = glCreateShader(shaderType);
	
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shader, 1, &sourcePointer, NULL);
	glCompileShader(shader);

	//Check for compile time errors
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		if (shaderType == GL_VERTEX_SHADER) {
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else if (shaderType == GL_FRAGMENT_SHADER) {
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}	
	}

	return shader;
}

// creates, links and returns a shader program, from a vertex shader and a fragment shader
GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	return shaderProgram;

}

// creates a comp
GLuint createShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path)
{
	// ****************************************
	// * Build and compile our shader program *
	// ****************************************

	// Read Shaders from File
	std::string VertexShaderCode = readShaderCodeFromFile(vertex_shader_path);
	std::string FragmentShaderCode = readShaderCodeFromFile(fragment_shader_path);

	// Create/Compile Shaders 
	GLuint vertexShader = createShader(VertexShaderCode, GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(FragmentShaderCode, GL_FRAGMENT_SHADER);

	// Link shaders. Created shaderProgram
	GLuint shaderProgram = createShaderProgram(vertexShader, fragmentShader);
	
	// Free up memory
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

std::vector<GLuint> genIndicesVector(UINT numCols, UINT numRows)
{
	std::vector<GLuint> indVector;
	for (int j = 0; j < (unsigned int)numRows - 1;++j) {
		for (int i = 0; i < (unsigned int)numCols;++i)
		{
			
			//TOP TRIANGLE
			indVector.push_back(i);
			indVector.push_back(i + numCols);
			indVector.push_back(i + 1);
			
			//BOTTOM TRIANGLE
			indVector.push_back(i + 1);
			indVector.push_back(i + numCols);
			indVector.push_back(i + numCols + 1);
		}
	}
	
	return indVector;
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
	GLuint shaderProgram = createShaderProgram("vertex.shader","fragment.shader");
	glUseProgram(shaderProgram);
	
	cimg_library::CImg<USHORT> img = loadImage("org_image.bmp");
	cimg_library::CImg<USHORT> hmap = loadImage("height_map.bmp");
	
	UINT imgArraySize;
	GLfloat* img_array = genArray(img, hmap, imgArraySize);

	normalizeImageArray(img_array, imgArraySize, -1.0f, 1.0f, 0.0f, 360.0f);
		
	std::vector<GLuint> indicesVector = genIndicesVector(360, 360);

	std::cout << "Done";

	// ****************************************
	// * Load Vertices for Triangle			  *
	// ****************************************

	// 1) Create Array of Vertices
	/*
	GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f,  // Top
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
	};
	*/
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(img_array), img_array, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
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
								const GLvoid * pointer);	first component of the first					(GLvoid*)0
															generic vertex attribute in the array
	*/
	
	// Bind the Element Buffer Object and use it to send the indices vector
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesVector.size() * sizeof(GLuint), &indicesVector.front(), GL_STATIC_DRAW);

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
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, indicesVector.size(), GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 200.0f)+camera_translation, //camera positioned here
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
	delete[] img_array;
	glfwTerminate();
	return 0;
}