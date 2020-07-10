#pragma once
#ifndef PROBLEMS_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define PROBLEMS_H

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

// #include <unistd.h>   // added for the timer part (April 22)

using namespace std;

// function declarations here
int problem1();
void problem2();
void problem3();
void problem4();
void problem5();
void problem6();

#endif