#pragma once

#include "problems.h"

// Learned from this tutorial: https://learnopengl.com/Getting-started/Shaders

class Shader
{
public:
	GLuint shaderProgram;

	// constructor
	Shader(const char* vertexFilePath, const char* fragmentFilePath) {

		const char* vertexShaderSource;
		const char* fragmentShaderSource;
		string vertexShaderCode, fragmentShaderCode; // declare these for grabbing source code
		ifstream vertexShaderFile, fragmentShaderFile; // declare these for file opening
		stringstream vertexShaderStream, fragmentShaderStream; //declare these for file streaming

		// ensure ifstream objects can throw exceptions:
		vertexShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fragmentShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		// step 1: get the source code
		try {
			//open shader files
			vertexShaderFile.open(vertexFilePath);
			fragmentShaderFile.open(fragmentFilePath);

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			// close the files
			vertexShaderFile.close();
			fragmentShaderFile.close();

			//grab the shader code in string
			vertexShaderCode = vertexShaderStream.str();
			fragmentShaderCode = fragmentShaderStream.str();
		}
		catch (ifstream::failure& e) {
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}

		// now you got the cource codes
		vertexShaderSource = vertexShaderCode.c_str();
		fragmentShaderSource = fragmentShaderCode.c_str();

		// step 2: Compiling shader programs
		// Build and compile our shader program

		// Vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		// Check for compile time errors
		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		}


		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
		}

		// Link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		// Check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		}

		// delete the shader programs
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	//function that activates the shader
	void use()
	{
		glUseProgram(shaderProgram);
	}

	// set global color
	void setColor(const string& name, float RValue, float GValue, float BValue, float Avalue) const
	{
		int vertexColorLocation = glGetUniformLocation(shaderProgram, name.c_str());
		glUniform4f(vertexColorLocation, RValue, GValue, BValue, Avalue);
	}
};