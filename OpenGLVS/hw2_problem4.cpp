
#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// publicly avaiable code, download it and put into your project directory
//https://github.com/nothings/stb/blob/master/stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 


const GLuint WIDTH = 1601, HEIGHT = 897; // manuall taken from the image dimension. I didn't automate this yet.
const float PI = 3.14159265359f;


// ------------------------------------------------------------------------------------
// Graph
// ------------------------------------------------------------------------------------
// data structure to store graph edges

// Modified from Source: https://www.techiedelight.com/graph-implementation-using-stl/

struct Node {
    int nodeNum;
    string nodeLabel;

    // for opengl draw -- draw lines first from (xCent, yCent), 
    //then draw rectangle with xLoc, yLoc, then draw text with xTextLoc, yTextLoc
    float width = 0.1f;
    float xLoc; 
    float yLoc;
    
    float xTextLoc;
    float yTextLoc;

    float xCent;
    float yCent;

    void completeInfo() {
        this->xTextLoc = xLoc;
        this->yTextLoc = yLoc - width / 2.0f;
        
        this->xCent = xLoc + width / 2.0f;
        this->yCent = yLoc - width / 2.0f;
    }
};

struct Edge {
    //int src, dest;
    Node srcNode, destNode;
};

// class to represent a graph object
class Graph
{
public:
    // construct a vector of vectors to represent an adjacency list
    vector<vector<Node>> adjList; // index is nodeNum
    vector<Node> nodeList; // index is nodeNum

    // Graph Constructor
    Graph(vector<Edge> const &edges, int N)
    {
        // resize the vector to N elements of type vector<int>
        adjList.resize(N);
        nodeList.resize(N);

        // add edges to the directed graph
        for (auto &edge : edges)
        {
            // insert at the end
            adjList[edge.srcNode.nodeNum].push_back(edge.destNode);
            nodeList[edge.srcNode.nodeNum] = edge.srcNode;

            // Uncomment below line for undirected graph
            adjList[edge.destNode.nodeNum].push_back(edge.srcNode);
            nodeList[edge.destNode.nodeNum] = edge.destNode;
        }
    }
};


// print adjacency list representation of graph
void printGraph(Graph const& graph, int N)
{
    for (int i = 0; i < N; i++)
    {
        // print current vertex number
        cout << i << " --> ";

        // print all neighboring vertices of vertex 
        for (Node node : graph.adjList[i])
            cout << node.nodeNum << " ";
        cout << endl;
    }

    cout << endl << endl;

    for (Node node : graph.nodeList) {
        cout << node.nodeNum << " ";
    }
    cout << endl;
}



// ------------------------------------------------------------------------------------
// OpenGL
// ------------------------------------------------------------------------------------




int getTexture(const char* filePath) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}



void getVertices(Graph const &graph, int chosenNodeNum,
                 vector<vector<float>> &allRect,
                 vector<vector<float>> &allLine,
                 vector<vector<float>> &allLineRed,
                 vector<pair<float, float>> &allTextPos)
{
    int numOfNode = graph.nodeList.size();

    // looping through each node + complete the node information
    for (int i = 0; i < numOfNode; i++)
    {
        Node node = graph.nodeList[i]; // current Node
        vector<Node> adjNodes = graph.adjList[i];
        vector<float> rect;
        pair<float, float> textPos;

        // add rectangle vertices
        rect.push_back(node.xLoc); // upper left point
        rect.push_back(node.yLoc);
        rect.push_back(0.0f);

        rect.push_back(node.xLoc + node.width); // upper right point
        rect.push_back(node.yLoc);
        rect.push_back(0.0f);

        rect.push_back(node.xLoc + node.width); // lower right point
        rect.push_back(node.yLoc - node.width);
        rect.push_back(0.0f);

        rect.push_back(node.xLoc); // lower left point
        rect.push_back(node.yLoc - node.width);
        rect.push_back(0.0f);
        
        allRect.push_back(rect);

        // add line vertices of adjacent edges -- there will be unhandled duplicates ... 

        for (Node adjNode : adjNodes)
        {
            vector<float> line;
            line.push_back(node.xCent); // 1st point
            line.push_back(node.yCent);
            line.push_back(0.0f);

            line.push_back(adjNode.xCent); // 2nd point
            line.push_back(adjNode.yCent);
            line.push_back(0.0f);

            if (i == chosenNodeNum) {
                allLineRed.push_back(line); // add 2 vertices
            }
            else {
                allLine.push_back(line); // add 2 vertices
            }
        }


        textPos.first = node.xTextLoc;
        textPos.second = node.yTextLoc;
        allTextPos.push_back(textPos);
    }
}





