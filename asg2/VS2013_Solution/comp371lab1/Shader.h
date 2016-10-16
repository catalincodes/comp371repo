#pragma once
#include <string>
#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library

class Shader
{
private:
	std::string readShaderCodeFromFile(std::string shader_path);
	GLuint createShader(std::string shaderCode, GLenum shaderType);
	GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);
	GLuint createShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path);

	GLuint shaderProgram;
public:
	inline GLuint getShaderProgram() { return shaderProgram; };
	Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path);
	~Shader();
};

