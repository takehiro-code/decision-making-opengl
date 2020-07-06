
#include "problems.h"
#include "shader.h"


// Window dimensions to be used in the call-back window size
const GLuint WIDTH = 800, HEIGHT = 600;


// For example 1: using the code provided from the tutorial soring.pdf
// take reference of vec so that original vector is modified without necessary returning it
void quicksort(vector<int> &vec, int left, int right) {
	int i = left, j = right;
	int tmp;
	int pivot = vec[(left + right) / 2];
	/* partition */
	while (i <= j) {
		while (vec[i] < pivot)
			i++;
		while (vec[j] > pivot)
			j--;
		if (i <= j) {
			tmp = vec[i];
			vec[i] = vec[j];
			vec[j] = tmp;
			i++;
			j--;
		}
	};
	/* recursion */
	if (left < j)
		quicksort(vec, left, j);
	if (i < right)
		quicksort(vec, i, right);
}


// get scquare data with w at center point (xc, yc) with width w
vector<GLfloat> getSquareData(GLfloat xc, GLfloat yc, GLfloat w) {
    vector<GLfloat> vec;

    GLfloat r = w / 2; // get radius r of square from w
    // 1st point - lower left
    vec.push_back(xc - r);
    vec.push_back(yc - r);
    vec.push_back(0.0f);

    // 2nd point - lower right
    vec.push_back(xc + r);
    vec.push_back(yc - r);
    vec.push_back(0.0f);

    // 3rd point - upper left
    vec.push_back(xc - r);
    vec.push_back(yc + r);
    vec.push_back(0.0f);

    // 4th point - upper right
    vec.push_back(xc + r);
    vec.push_back(yc + r);
    vec.push_back(0.0f);
    return vec;
}


void setupSquare(GLuint VAO, GLuint VBO, vector<GLfloat> data) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}


int drawStack(vector<int> Rvec) {
    // Init GLFW -- initilizaing GLFW library
    glfwInit();

    // version compatibility with opengl 3.3, core profile, mac computer
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Problem 2 - Example 2", nullptr, nullptr);

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

    // now drawing start here ----------------------------------------------
    // Set up vertex data (and buffer(s)) and attribute pointers
    Shader shaderProgram("vertexShader.vs", "fragmentShader.vs");

    // stack data (4 squares)
    vector<GLfloat> squareData1 = getSquareData(0.0f, -0.3f, 0.3f);
    vector<GLfloat> squareData2 = getSquareData(0.0f, 0.0f, 0.3f);
    vector<GLfloat> squareData3 = getSquareData(0.0f, 0.3f, 0.3f);
    vector<GLfloat> squareData4 = getSquareData(0.0f, 0.6f, 0.3f);


    GLuint VBOs[4], VAOs[4];
    glGenVertexArrays(4, VAOs);
    glGenBuffers(4, VBOs);

    // orginal stack setup
    setupSquare(VAOs[0], VBOs[0], squareData1);
    setupSquare(VAOs[1], VBOs[1], squareData2);
    setupSquare(VAOs[2], VBOs[2], squareData3);
    setupSquare(VAOs[3], VBOs[3], squareData4);

    // unbind all
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer  -- why we clear the colour buffer --> keep reseting the colour buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // re-set the colour state
        glClear(GL_COLOR_BUFFER_BIT); // filling operation of colour state we re-set

        //glUseProgram(shaderProgram);
        shaderProgram.use();

        //drawing original stack
        for (int i = 0; i < 4; i++) {
            // set colour here but may not be the best way performance wise...
            shaderProgram.setColor("userDefinedColor", (float) Rvec[i] / 255, 0.0f, 0.0f, 1.0f);
            glBindVertexArray(VAOs[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 4 vetexes
        }

        // unbind VAO
        glBindVertexArray(0);

        // Swap the screen buffers. This is a double buffering (front buffer and back buffer)
        // 60-75 times per second.
        //
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(4, VAOs);
    glDeleteBuffers(4, VBOs);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}


void problem2() {

    // ----------------------------------------------------------------------------------
    // set up example 1
    // ----------------------------------------------------------------------------------
	cout << "Example 1 ..." << endl;

	//One example - assume no space between first name and last name
	string name = "TakehiroTanaka";
	string sortedName;
	vector<int> numVec;


	// only consider name in upper cases
	transform(name.begin(), name.end(), name.begin(), ::toupper);

	cout << "Original name: " << name << endl;

	// set number A=1, B=2 .. using ASCII code and subtract 64 to set A starting with 1
	for (int i = 0; i < name.size(); i++) {
		numVec.push_back(int(name[i])-64);
		cout << numVec[i] << "(" << name[i] << ")" << " ";
	}

	cout << endl << endl;

	//sort it
	quicksort(numVec, 0, numVec.size()-1);
	// set number A=1, B=2 .. using ASCII code and subtract 64 to set A starting with 1
	char c;
	for (int i = 0; i < numVec.size(); i++) {
		c = char(numVec[i] + 64); // coverting back to ASCII CHAR with offset 64
		sortedName.push_back(c);
		cout << numVec[i] << "(" << c << ")" << " ";
	}

	cout << endl;
	cout << "Sorted name: " << sortedName << endl;


    // ----------------------------------------------------------------------------------
	// set up example 2
    // ----------------------------------------------------------------------------------
	cout << endl << endl;
	cout << "Example 2 ... Press Enter to continue ..." << endl;
    
    GLfloat userElement;
    vector<int> userRvec;
    vector<int> sortedRvec;
    bool tryAgain;
    // prompt user defined R value 4 times and push back to the userRvec
    // basically get 4 int values of vector
    for (int i = 1; i < 5; i++) {
        do {
            tryAgain = false;
            cin.clear();
            cin.ignore(INT_MAX, '\n'); // flashing value
            cout << "Enter the R value "<< i <<" (Enter value only between 0 - 255): ";
            cin >> userElement;
            if (userElement < 0 || userElement > 255 || !cin.good()) {
                cout << "Invalid value. Please Enter the R value between 0 - 255!: " << endl;;
                tryAgain = true;
            }
        } while (tryAgain);
        userRvec.push_back(userElement);
    }

    // Original value  ----------------------------------------
    cout << "Original R value vector: ";
    for (int el : userRvec) {
        cout << el << " ";
    }
    cout << endl << "Drawing original stack ... " << endl;
    drawStack(userRvec);
    cout << endl;


    // sorting ----------------------------------------
    cout << "Sorting R value ..." << endl;
    sortedRvec = userRvec;
    quicksort(sortedRvec, 0, userRvec.size() - 1);
    cout << "Sorted R value vector: ";
    for (int el : sortedRvec) {
        cout << el << " ";
    }
    cout << endl << "Drawing sorted stack ... " << endl;
    drawStack(sortedRvec);
}
