#ifndef MAIN_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define MAIN_H

#include <iostream>

// GLEW ------ Header
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW ------ Header
// Please refer to more examples at: https://www.glfw.org/docs/latest/quick_guide.html
// 
#include <GLFW/glfw3.h>
//
// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFE!
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

// #include <unistd.h>   // added for the timer part (April 22)

using namespace std;

// function declarations here
int hw1_problem1();
void hw1_problem2();
void hw1_problem3();
void hw1_problem4();
void hw1_problem5();
void hw1_problem6();

void hw2_problem123();
void hw2_problem4();

void hw3_problem123();

// user defined functions in opengl.cpp
float normalize(float value, float minValue, float maxValue, float a, float b);
int glinit(GLFWwindow*& window, int width, int height, const char* title); // creating windows and load glfw, glew
void setupVAOVBO(GLuint VAO, GLuint VBO, vector<float> data); //setup of VAO, VBO only position
void setupTextureVAOVBO(GLuint VAO, GLuint VBO, vector<float> data); // setup of VAO, VBO for position and texture

#endif