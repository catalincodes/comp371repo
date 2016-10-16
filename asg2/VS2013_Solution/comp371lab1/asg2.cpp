#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Shader.h"
#include "DataContainer.h"

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

enum State { INPUT_DATA, RENDER_SPLINE, ANIMATE };

State currentState = INPUT_DATA;
bool needsUpdate = false;

// Is called whenever an error is encountered
void error_callback(int error, const char* description);
// Is called whenever a key is pressed / released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Is called whenever a mouse button is pressed / released via GLFW
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// is called to initialize the graphics
bool initGraphics(GLFWwindow*& window, std::string title, int& viewportWidth, int& viewportHeight);
// generates the Vector Array Object
GLuint generateVAO();
void sendDataToGPU(vector<GLfloat>& inputData, GLuint& VAO);
void renderPoints(GLuint& VAO, int nrPoints);
void displayContent();
void normalizePoints(double &x, double &y);

DataContainer* container = nullptr;


// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window = nullptr;
	
	// initialize graphics
	int width, height;
	if (!initGraphics(window, "Triangle", width, height))
		return -1;
	
	// create shaders
	Shader* shader = new Shader("vertex.shader", "fragment.shader");
	
	container = new DataContainer();

	GLuint VAO;
	VAO = generateVAO();

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
	
	GLuint transformLoc = glGetUniformLocation(shader->getShaderProgram(), "model_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shader->getShaderProgram(), "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shader->getShaderProgram(), "projection_matrix");

	// *********************
	// * G A M E   L O O P *
	// *********************
	while (!glfwWindowShouldClose(window)) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
	
		switch (currentState) {
		case INPUT_DATA:
		{
			// update GPU data
					
			if (needsUpdate == true) {
				sendDataToGPU(container->getData(), VAO);
			}
			
			renderPoints(VAO, container->getData().size()/6 );
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


		/*
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		*/
		glm::mat4 model_matrix;
		//model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), //camera positioned here
					glm::vec3(0.0f, 0.0f, 0.0f), //looks at origin
					glm::vec3(0.0f, 1.0f, 0.0f)); //up vector

		glm::mat4 projection_matrix;
		//projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
		projection_matrix = glm::ortho(0.0f, 800.0f, 800.0f, 0.0f);
		
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	
	delete shader;
	delete container;

	shader = nullptr;
	container = nullptr;

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		displayContent();
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		system("CLS");
}

void error_callback(int error, const char* description)
{
	std::cerr << "Error #" << error << " = " << description;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (container != nullptr && currentState == INPUT_DATA) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			normalizePoints(x, y);
			GLfloat xPos = (GLfloat)x;
			GLfloat yPos = (GLfloat)y;
			container->addData(xPos, yPos);
			needsUpdate = true;
			cout << "adding data " << container->getData().size() << endl;
		}
	}
	
}


GLuint generateVAO()
{
	GLuint VAO;
	cout << "generating VAO" << endl;
	// Generate Vertex Array Object
	glGenVertexArrays(1, &VAO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	return VAO;
}


void sendDataToGPU(vector<GLfloat>& inputData, GLuint& VAO)
{
	// fail-safe in case the VAO has not been initalized
	cout << "sending new data to GPU." << endl;
	if (VAO == 0) {
		generateVAO();
	}

	GLuint VBO;

	// Generate Vertex Buffer Object
	glGenBuffers(1, &VBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, inputData.size() * sizeof(GLfloat), inputData.data(), GL_STATIC_DRAW);

	// inform the card of the structure of the information sent
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	needsUpdate = false;
}

void renderPoints(GLuint& VAO, int nrPoints)
{
	//cout << "Rendering  " << nrPoints <<" points " << endl;
	glPointSize(45);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 6 );
	glBindVertexArray(0);
}

bool initGraphics(GLFWwindow*& window, std::string title, int& viewportWidth, int& viewportHeight)
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
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//glfwWindowHint(GLFW_DEPTH_BITS, 24);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	//V_SYNC - enabled
	glfwSwapInterval(1);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Define the viewport dimensions
	glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
	glViewport(0, 0, viewportWidth, viewportHeight);
	
	return true;
}
//debug function used to display content of vector
void displayContent()
{
	for (unsigned int i = 0; i < container->getData().size(); i += 6) {
		cout << container->getData().at(i) << "\t"
			<< container->getData().at(i + 1) << "\t"
			<< container->getData().at(i + 2) << "\t"
			<< container->getData().at(i + 3) << "\t"
			<< container->getData().at(i + 4) << "\t"
			<< container->getData().at(i + 5) << endl;
	}
	cout << "\n\n";
}

void normalizePoints(double &x , double &y)
{
	x = x * 2 / 800 - 1;
	y = y * 2 / 800 - 1;
}	