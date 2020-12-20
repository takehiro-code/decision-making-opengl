
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
const int NUM_CLUSTER = 3; // any number of cluster works in my program
const int NUM_SAMPLES = 50; // 14092 is entire samples
const int ITERATIONS = 1000;
const float eps = 1e-5f;
const float c = 1.0f; //step size


// only works for 2D -- use this for convenience
struct Point {
    float x;
    float y;
};

struct WeightVector {
    float w0;
    float w1;
    float w2;
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


float dotProduct(vector<float> v1, vector<float> v2) {
    float result = 0;
    for (int i = 0; i < v1.size(); i++) {
        result += v1[i] * v2[i];
    }
    return result;
}


vector<float> sumVector(vector<float> v1, vector<float> v2) {
    int size = v1.size();
    vector<float> result(size, 0);
    for (int i = 0; i < size; i++) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}


float linearYsolver(float w0, float w1, float w2, float x) {
    // d (w,x) = w0 * x + w1 * y + w2 = 0
    float y = (-w2 - w0 * x) / w1;
    return y;
}

vector<vector<Point>> kmeansIteration(vector<float> data1, vector<float> data2,
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
    
    return s_vec;
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


// -----------------------------------------------------------------------------------------
// HW 3 Problem 1 Function
// -----------------------------------------------------------------------------------------
int drawHW3P1(vector<float> data1, vector<float> data2) {
    int size = data1.size();

    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW3 Problem 1: K-means Clustering");
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
            else { // needs a better method of dynamically coloring for more than 3 clusters
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
        //ax = -1.0f;
        //bx = 1.0f;
        //ay = -1.0f;
        //by = 1.0f;

        ////system("pause");
        //cout << "cluster 0 ------------------------------------------------------------" << endl;
        //cout << "cluster center: " << normalize(z_vec[0].x, xmin, xmax, ax, bx) << ", " << normalize(z_vec[0].y, ymin, ymax, ay, by) << endl;
        //cout << "num pts: " <<vertices_vec[0].size() / 3 << endl;
        //for (int i = 0; i < vertices_vec[0].size() / 3; i++) {
        //    cout << vertices_vec[0][3*i] << ", " << vertices_vec[0][3 * i] << endl;
        //}

        //////system("pause");
        //cout << "cluster 1 ------------------------------------------------------------" << endl;
        //cout << "cluster center: " << normalize(z_vec[1].x, xmin, xmax, ax, bx) << ", " << normalize(z_vec[1].y, ymin, ymax, ay, by) << endl;
        //cout << "num pts: " << vertices_vec[1].size() / 3 << endl;
        //for (int i = 0; i < vertices_vec[1].size() / 3; i++) {
        //    cout << vertices_vec[1][3 * i] << ", " << vertices_vec[1][3 * i + 1] << endl;
        //}

        // ////system("pause");
        //cout << "cluster 2 ------------------------------------------------------------" << endl;
        //cout << "cluster center: " << normalize(z_vec[2].x, xmin, xmax, ax, bx) << ", " << normalize(z_vec[2].y, ymin, ymax, ay, by) << endl;
        //cout << "num pts: " << vertices_vec[1].size() / 3 << endl;
        //for (int i = 0; i < vertices_vec[2].size() / 3; i++) {
        //    cout << vertices_vec[2][3 * i] << ", " << vertices_vec[2][3 * i + 1] << endl;
        //}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwTerminate();
}



// -----------------------------------------------------------------------------------------
// HW 3 Problem 2 Function
// -----------------------------------------------------------------------------------------

WeightVector classifier(vector<Point> set1, vector<Point> set2) {

    WeightVector w;
    w.w0 = 0;
    w.w1 = 0;
    w.w2 = 0;

    for (int k = 0; k < ITERATIONS; k++) {

        // iterations on the set1
        for (int i = 0; i < set1.size(); i++) {
            Point pt;
            pt.x = set1[i].x;
            pt.y = set1[i].y;

            float value = dotProduct({ w.w0, w.w1, w.w2 }, { pt.x, pt.y, 1.0f });

            // punish
            if (value <= 0) {
                vector<float> w_temp = sumVector({ w.w0, w.w1, w.w2 }, { c * pt.x, c * pt.y, c * 1.0f });
                w.w0 = w_temp[0];
                w.w1 = w_temp[1];
                w.w2 = w_temp[2];
            }

            //cout << w.w0 << ", " << w.w1 << "," << w.w2 << endl;

        }

        // iterations on the set2
        for (int i = 0; i < set2.size(); i++) {
            Point pt;
            pt.x = set2[i].x;
            pt.y = set2[i].y;

            float value = dotProduct({ w.w0, w.w1, w.w2 }, { pt.x, pt.y, 1.0f });

            // punish
            if (value >= 0) {
                vector<float> w_temp = sumVector({ w.w0, w.w1, w.w2 }, { -c * pt.x, -c * pt.y, -c * 1.0f });
                w.w0 = w_temp[0];
                w.w1 = w_temp[1];
                w.w2 = w_temp[2];
            }

            //cout << w.w0 << ", " << w.w1 << "," << w.w2 << endl;

        }
        //cout << w.w0 << ", " << w.w1 << "," << w.w2 << endl;
    }
    return w;
}


struct SetPair {
    float dist;
    int set1_index;
    int set2_index;
};


vector<WeightVector> classifierAll(vector<vector<Point>> s_vec, vector<Point> z_vec) {

    int numLines = s_vec.size() - 1;
    vector<WeightVector> w_vec;


    // determine the two sets to classify
    vector<SetPair> setPair_vec;
    for (int i = 0; i < z_vec.size(); i++) {

        for (int j = 0; j < z_vec.size(); j++) {
            if (j > i) {
                //dist_vec.push_back(distance(z_vec[0], z_vec[1]));
                //set_pair_ind.push_back({ i,j });
                SetPair sp;
                sp.dist = distance(z_vec[i], z_vec[j]);
                sp.set1_index = i;
                sp.set2_index = j;
                setPair_vec.push_back(sp);
            }
        }
    }

    sort(setPair_vec.begin(), setPair_vec.end(),
        [](const auto& i, const auto& j) { return i.dist < j.dist; });

    // if number of lines is greater than the valid number of lines, pop the last element
    while(setPair_vec.size()  > numLines) {
        setPair_vec.pop_back(); // remove the last element until number of lines is valid
    }

    // for debugging
    //for (int i = 0; i < setPair_vec.size(); i++) {
    //    cout << setPair_vec[i].dist << ", " << setPair_vec[i].set1_index << ", " << setPair_vec[i].set2_index << endl;
    //}

    for (int i = 0; i < setPair_vec.size(); i++) {
        int s1_index = setPair_vec[i].set1_index;
        int s2_index = setPair_vec[i].set2_index;

        WeightVector w = classifier(s_vec[s1_index], s_vec[s2_index]);
        w_vec.push_back(w);
    }
    return w_vec;
}



int drawHW3P2(vector<float> data1, vector<float> data2) {
    int size = data1.size();

    GLFWwindow* window;
    int EXIT_STAUTS = glinit(window, WIDTH, HEIGHT, "HW3 Problem 2: Single Perceptron Approach");
    if (EXIT_STAUTS == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // for texture for background image
    Shader shaderProgram("vShader.glsl", "fShader.glsl");
    TextRender textRenderProgram(WIDTH, HEIGHT);

    vector<float> vertices; // legacy vertices ...
    vector<vector<float>> vertices_vec; // points
    vector<vector<Point>> s_vec;


    float xmin, xmax, ymin, ymax;
    //getScatterData(data1, data2, size, vertices, xmin, xmax, ymin, ymax);
    // get xmin, xmax, ymin, ymax
    xmin = *min_element(data1.begin(), data1.end());
    xmax = *max_element(data1.begin(), data1.end());
    ymin = *min_element(data2.begin(), data2.end());
    ymax = *max_element(data2.begin(), data2.end());

    // k-means clustering - no animation, so do it here before while loop
    vector<Point> z_vec; // cluster centers
    z_vec.resize(NUM_CLUSTER);

    for (int i = 0; i < NUM_CLUSTER; i++) {
        Point pt;
        pt.x = data1[i];
        pt.y = data2[i];
        z_vec[i] = pt;
    }

    for (int i = 0; i < ITERATIONS; i++) {
        vertices_vec.clear();
        vertices_vec.resize(NUM_CLUSTER);
        s_vec = kmeansIteration(data1, data2, vertices_vec, z_vec, NUM_CLUSTER);
    }

    // -----------------------------------
    // Single Perceptron Algorithm
    // ------------------------------------

    // initialize a list weight vectors

    int numLines = NUM_CLUSTER - 1;

    // lecture example works...
    cout << "Testing with the lecture example ..." << endl;
    Point s1, s2, s3, s4;
    vector<Point> set1, set2;

    s1.x = 0.0f;
    s1.y = 0.0f;

    s2.x = 0.0f;
    s2.y = 1.0f;

    s3.x = 1.0f;
    s3.y = 0.0f;

    s4.x = 1.0f;
    s4.y = 1.0f;
    
    set1.clear();
    set2.clear();
    set1.push_back(s1);
    set1.push_back(s2);

    set2.push_back(s3);
    set2.push_back(s4);
    WeightVector w_example = classifier(set1, set2);
    cout <<"w0 = " << w_example.w0 << ", w1 = " << w_example.w1 << ", w2 = " << w_example.w2 << endl;
    cout << "Lecture example Validated ..." << endl;
    cout << "Now classfify on the actual examples ..." << endl;

    vector<WeightVector> w_vec = classifierAll(s_vec, z_vec);
    vector<vector<float>> lines;

    for (int i = 0; i < w_vec.size(); i++) {
        WeightVector w = w_vec[i];
        vector<float> line;

        //// define normalization range
        float ax = -1.0f;
        float bx = 1.0f;
        float ay = -1.0f;
        float by = 1.0f;

        line.push_back(normalize(xmin, xmin, xmax, ax, bx));
        line.push_back(normalize(linearYsolver(w.w0, w.w1, w.w2, xmin), ymin, ymax, ay, by));
        line.push_back(0.0f);

        line.push_back(normalize(xmax, xmin, xmax, ax, bx));
        line.push_back(normalize(linearYsolver(w.w0, w.w1, w.w2, xmax), ymin, ymax, ay, by));
        line.push_back(0.0f);

        lines.push_back(line);

        cout << "line #"<<i << ": w0 = " << w.w0 << ", w1 = " << w.w1 << ", w2 = " << w.w2 << endl;

        // for debugging
        //cout << xmin << ", " << linearYsolver(w.w0, w.w1, w.w2, xmin) << endl;
        //cout << xmax << ", " << linearYsolver(w.w0, w.w1, w.w2, xmax) << endl;

        //cout << normalize(xmin, xmin, xmax, ax, bx) << ", " << normalize(linearYsolver(w.w0, w.w1, w.w2, xmin), ymin, ymax, ay, by) << endl;
        //cout << normalize(xmax, xmin, xmax, ax, bx) << ", " << normalize(linearYsolver(w.w0, w.w1, w.w2, xmax), ymin, ymax, ay, by) << endl;
    }




    // game loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();

        // draw points with different color for each cluster
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

        // draw decision boundaries lines
        for (int i = 0; i < lines.size(); i++) {
            unsigned int VBO, VAO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            setupVAOVBO(VAO, VBO, lines[i]);
            shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 1.0f, 1.0f); // purple color of each line
            glDrawArrays(GL_LINES, 0, lines[i].size() / 3);
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }


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
    //ifstream   data("../data/test2.csv");

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

    cout << "Number of Samples chosen: " << NUM_SAMPLES << endl;
    cout << "Number of Clusters chosen: " << NUM_CLUSTER << endl;
    cout << "Iterations chosen: " << ITERATIONS << endl;
    cout << "Step size c chosen: " << c << endl << endl;


    cout << "HW3 Problem 1: Clustering the dataset and visualizing ..." << endl;
    drawHW3P1(latitude, longitude);

    cout << "HW3 Problem 2: Visualizing Decision Boundary Layer ..." << endl;
    drawHW3P2(latitude, longitude);


}