int drawMap(Graph graph, int chosenNodeNum) {


    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW2 Problem 4: Burnaby Campus Map");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // for texture for background image
    //TextRender textRenderProgram(WIDTH, HEIGHT); -- this doesnt't work !!!!! ...

    Shader textureShader("vTexture.glsl", "fTexture.glsl");
    vector<float> imgVertex = {
        // positions           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    setupTextureVAOVBO(VAO, VBO, imgVertex);

    int texture = getTexture("../data/burnabyCampus.jpg");



    Shader shaderProgram("vShader.glsl", "fShader.glsl");

    vector<vector<float>> allRect; // draw all rectangles
    vector<vector<float>> allLine; // draw all edges
    vector<vector<float>> allLineRed; // draw all edges
    vector<pair<float, float>> allTextPos; // text position (xTextPos, yTextPos)


    getVertices(graph, chosenNodeNum, allRect, allLine, allLineRed, allTextPos);


    // set up  for allRect
    const int numRects = allRect.size();
    vector<GLuint> rectVBOs(numRects), rectVAOs(numRects);
    glGenVertexArrays(numRects, &rectVAOs[0]);
    glGenBuffers(numRects, &rectVBOs[0]);
    for (int i = 0; i < numRects; i++) {
        setupVAOVBO(rectVAOs[i], rectVBOs[i], allRect[i]);
    }

    // set up  for allLine
    const int numLines = allLine.size();
    vector<GLuint> lineVBOs(numLines), lineVAOs(numLines);
    glGenVertexArrays(numLines, &lineVAOs[0]);
    glGenBuffers(numLines, &lineVBOs[0]);
    for (int i = 0; i < numLines; i++) {
        setupVAOVBO(lineVAOs[i], lineVBOs[i], allLine[i]);
    }

    // set up  for allLineRed
    const int numLinesRed = allLineRed.size();
    vector<GLuint> lineRedVBOs(numLinesRed), lineRedVAOs(numLinesRed);
    glGenVertexArrays(numLinesRed, &lineRedVAOs[0]);
    glGenBuffers(numLinesRed, &lineRedVBOs[0]);
    for (int i = 0; i < numLinesRed; i++) {
        setupVAOVBO(lineRedVAOs[i], lineRedVBOs[i], allLineRed[i]);
    }

    // game loop
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // doesn't matter
        glClear(GL_COLOR_BUFFER_BIT);

        textureShader.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 4 vetexes

        shaderProgram.use();

        // draw lines
        for (int i = 0; i < numLines; i++) {
            glBindVertexArray(lineVAOs[i]);
            shaderProgram.setColor("userDefinedColor", 0.0f, 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_LINES, 0, allLine[i].size() / 3);
        }

        // draw Red highlighted lines
        for (int i = 0; i < numLinesRed; i++) {
            glBindVertexArray(lineRedVAOs[i]);
            shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_LINES, 0, allLineRed[i].size() / 3);
        }

        // draw rectangle node
        for (int i = 0; i < numRects; i++) {
            glBindVertexArray(rectVAOs[i]);

            if (i == chosenNodeNum) {
                shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f);
            }
            else {
                shaderProgram.setColor("userDefinedColor", 0.0f, 0.0f, 0.0f, 1.0f);
            }
            //glDrawArrays(GL_LINE_LOOP, 0, allRect[i].size() / 3);
            glDrawArrays(GL_TRIANGLE_FAN, 0, allRect[i].size() / 3);
        }

        // text rendering doesn't work !!
        // draw 
        //float ax = 0.0f;
        //float bx = width;
        //float ay = 0.0f;
        //float by = height;
        //float fontSize = 0.3f;

        // unbind VAO
        //for (int i = 0; i < graph.nodeList.size(); i++) {
        //    textRenderProgram.RenderText(graph.nodeList[i].nodeLabel,
        //        normalize(graph.nodeList[i].xTextLoc, -1.0f, 1.0f, ax, bx),
        //        normalize(graph.nodeList[i].yTextLoc, -1.0f, 1.0f, ay, by),
        //        fontSize, glm::vec3(0.5, 0.8f, 0.2f));
        //}


        // unbind VAO
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // for background image
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // for all rectangles (node)
    glDeleteVertexArrays(numRects, &rectVAOs[0]);
    glDeleteBuffers(numRects, &rectVBOs[0]);

    // for all rectangles (node)
    glDeleteVertexArrays(numLines, &lineVAOs[0]);
    glDeleteBuffers(numLines, &lineVBOs[0]);

    // for all rectangles (node)
    glDeleteVertexArrays(numLinesRed, &lineRedVAOs[0]);
    glDeleteBuffers(numLinesRed, &lineRedVBOs[0]);


    glfwTerminate();
    return 0;
}


