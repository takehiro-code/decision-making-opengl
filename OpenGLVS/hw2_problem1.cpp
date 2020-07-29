
#include "main.h"
#include "shader.h"
#include "textRender.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

const GLuint WIDTH = 800, HEIGHT = 800;
const float PI = 3.14159265359f;
const int BIN_NUM = 30;
const int NUM_SAMPLES = 14092; // 14092 is entire samples
const float eps = 1e-5f;

// dataset from Kaggle Earthquakes in Japan
// https://www.kaggle.com/aerodinamicc/earthquakes-in-japan/data#


// Used from https://gist.github.com/lorenzoriano/5414671
template <typename T>
std::vector<T> linspace(T a, T b, size_t N) {
    T h = (b - a) / static_cast<T>(N - 1);
    std::vector<T> xs(N);
    typename std::vector<T>::iterator x;
    T val;
    for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        *x = val;
    return xs;
}


float gaussian1D(float x, float mean, float variance) {
    return 1 / sqrt(2 * PI * variance) * exp(-pow((x - mean), 2) / (2 * variance));
}


struct histBin {
    float leftRange;
    float rightRange;
    int count;
    float normCount;
};


vector<histBin> getHist(vector<float> data) {

    float xmin = *min_element(data.begin(), data.end());
    float xmax = *max_element(data.begin(), data.end());

    // get Histogram Data
    vector<histBin> hist;

    float bin_width = (float)(xmax - xmin) / BIN_NUM;
    float current_value = xmin;

    for (int i = 0; i < BIN_NUM; i++) {
        histBin bi;
        bi.leftRange = current_value;
        bi.rightRange = current_value + bin_width;
        bi.count = 0;

        for (int j = 0; j < data.size(); j++) {
            if (data[j] >= bi.leftRange && (data[j] < bi.rightRange || abs(bi.rightRange - data[j]) < eps)) {
                // debug
                //cout << data[j] << ",leftRange = " << bi.leftRange << ", rightRange = " <<  bi.rightRange << endl;
                bi.count++;
            }
        }

        bi.normCount = (float)bi.count / (bin_width * data.size());

        current_value = bi.rightRange;
        hist.push_back(bi);
    }
    return hist;
}



void getData(vector<float> data, vector<float> &gaussianData, vector<float> &gridData, vector<float> &histData, 
    float &xmin, float& xmax, float& ymin, float& ymax, float& mean, float &variance) {
    
    // get Histogram Data
    vector<histBin> hist = getHist(data);

    // get xmin, xmax, ymin, ymax
    // ymin and ymax is according to the histogram. Assumptions is that maximum height of histogram is usually higher
    // than the gaussian, so pick ymax from histogram bin height
    xmin = *min_element(data.begin(), data.end());
    xmax = *max_element(data.begin(), data.end());
    ymin = 0;
    ymax = 0;
    for (int i = 0; i < hist.size(); i++) {
        if (ymax < hist[i].normCount) {
            ymax = hist[i].normCount; // update the maximum y value
        }
    }


    for (int i = 0; i < hist.size(); i++) {
        histBin bi = hist[i];
        float leftRangeNorm = (bi.leftRange - xmin) / (xmax - xmin) * 2.0f - 1.0f; // normalize to [-1, 1] for drawing
        float rightRangeNorm = (bi.rightRange - xmin) / (xmax - xmin) * 2.0f - 1.0f; // normalize to [-1, 1] for drawing
        rightRangeNorm = rightRangeNorm - 0.01f; // make a small gap between each bin

        float baseline = (0.0f - ymin) / (ymax - ymin); // normalize to [0, 1] for drawing
        float normCountNorm = (bi.normCount - ymin) / (ymax - ymin); // normalize to [0, 1] for drawing

        // lower left point
        histData.push_back(leftRangeNorm); // x value
        histData.push_back(baseline); // y value
        histData.push_back(0.0f); // z value

        // lower right point
        histData.push_back(rightRangeNorm); // x value
        histData.push_back(baseline); // y value
        histData.push_back(0.0f); // z value

        // upper left point
        histData.push_back(leftRangeNorm); // x value
        histData.push_back(normCountNorm); // y value
        histData.push_back(0.0f); // z value

        // upper right point
        histData.push_back(rightRangeNorm); // x value
        histData.push_back(normCountNorm); // y value
        histData.push_back(0.0f); // z value
    }


    // get gaussian data
    mean = 0;
    for (int i = 0; i < data.size(); i++) {
        mean = mean + data[i];
    }
    mean = mean / data.size();

    variance = 0;
    for (int i = 0; i < data.size(); i++) {
        variance = variance + pow((data[i] - mean), 2);
    }
    variance = variance / (data.size() - 1);

    vector<float> x = linspace(xmin, xmax, 500);
    vector<float> y;

    for (int i = 0; i < x.size(); i++) {
        float xValue = (x[i] - xmin) / (xmax - xmin) * 2.0f - 1.0f; // normalize to [-1, 1] for drawing
        float yValue = gaussian1D(x[i], mean, variance);
        yValue = (yValue - ymin) / (ymax - ymin);  //  normalize [0, 1]

        gaussianData.push_back(xValue);
        gaussianData.push_back(yValue);
        gaussianData.push_back(0.0f);

        y.push_back(yValue); // needed to compute ymin and ymax
    }


    //get gridData
    gridData =
    {
    -1.0f, 0.0f, 1.0f,
     1.0f, 0.0f, 1.0f,
    };


    //for debug
    //for (int i = 0; i < BIN_NUM; i++) {
    //    cout << hist[i].leftRange << ", " << hist[i].rightRange << ", count="  << hist[i].count << "," << hist[i].normCount<< endl;
    //}

    //int i = 0;
    //while (i  < histData.size() - 4) {
    //    cout << histData[i] << ",\t"<< histData[i+1] << ",\t" << histData[i+2] << endl;
    //    i = i + 3;
    //}
}


