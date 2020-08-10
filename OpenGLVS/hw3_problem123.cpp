
#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <thread>
#include <chrono>

const GLuint WIDTH = 1000, HEIGHT = 1000;
const float PI = 3.14159265359f;
const int NUM_CLUSTER = 3;
const int NUM_SAMPLES = 500; // 14092 is entire samples
const float eps = 1e-5f;

// only works for 2D -- use this for convenience
struct Point {
    float x;
    float y;
};


float distance(Point pt1, Point pt2) {
    return sqrt((pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y));
}


Point meanPoint(vector<Point> set) {
    
    Point pt;

    int numPts = set.size();
    float xValue = 0;
    float yValue = 0;
    for (int i = 0; i < numPts; i++) {
        xValue += set[i].x;
        yValue += set[i].y;
    }
    
    pt.x = xValue / numPts;
    pt.y = yValue / numPts;

    return pt;
}



void kmeansIteration(vector<float> data1, vector<float> data2,
                    vector<vector<float>> &vertices_vec, vector<Point> &z_vec, int num_clust) {
    
    // get xmin, xmax, ymin, ymax
    float xmin = *min_element(data1.begin(), data1.end());
    float xmax = *max_element(data1.begin(), data1.end());
    float ymin = *min_element(data2.begin(), data2.end());
    float ymax = *max_element(data2.begin(), data2.end());

    // define normalization range
    float ax = -1.0f;
    float bx = 1.0f;
    float ay = -1.0f;
    float by = 1.0f;


    int N = data1.size();
    vector<vector<Point>> s_vec;
    s_vec.resize(num_clust);

    // assign each pt to the set corresponding to each class - s_vec
    for (int i = 0; i < N; i++) {
        Point pt;

        pt.x = data1[i];
        pt.y = data2[i];
        
        // find the index corresponding to the point with the minumum distance to the cluster center
        int classIndex = 0;
        float curr_dist = distance(pt, z_vec[0]);
        for (int j = 1; j < num_clust; j++) {
            if (curr_dist > distance(pt, z_vec[j])){
                curr_dist = distance(pt, z_vec[j]);
                classIndex = j;
            }
        }
        
        // Found the class Index, so push pt to the corresponding class set
        s_vec[classIndex].push_back(pt);

        float vertX = normalize(pt.x, xmin, xmax, ax, bx);
        float vertY = normalize(pt.y, ymin, ymax, ay, by);

        vertices_vec[classIndex].push_back(vertX);
        vertices_vec[classIndex].push_back(vertY);
        vertices_vec[classIndex].push_back(0.0f);
    }

    for (int i = 0; i < num_clust; i++) {
        z_vec[i] = meanPoint(s_vec[i]);
    }
}


void getScatterData(vector<float> data1, vector<float> data2, int size,
    vector<float>& scatData, float& xmin, float& xmax, float& ymin, float& ymax) {

    // get xmin, xmax, ymin, ymax
    xmin = *min_element(data1.begin(), data1.end());
    xmax = *max_element(data1.begin(), data1.end());
    ymin = *min_element(data2.begin(), data2.end());
    ymax = *max_element(data2.begin(), data2.end());

    // define normalization range
    float ax = -1.0f;
    float bx = 1.0f;
    float ay = -1.0f;
    float by = 1.0f;

    int numPts = data1.size();
    for (int i = 0; i < numPts; i++) {

        float xValue = normalize(data1[i], xmin, xmax, ax, bx);
        float yValue = normalize(data2[i], ymin, ymax, ay, by);

        scatData.push_back(xValue);
        scatData.push_back(yValue);
        scatData.push_back(0.0f);
    }
}


