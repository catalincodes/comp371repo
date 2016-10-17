#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <vector>
#include "Shader.h"

/*************************************************
* Title:					Cat GL Engine
* Author:					Catalin Stefan Popescu
* Date Last Modified:		16-Oct-2016
* Version:					0.1.2
**************************************************/

/*
Notes: 
	this is a developmental version. We are only using one single VBO
*/	

#pragma once
class CatGLEngine
{
public:
	//constructor, intializes
	CatGLEngine();
	~CatGLEngine();

	GLint initGL(GLuint const WIDTH, GLuint const HEIGHT, GLFWkeyfun keyCallBackFunc, GLFWmousebuttonfun msBtnCallBackFunc);

	void initVertexObjects();
	void sendData(std::vector<GLfloat> bufferData, GLenum usage);
	void renderVBO(GLenum mode, GLint offset, GLsizei nrVertices);
	
	inline GLuint getVertexArrayObject() const { return VAO; }
	inline GLFWwindow* getWindowPtr() const { return window; }
	inline GLuint getShaderProgram() const { return shader->getShaderProgram(); }

private:
	
	GLFWwindow* window;

	Shader* shader;

	GLuint VAO;
	GLuint VBO;

};
