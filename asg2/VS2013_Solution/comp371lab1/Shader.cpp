#include "Shader.h"
#include <fstream>
#include <iostream>

Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path)
{
	shaderProgram = createShaderProgram("vertex.shader", "fragment.shader");
	glUseProgram(shaderProgram);
}


Shader::~Shader()
{

}

// reads ShaderCode from a file (i.e. vertex.shader, fragment.shader) and returns the code as a string
std::string Shader::readShaderCodeFromFile(std::string shader_path)
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
GLuint Shader::createShader(std::string shaderCode, GLenum shaderType)
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
GLuint Shader::createShaderProgram(GLuint vertexShader, GLuint fragmentShader)
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
GLuint Shader::createShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path)
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

	return shaderProgram;
}
