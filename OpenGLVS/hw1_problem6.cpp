#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// Window dimensions to be used in the call-back window size
const GLuint WIDTH = 800, HEIGHT = 600;


// use gaussian elimination to work with any size of matrix but we only do 2 by 2 matrix only
//vector<float> gaussj(int n, vector<vector<int>> A, vector<int> b);

// for debugging purpose
template <typename T>
void printMatrix(vector<vector<T>> vec2d) {
	cout << "Printing matrix ..." << endl;
	for (int i = 0; i < vec2d.size(); i++) {
		for (int j = 0; j < vec2d[i].size(); j++) {
			cout << vec2d[i][j] << "\t";
		}
		cout << endl;
	}
}


vector<float> getPayoff2by2(vector<vector<pair<int, int>> > vec2d, vector<vector<float>> MSNEsoln) {
	
	int numRows = vec2d.size();
	int numCols = vec2d[0].size();
	vector<float> payoffVec;
	
	vector<vector<float>> confMatrix = 
	{
		{MSNEsoln[0][0] * MSNEsoln[1][0], MSNEsoln[0][0] * MSNEsoln[1][1]},
		{MSNEsoln[0][1] * MSNEsoln[1][0], MSNEsoln[0][1] * MSNEsoln[1][1]}
	};

	// payoff for player 1
	float current_sum = 0;
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++)
			current_sum = current_sum + vec2d[i][j].first * confMatrix[i][j];
	}
	payoffVec.push_back(current_sum);

	// payoff for player 2
	current_sum = 0; // reset this value
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++)
			current_sum = current_sum + vec2d[i][j].second * confMatrix[i][j];
	}
	payoffVec.push_back(current_sum);

	return payoffVec;
}

vector<vector<float>> MSNEsolver2by2(vector<vector<pair<int, int>> > vec2d){
	
	//for player 1
	vector<vector<int>> A = {
	{ vec2d[0][0].second - vec2d[0][1].second, vec2d[1][0].second - vec2d[1][1].second },
	{ 1										 , 1} };
	vector<int> b = { 0, 1 };
	float p = (float) (b[0] * A[1][1] - A[0][1] * b[1]) / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
	vector<float> MSNE1 = { p, 1 - p };

	// for player 2
	A.clear();
	b.clear();
	A = {
	{ vec2d[0][0].first - vec2d[1][0].first, vec2d[0][1].first - vec2d[1][1].first },
	{ 1										 , 1} };
	b = { 0, 1 };
	float q = (float)(b[0] * A[1][1] - A[0][1] * b[1]) / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
	vector<float> MSNE2 = { q, 1 - q };

	return { MSNE1, MSNE2 };
}


int visualization(vector<vector<string>> vec2dStr, vector<vector<string>>  MSNEsolnStr) {

	cout << "Visualizing this problem ... " << endl;

	// Init GLFW -- initilizaing GLFW library
	glfwInit();

	// version compatibility with opengl 3.3, core profile, mac computer
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "HW1 Problem 6 Visualization", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	// Making the OpenGL context current
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	// Alternative is to use GLAD
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		cout << "Failed to initialize GLEW" << endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);


	// -----------------------------------------------------------
	// now drawing start here 
	// -----------------------------------------------------------

	// text rendering setup
	TextRender textRenderProgram(WIDTH, HEIGHT);

	Shader shaderProgram("vShader.glsl", "fShader.glsl");
	GLfloat lineData[] =
	{
		-0.7f, 0.4f, 0.0f, // Line 1 left
		1.0f, 0.4f, 0.0f, //  Line 1 right

		-0.7f, -0.3f, 0.0f,   // Line 2 left
		1.0f, -0.3f, 0.0f, // Line 2 right

		-0.4f, 0.7f, 0.0f,   // Line 3 up
		-0.4f, -1.0f, 0.0f, // Line 3 bottom

		0.3f, 0.7f, 0.0f,   // Line 4 up
		0.3f, -1.0f, 0.0f, // Line 4 bottom
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO); // 3 setups we have
	glGenBuffers(1, &VBO);  // 3

	// Line
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineData), lineData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw Line
		shaderProgram.use(); 
		shaderProgram.setColor("userDefinedColor", 0.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 8); // 4 vetexes

		//Draw Text
		textRenderProgram.RenderText("Player 1", 2.0f, 300.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderProgram.RenderText("Player 2", 500.0f, 550.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));

		// player 1 probabilities
		textRenderProgram.RenderText(MSNEsolnStr[0][0], 150.0f, 300.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderProgram.RenderText(MSNEsolnStr[0][1], 150.0f, 100.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));

		// data
		textRenderProgram.RenderText(vec2dStr[0][0], 350.0f, 300.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderProgram.RenderText(vec2dStr[1][0], 350.0f, 100.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));

		textRenderProgram.RenderText(vec2dStr[0][1], 650.0f, 300.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderProgram.RenderText(vec2dStr[1][1], 650.0f, 100.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));

		// Player 2 probabilities
		textRenderProgram.RenderText(MSNEsolnStr[1][0], 350.0f, 500.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderProgram.RenderText(MSNEsolnStr[1][1], 650.0f, 500.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));


		// unbind VAO for the 
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return EXIT_SUCCESS;
}


void dataToString(vector<vector<pair<int, int>> > vec2d, vector<vector<float>> MSNEsoln,
	vector<vector<string>>& vec2dStr, vector<vector<string>>& MSNEsolnStr)
{
	string num1;
	string num2;
	string label;

	for (int i = 0; i < vec2d.size(); i++) {
		for (int j = 0; j < vec2d[0].size(); j++) {
			num1 = to_string(vec2d[i][j].first);
			num2 = to_string(vec2d[i][j].second);
			label = num1 + ", " + num2;
			vec2dStr[i].push_back(label);
		}
	}

	for (int i = 0; i < vec2d.size(); i++) {
		for (int j = 0; j < vec2d[0].size(); j++) {
			label = to_string(MSNEsoln[i][j]);
			MSNEsolnStr[i].push_back(label);
		}
	}
}


void hw1_problem6() {
	vector<vector<pair<int, int>> > vec2d = 
	{ { {2, -3},	{1, 2} },
	  { {1,  1},	{4, -1} } };

	vector<vector<float>> MSNEsoln = MSNEsolver2by2(vec2d);
	cout << "Player 1 mixed strategies with probabilities (p, 1-p) = "
		<< "(" << MSNEsoln[0][0] << ", " << MSNEsoln[0][1] << ")" << endl;
	cout << "Player 2 mixed strategies with probabilities (q, 1-q) = "
		<< "(" << MSNEsoln[1][0] << ", " << MSNEsoln[1][1] << ")" << endl;

	vector<float> payoffVec = getPayoff2by2(vec2d, MSNEsoln);
	cout << "Payoff for player 1: " << payoffVec[0] << endl;
	cout << "Payoff for player 2: " << payoffVec[1] << endl;

	vector<vector<string>> vec2dStr(2);
	vector<vector<string>> MSNEsolnStr(2);

	// convert the data and solution to string data and string solution
	dataToString(vec2d, MSNEsoln, vec2dStr, MSNEsolnStr);

	// pass them to the visualization function
	visualization(vec2dStr, MSNEsolnStr);
}