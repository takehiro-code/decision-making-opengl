
#include "problems.h"

// Define constant varibles here instead of header files to avoid compiling/linking issue 
// Window dimensions to be used in the call-back window size
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders ------ Extra steps to create the shader setup (not needed if not worry about shading)
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const float PI = 3.14159265359f;

vector<GLfloat>  getCircleData(GLfloat xc, GLfloat yc, GLfloat r, int numVerts);


// function that constructs the cirlce data taking a circle center coordinate of (xc, yc) with radius r
vector<GLfloat> getCircleData(GLfloat xc, GLfloat yc, GLfloat r, int numVerts) {

    vector<GLfloat> vec;
    GLfloat x;
    GLfloat y;
    GLfloat z = 0.0f;
    GLfloat theta = 0.0f;
    float stepSize = 2.0f * PI / numVerts;

    //add center point
    vec.push_back(xc);
    vec.push_back(yc);
    vec.push_back(z);


    while (theta < 2.0f * PI) {
        
        x = xc + r * cosf(theta);
        y = yc + r * sinf(theta);

        vec.push_back(x);
        vec.push_back(y);
        vec.push_back(z);

        theta = theta + stepSize;
        //cout << x << "  " << y << "  " << endl;
    }

    return vec;
}


int problem1() {

    // Init GLFW -- initilizaing GLFW library
    glfwInit();


    // version compatibility with opengl 3.3, core profile, mac computer
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
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
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);


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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // now drawing start here ----------------------------------------------
    // Set up vertex data (and buffer(s)) and attribute pointers

    // 1. get triangle data center at (0, 0)
    GLfloat triangleData[] =
    {
        -0.2f, -0.2f, 0.0f, // lower left
        0.2f, -0.2f, 0.0f, //  lower right
        0.0f, 0.2f, 0.0f   // upper 
    };

    // 2. get square data center at (-0.7, -0.7) with width 0.2
    GLfloat squareData[] =
    {
        -0.9f, -0.9f, 0.0f, // lower left
        -0.5f, -0.9f, 0.0f, // lower right
        -0.9f, -0.5f, 0.0f,   // upper left
        -0.5f, -0.5f, 0.0f, // upper right
    };

    // get circle data center at (0.5, 0.5) with radius 0.2
    int numpts = 360;
    vector<GLfloat>  circleData = getCircleData(0.5f, 0.5f, 0.2f, numpts);
    
    GLuint VBOs[3], VAOs[3];
    glGenVertexArrays(3, VAOs); // changed to 1 to 2 if you want to use both arrays defined above
    glGenBuffers(3, VBOs);  // changed to 1 to 2 or to more if you are using more data sets

    // triangle setup
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleData), triangleData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // square setup
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareData), squareData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // circle setup
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circleData.size(), &circleData[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

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

        glUseProgram(shaderProgram);

        // Draw triangle
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 vetexes

        // Draw triangle
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 4 vetexes

        // Draw Circle
        glBindVertexArray(VAOs[2]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, numpts+2); // numpts + center point + overlap ending point 

        // unbind VAO
        glBindVertexArray(0);

        // Swap the screen buffers. This is a double buffering (front buffer and back buffer)
        // 60-75 times per second.
        //
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(3, VBOs);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}