float getBhattacharyya(vector<float> data1, vector<float> data2) {

    vector<histBin> hist1 = getHist(data1);
    vector<histBin> hist2 = getHist(data2);

    float result = 0;
    for (int i = 0; i < BIN_NUM; i++) {
        result += sqrt(hist1[i].normCount * hist2[i].normCount);
    }
    return result;
}




int drawHw2p1(vector<float> data, const char* title) {
    GLFWwindow* window;
    int EXIT_STAUTS1 = glinit(window, WIDTH, HEIGHT, title);
    if (EXIT_STAUTS1 == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }


    // now drawing start here ----------------------------------------------
    TextRender textRenderProgram(WIDTH, HEIGHT);
    Shader shaderProgram("vShader.glsl", "fShader.glsl");

    // get all the data for rendering
    vector<float>  gaussianData;
    vector<float> gridData;
    vector<float> histData;
    float xmin, xmax, ymin, ymax, mean, variance;

    getData(data, gaussianData, gridData, histData, xmin, xmax, ymin, ymax, mean, variance);

    // setup VAOs, VBOs
    const int numData = 3;
    GLuint VBOs[numData], VAOs[numData];
    glGenVertexArrays(numData, VAOs);
    glGenBuffers(numData, VBOs);

    setupVAOVBO(VAOs[0], VBOs[0], gaussianData);
    setupVAOVBO(VAOs[1], VBOs[1], gridData);
    setupVAOVBO(VAOs[2], VBOs[2], histData);


    // unbind all
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // re-set the colour state
        glClear(GL_COLOR_BUFFER_BIT); // filling operation of colour state we re-set

        shaderProgram.use();

        // histData
        shaderProgram.setColor("userDefinedColor", 0.3f, 0.3f, 1.0f, 1.0f);
        glBindVertexArray(VAOs[2]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, histData.size());

        // gaussian data
        shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_POINTS, 0, gaussianData.size());

        //grid
        shaderProgram.setColor("userDefinedColor", 0.3f, 0.3f, 0.3f, 1.0f);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_LINES, 0, gridData.size());


        // unbind VAO
        glBindVertexArray(0);

        // it has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        textRenderProgram.RenderText(to_string(xmin), 0.0f, (float)HEIGHT / 2 - 20.0f, 0.4f, glm::vec3(0.3f, 0.3f, 0.3f));
        textRenderProgram.RenderText(to_string(xmax), (float)WIDTH - 100.0f, (float)HEIGHT / 2 - 20.0f, 0.4f, glm::vec3(0.3f, 0.3f, 0.3f));
        textRenderProgram.RenderText("sample mean = " + to_string(mean), (float)WIDTH - 300.0f, (float)HEIGHT - 20.0f, 0.4f, glm::vec3(0.3f, 0.3f, 0.3f));
        textRenderProgram.RenderText("sample variance = " + to_string(variance), (float)WIDTH - 300.0f, (float)HEIGHT - 40.0f, 0.4f, glm::vec3(0.3f, 0.3f, 0.3f));


        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(numData, VAOs);
    glDeleteBuffers(numData, VBOs);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}



void hw2_problem1() {

    // read the data - 3 columns
    ifstream  data("Japan earthquakes 2001 - 2018.csv");

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


    // draw two histograms with gaussian distribution
    cout << "Visualizing latitude data ..." << endl;
    drawHw2p1(latitude, "HW2 Problem 1 - Latitude");
    cout << "Visualizing longitude data ..." << endl;
    drawHw2p1(longitude, "HW2 Problem 1 - Longitude");
    //cout << "Visualizing depth data ..." << endl;
    //drawHw2p1(depth, "HW2 Problem 1 - Depth");


    cout << "Compute Bhatacharrya Coefficient ..." << endl;
    float bhattacharyya = getBhattacharyya(latitude, longitude);
    cout << "Bhattacharyya Coefficient of latitude and longitude = " << bhattacharyya << endl;
}