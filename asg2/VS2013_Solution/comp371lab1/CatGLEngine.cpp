#include "CatGLEngine.h"
#include <iostream>

void error_callback(int error, const char* description);

CatGLEngine::CatGLEngine()
{
	VAO = 0;
	VBO = 0;
	TriangleVBO = 0;
}


CatGLEngine::~CatGLEngine()
{
	delete shader;
}

void CatGLEngine::initVertexObjects()
{
	// Generate VAO
	glGenVertexArrays(1, &VAO);

	// Generate Triangle VAO
	glGenVertexArrays(1, &TriangleVAO);

	// Generate VBO
	glGenBuffers(1, &VBO);

	// Generate Triangle VBO
	glGenBuffers(1, &TriangleVBO);

}

/*************** CHARACTERISTICS OF glVertexAttribPointer METHOD ***************************************
GLuint index,				generic vertex attrib to be modified			0

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
*********************************************************************************************************/



void CatGLEngine::sendData(std::vector<GLfloat> bufferData, GLenum usage)
{
	//Bind VAO and VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Copy bufferData to bound VBO
	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(GLfloat), bufferData.data(), usage);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	//Unbind VAO and VBO (reverse order)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CatGLEngine::sendTriangleData(std::vector<GLfloat> bufferData, GLenum usage)
{
	//Bind VAO and VBO
	glBindVertexArray(TriangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, TriangleVBO);

	//Copy bufferData to bound VBO
	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(GLfloat), bufferData.data(), usage);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	//Unbind VAO and VBO (reverse order)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CatGLEngine::renderVBO(GLenum mode, GLint offset , GLsizei nrVertices)
{
	glBindVertexArray(VAO);
	glDrawArrays(mode, offset , nrVertices);
	glBindVertexArray(0);
}

void CatGLEngine::renderTriangle(GLenum mode, GLint offset, GLsizei nrVertices)
{
	glBindVertexArray(TriangleVAO);
	glDrawArrays(mode, offset, nrVertices);
	glBindVertexArray(0);
}

GLint CatGLEngine::initGL(GLuint const WIDTH , GLuint const HEIGHT, GLFWkeyfun keyCallBackFunc, GLFWmousebuttonfun msBtnCallBackFunc, GLFWframebuffersizefun windowResizeCallbackFunc)
{
	std::cout << "Starting GLFW context, OpenGL 3.1" << std::endl;
	// Init GLFW
	glfwInit();

	// Register error callback function
	glfwSetErrorCallback(error_callback);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//V_SYNC - enabled
	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set the required callback functions
	glfwSetKeyCallback(window, keyCallBackFunc);
	glfwSetMouseButtonCallback(window, msBtnCallBackFunc);
	glfwSetFramebufferSizeCallback(window, windowResizeCallbackFunc);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Generate the Shader
	shader = new Shader("vertex.shader", "fragment.shader");
	GLuint shaderProgram = shader->getShaderProgram();
	glUseProgram(shaderProgram);

	return true;
}

void error_callback(int error, const char* description)
{
	std::cerr << "Error #" << error << " = " << description;
}