int drawScatter(vector<float> data1, vector<float> data2) {
    int size = data1.size();

    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW3 Problem 1: Kmeans Clustering");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // for texture for background image
    Shader shaderProgram("vShader.glsl", "fShader.glsl");
    TextRender textRenderProgram(WIDTH, HEIGHT);

    vector<float> vertices;
    vector<vector<float>> vertices_vec;

    float xmin, xmax, ymin, ymax;
    getScatterData(data1, data2, size, vertices, xmin, xmax, ymin, ymax);
    vertices_vec.push_back(vertices);

    //kmeans initialization - 1st step
    vector<Point> z_vec; // cluster centers
    z_vec.resize(NUM_CLUSTER);

    for (int i = 0; i < NUM_CLUSTER; i++) {
        Point pt;
        pt.x = data1[i];
        pt.y = data2[i];
        z_vec[i] = pt;
    }

    // game loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();

        for (int i = 0; i < vertices_vec.size(); i++) {
            if (i == 0) {
                shaderProgram.setColor("userDefinedColor", 0.0f, 0.0f, 1.0f, 1.0f); // blue
            }
            else if (i == 1) {
                shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f); // red
            }
            else if (i == 2) {
                shaderProgram.setColor("userDefinedColor", 0.0f, 1.0f, 0.0f, 1.0f); // green
            }
            else { // needs a better method of dynamically coloring for more than 4 clusters
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                shaderProgram.setColor("userDefinedColor", r, g, b, 1.0f);
            }

            // computationally expensive ... : ( but it works...
            unsigned int VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            setupVAOVBO(VAO, VBO, vertices_vec[i]);
            glDrawArrays(GL_POINTS, 0, vertices_vec[i].size() / 3);
            
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }

        // perform k-means clustering - one iteration -- comment if you don't do kmeans clustering
        vertices_vec.clear();
        vertices_vec.resize(NUM_CLUSTER);
        kmeansIteration(data1, data2, vertices_vec, z_vec, NUM_CLUSTER);

        // Text Rendering has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
        // we may need to modify the textRender program so that we don't need to do conversion ...
        float ax = 0.0f;
        float bx = WIDTH;
        float ay = 0.0f;
        float by = HEIGHT;

        float xloc_xmin = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_xmin = normalize(-1.0f, -1.0f, 1.0f, ay, by);
        float xloc_xmax = normalize(1.0f - 0.30f, -1.0f, 1.0f, ax, bx);
        float yloc_xmax = normalize(-1.0f, -1.0f, 1.0f, ay, by);

        float xloc_ymin = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_ymin = normalize(-1.0f + 0.05f, -1.0f, 1.0f, ay, by);
        float xloc_ymax = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_ymax = normalize(1.0f - 0.05f, -1.0f, 1.0f, ay, by);

        textRenderProgram.RenderText("xmin = " + to_string(xmin), xloc_xmin, yloc_xmin, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("xmax = " + to_string(xmax), xloc_xmax, yloc_xmax, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("ymin = " + to_string(ymin), xloc_ymin, yloc_ymin, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("ymax = " + to_string(ymax), xloc_ymax, yloc_ymax, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));

       
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        //system("pause"); // or just pause and user press by themselves
        
        // for debugging ---------------------------------------------------------------------
        //cout << z_vec[0].x << ", " << z_vec[0].y<< " \t\t" << z_vec[1].x << ", " << z_vec[1].y << endl;
        //cout << vertices_vec[0].size() / 3 <<  " \t\t" << vertices_vec[1].size() / 3 << endl;

        //// define normalization range
        //float ax = -1.0f;
        //float bx = 1.0f;
        //float ay = -1.0f;
        //float by = 1.0f;

        ////system("pause");
        //cout << "cluster 1 ------------------------------------------------------------" << endl;
        //cout << "cluster center: " << normalize(z_vec[0].x, xmin, xmax, ax, bx) << ", " << normalize(z_vec[0].y, ymin, ymax, ay, by) << endl;
        //cout << "num pts: " <<vertices_vec[0].size() / 3 << endl;
        //for (int i = 0; i < vertices_vec[0].size() / 3; i++) {
        //    cout << vertices_vec[0][3*i] << ", " << vertices_vec[0][3 * i] << endl;
        //}

        //////system("pause");
        //cout << "cluster 2 ------------------------------------------------------------" << endl;
        //cout << "cluster center: " << normalize(z_vec[1].x, xmin, xmax, ax, bx) << ", " << normalize(z_vec[1].y, ymin, ymax, ay, by) << endl;
        //cout << "num pts: " << vertices_vec[1].size() / 3 << endl;
        //for (int i = 0; i < vertices_vec[0].size() / 3; i++) {
        //    cout << vertices_vec[1][3 * i] << ", " << vertices_vec[1][3 * i + 1] << endl;
        //}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwTerminate();
}


// -----------------------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------------------
void hw3_problem123() {
    cout << "Processing HW3 dataset ..." << endl;
    // read the data - 3 columns

    ifstream  data("../data/Japan earthquakes 2001 - 2018.csv");
    //ifstream   data("../data/test.csv"); // 8 samples

    vector <float> latitude;
    vector <float> longitude;
    vector <float> depth;

    string line;
    int rowNum = 0;
    
    getline(data, line);
    while (getline(data, line))
    {
        if (rowNum == NUM_SAMPLES) {
            break;
        }

        stringstream  lineStream(line);
        string cell;

        int colNum = 0;
        while (getline(lineStream, cell, ','))
        {

            if (colNum == 1) {
                latitude.push_back(stof(cell));
            }
            else if (colNum == 2) {
                longitude.push_back(stof(cell));
            }
            else if (colNum == 3) {
                depth.push_back(stof(cell));
            }
            colNum++;
        }
        rowNum++;
    }

    cout << "HW3 Problem 1: Clustering the dataset and visualizing ..." << endl;
    drawScatter(latitude, longitude);

    cout << "HW3 Problem 2: Visualizing Decision Boundary Layer ..." << endl;


}