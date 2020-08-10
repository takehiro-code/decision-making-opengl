

#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <random>


const GLuint WIDTH = 1000, HEIGHT = 1000;
const float PI = 3.14159265359f;

// Formalize the problem:
// User enters the number for their bet (1 - 36, 0, 00, which are 38 possible cases of number).
// Bet amount is x
// Return amount is 35 * x


int drawDecisonMatrix() {
    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW1 Problem 4 (a): Formalized Decision Matrix");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

        // for texture for background image
    Shader shaderProgram("vShader.glsl", "fShader.glsl");
    TextRender textRenderProgram(WIDTH, HEIGHT);
    
    vector<float> vertices = {
        // horizontal lines (2 lines)
        -1.0f, 0.33f, 0.0f,
        1.0f, 0.33f, 0.0f,

        -1.0f, -0.33f, 0.0f,
        1.0f, -0.33f, 0.0f,

        //vertical lines (2 lines)
        -0.33f, -1.0f, 0.0f,
        -0.33f, 1.0f, 0.0f,
        0.33f, -1.0f, 0.0f,
        0.33f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f,
        -0.33f, 0.33f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    setupVAOVBO(VAO, VBO, vertices);

    // unbind all
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


    // game loop
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();
        glBindVertexArray(VAO);
        shaderProgram.setColor("userDefinedColor", 0.5, 0.8f, 0.2f, 1.0f);
        glDrawArrays(GL_LINES, 0, vertices.size() / 3);

        // Text Rendering has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
        // we may need to modify the textRender program so that we don't need to do conversion ...
        float ax = 0.0f;
        float bx = WIDTH;
        float ay = 0.0f;
        float by = HEIGHT;

        float xloc = -1.0f + 0.15f;
        float yloc = 0.66f;
        float xOffset = 0.66f;
        float yOffset = 0.66f;

        float fontSize = 0.4f;
        
        // 1st row
        textRenderProgram.RenderText("State: Win (Prob. = 1 / 38)",
            normalize(xloc + xOffset, -1.0f, 1.0f, ax, bx),
            normalize(yloc, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("State: Lost (Prob. = 37 / 38)",
            normalize(xloc + xOffset * 2, -1.0f, 1.0f, ax, bx),
            normalize(yloc, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        //2nd row
        textRenderProgram.RenderText("Action Bet: $ X",
            normalize(xloc, -1.0f, 1.0f, ax, bx), // x location of text
            normalize(yloc - yOffset, -1.0f, 1.0f, ay, by), // y location of text
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("$ 35 * X (net amount)",
            normalize(xloc + xOffset, -1.0f, 1.0f, ax, bx),
            normalize(yloc - yOffset, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("- $ X (net amount)",
            normalize(xloc + xOffset * 2, -1.0f, 1.0f, ax, bx),
            normalize(yloc - yOffset, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        // 3rd row
        textRenderProgram.RenderText("Action No Bet",
            normalize(xloc, -1.0f, 1.0f, ax, bx), // x location of text
            normalize(yloc - yOffset * 2, -1.0f, 1.0f, ay, by), // y location of text
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("$ 0 (net amount)",
            normalize(xloc + xOffset, -1.0f, 1.0f, ax, bx),
            normalize(yloc - yOffset * 2, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("$ 0 (net amount)",
            normalize(xloc + xOffset * 2, -1.0f, 1.0f, ax, bx),
            normalize(yloc - yOffset * 2, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
}


int game(int betValue, int betAmount) {
    
    int returnAmount = 0;
    int max = 38;
    int min = 1;

    // Source: https://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
    random_device rd;     // only used once to initialise (seed) engine
    mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    uniform_int_distribution<int> uni(min, max); // guaranteed unbiased
    int randValue = uni(rng);

    if (randValue == betValue) {
        returnAmount = 35 * betAmount;
    }
    else {
        returnAmount = 0;
    }
    return returnAmount;
}



int drawDecisonTree(int netAmount) {

    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW1 Problem 4 (b): Formalized Decision Tree with Bet Amount");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // for texture for background image
    Shader shaderProgram("vShader.glsl", "fShader.glsl");
    TextRender textRenderProgram(WIDTH, HEIGHT);

    vector<float> vertices = {
        
        // root level to 1st level
        -0.6f, 0.0f, 0.0f,
        0.0f, 0.3f, 0.0f,

        -0.6f, 0.0f, 0.0f,
        0.0f, -0.3f, 0.0f,

        // 1st level to 2nd level
        0.0f, 0.3f, 0.0f,
        0.6f, 0.6f, 0.0f,

        0.0f, 0.3f, 0.0f,
        0.6f, 0.0f, 0.0f,

        0.0f, -0.3f, 0.0f,
        0.6f, -0.6f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    setupVAOVBO(VAO, VBO, vertices);

    // unbind all
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


    // game loop
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();
        glBindVertexArray(VAO);
        shaderProgram.setColor("userDefinedColor", 0.5, 0.8f, 0.2f, 1.0f);
        glDrawArrays(GL_LINES, 0, vertices.size() / 3);

        // Text Rendering has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
        // we may need to modify the textRender program so that we don't need to do conversion ...
        float ax = 0.0f;
        float bx = WIDTH;
        float ay = 0.0f;
        float by = HEIGHT;



        float fontSize = 0.4f;

        textRenderProgram.RenderText("Bet",
            normalize(-0.5f, -1.0f, 1.0f, ax, bx),
            normalize(0.15f, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("No Bet",
            normalize(-0.5f, -1.0f, 1.0f, ax, bx),
            normalize(-0.15f, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("Win (1/38)",
            normalize(0.1f, -1.0f, 1.0f, ax, bx), // x location of text
            normalize(0.45f, -1.0f, 1.0f, ay, by), // y location of text
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("Lose (37/38)",
            normalize(0.1f, -1.0f, 1.0f, ax, bx), // x location of text
            normalize(0.05f, -1.0f, 1.0f, ay, by), // y location of text
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));


        textRenderProgram.RenderText("Return: $ " + to_string(netAmount),
            normalize(0.65f, -1.0f, 1.0f, ax, bx),
            normalize(0.6f, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("Return: $ " + to_string(netAmount),
            normalize(0.65f, -1.0f, 1.0f, ax, bx),
            normalize(0.0f, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("Return: $ 0",
            normalize(0.65f, -1.0f, 1.0f, ax, bx),
            normalize(-0.6f, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
}


void hw1_problem4() {

	cout << "(a) Formalizing the problem ..." << endl;
	drawDecisonMatrix();
    cout << endl << endl;


    cout << "Staring part (b) ..." << endl;

    int betValue;
    int betAmount;
    int returnAmount;
    int netAmount;
    bool tryAgain;

    do {
        tryAgain = false;
        cin.clear();
        cin.ignore(INT_MAX, '\n'); // flashing value
        cout << "Now enter the bet value between 1 - 38 (1 -36 and 0 for 37, 00 for 38): ";
        cin >> betValue;
        if (betValue < 1 || betValue > 38 || !cin.good()) {
            cout << "Invalid value. Please Enter bet value between 1 - 38: " << endl;;
            tryAgain = true;
        }
    } while (tryAgain);


    do {
        tryAgain = false;
        cin.clear();
        cin.ignore(INT_MAX, '\n'); // flashing value
        cout << "Now enter the bet amount (integer only): ";
        cin >> betAmount;
        if (betAmount < 0 || !cin.good()) {
            cout << "Invalid value. Please Enter valid positive integer of the bet amount!: " << endl;;
            tryAgain = true;
        }
    } while (tryAgain);


    cout << "Playing game ..." << endl;
    returnAmount = game(betValue, betAmount);
    netAmount = returnAmount - betAmount;

    if (netAmount > 0) {
        cout << "Win! You earned $ " << netAmount << " (net amount after subtracting bet amount)" << endl;
    }
    else if (netAmount == 0) {
        cout << "No bet. You earned $ " << netAmount << " (net amount after subtracting bet amount)" << endl;
    }
    else {
        cout << "Lost... Your lost $ " << -netAmount << " (net amount after subtracting bet amount)" << endl;
    }

    cout << "(b) Visualizing the decision tree ..." << endl;
    drawDecisonTree(netAmount);
    cout << "HW1 Problem 4 ends ..." << endl;
}