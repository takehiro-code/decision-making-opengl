
#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H


const GLuint WIDTH = 1000, HEIGHT = 1000;
const float PI = 3.14159265359f;

// Formalize the problem:
// User enters the number for their bet (1 - 36, 0, 00, which are 38 possible cases of number).
// Bet amount is x
// Return amount is 35 * x


int drawDecisonMatrix() {
    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW1 Problem 4 (a): Formalized Decisoon Matrix");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

        // for texture for background image
    Shader shaderProgram("vShader.glsl", "fShader.glsl");
    TextRender textRenderProgram(WIDTH, HEIGHT);
    
    vector<float> vertices = {
        // horizontal lines
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        //vertical lines (2 lines)
        -0.33f, -1.0f, 0.0f,
        -0.33f, 1.0f, 0.0f,
        0.33f, -1.0f, 0.0f,
        0.33f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f,
        -0.33f, 0.0f, 0.0f
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
        glDrawArrays(GL_LINES, 0, 8); // 8 vetexes

        // Text Rendering has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
        // we may need to modify the textRender program so that we don't need to do conversion ...
        float ax = 0.0f;
        float bx = WIDTH;
        float ay = 0.0f;
        float by = HEIGHT;

        float xloc = -1.0f + 0.15f;
        float yloc = 0.5f;
        float xOffset = 0.66f;
        float yOffset = 1.0f;

        float fontSize = 0.4f;
        
        textRenderProgram.RenderText("State: Win (Prob. = 1 / 38)",
            normalize(xloc + xOffset, -1.0f, 1.0f, ax, bx),
            normalize(yloc, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

        textRenderProgram.RenderText("State: Lost (Prob. = 37 / 38)",
            normalize(xloc + xOffset * 2, -1.0f, 1.0f, ax, bx),
            normalize(yloc, -1.0f, 1.0f, ay, by),
            fontSize, glm::vec3(0.5, 0.8f, 0.2f));

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


    //cout << "Staring part (b) ..." << endl;
    //cout << " Enter the betted amount:";

    cout << "(b) Visualizing the decision tree ..." << endl;

}