void hw2_problem4() {


    // vector of graph edges as per above diagram.
    // Please note that initialization vector in below format will
    // work fine in C++11, C++14, C++17 but will fail in C++98.

    // defining 10 distributed locations on the Burnaby Campus
    Node residence;
    residence.nodeNum = 0;
    residence.nodeLabel = "Residence (My Home)";
    residence.xLoc = -0.80f;
    residence.yLoc = 0.075f;
    residence.completeInfo();

    Node dining;
    dining.nodeNum = 1;
    dining.nodeLabel = "Dining hall";
    dining.xLoc = -0.475f;
    dining.yLoc = 0.235f;
    dining.completeInfo();

    Node gym;
    gym.nodeNum = 2;
    gym.nodeLabel = "Gym";
    gym.xLoc = -0.3f;
    gym.yLoc = 0.23f;
    gym.completeInfo();

    Node tennisCourt;
    tennisCourt.nodeNum = 3;
    tennisCourt.nodeLabel = "Tennis court";
    tennisCourt.xLoc = -0.325f;
    tennisCourt.yLoc = -0.05f;
    tennisCourt.completeInfo();

    Node southScienceBuilding;
    southScienceBuilding.nodeNum = 4;
    southScienceBuilding.nodeLabel = "southScienceBuilding";
    southScienceBuilding.xLoc = 0.0f;
    southScienceBuilding.yLoc = 0.0f;
    southScienceBuilding.completeInfo();

    Node MBC;
    MBC.nodeNum = 5;
    MBC.nodeLabel = "MBC";
    MBC.xLoc = -0.1f;
    MBC.yLoc = 0.15f;
    MBC.completeInfo();

    Node Library;
    Library.nodeNum = 6;
    Library.nodeLabel = "Library";
    Library.xLoc = -0.1f;
    Library.yLoc = 0.35f;
    Library.completeInfo();

    Node AQPond;
    AQPond.nodeNum = 7;
    AQPond.nodeLabel = "AQ Pond";
    AQPond.xLoc = 0.05f;
    AQPond.yLoc = 0.3f;
    AQPond.completeInfo();

    Node CornorStone;
    CornorStone.nodeNum = 8;
    CornorStone.nodeLabel = "Cornorstone";
    CornorStone.xLoc = 0.35f;
    CornorStone.yLoc = 0.275f;
    CornorStone.completeInfo();

    Node ASB;
    ASB.nodeNum = 9;
    ASB.nodeLabel = "ASB";
    ASB.xLoc = 0.225f;
    ASB.yLoc = 0.15f;
    ASB.completeInfo();
       
    vector<Edge> edges;
    edges.push_back({ residence, dining });
    edges.push_back({ residence, tennisCourt });
    edges.push_back({ dining, gym });
    edges.push_back({ gym, tennisCourt });
    edges.push_back({ gym, ASB });
    edges.push_back({ tennisCourt, southScienceBuilding });
    edges.push_back({ southScienceBuilding, MBC });
    edges.push_back({ MBC, Library });
    edges.push_back({ MBC, AQPond });
    edges.push_back({ Library, AQPond });
    edges.push_back({ AQPond, CornorStone });
    edges.push_back({ CornorStone, ASB });

    // Number of nodes in the graph
    int N = 10;

    // construct graph
    Graph graph(edges, N);

    // print adjacency list representation of graph for debugging
    //printGraph(graph, N);

    cout << "Enter the integer number 0 - 9 for choosing a node." << endl;
    cout << "Residence (0)" << endl;
    cout << "Dining Hall (1)" << endl;
    cout << "Gym (2)" << endl;
    cout << "Tennis court (3)" << endl;
    cout << "South Science Building (4)" << endl;
    cout << "MBC (5)" << endl;
    cout << "Library (6)" << endl;
    cout << "AQ Pond (7)" << endl;
    cout << "Cornorstone (8)" << endl;
    cout << "ASB (9)" << endl;
    cout << "don't want to choose (-1)" << endl;
    cout << "... Now Press Enter again if necessary ..." << endl; 

    int chosenNodeNum;
    bool tryAgain;

    do {
        tryAgain = false;
        cin.clear();
        cin.ignore(INT_MAX, '\n'); // flashing value
        cout << "Now enter node number (Enter value only between 0 - 9 or -1): ";
        cin >> chosenNodeNum;
        if (chosenNodeNum < -1 || chosenNodeNum > 9 || !cin.good()) {
            cout << "Invalid value. Please Enter the R value between 0 - 9 or -1!: " << endl;;
            tryAgain = true;
        }
    } while (tryAgain);

    drawMap(graph, chosenNodeNum);

}