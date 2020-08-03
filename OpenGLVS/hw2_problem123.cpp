
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


float normalize(float value, float minValue, float maxValue, float a, float b) {
    return (value - minValue) / (maxValue - minValue) * (b - a) + a; // normalize to [a, b] for drawing
}


float computeMean(vector<float> data) {
    float mean = 0.0f;
    for (int i = 0; i < data.size(); i++) {
        mean = mean + data[i];
    }
    mean = mean / data.size();
    return mean;
}


float computeVariance(vector<float> data) {
    float variance = 0;
    float mean = computeMean(data);
    for (int i = 0; i < data.size(); i++) {
        variance = variance + pow((data[i] - mean), 2);
    }
    variance = variance / (data.size() - 1);
    return variance;
}


float computeCovariance(vector<float> data1, vector<float> data2) {
    float mean1 = computeMean(data1);
    float mean2 = computeMean(data2);

    float covariance = 0;
    for (int i = 0; i < data1.size(); i++) {
        covariance = covariance + (data1[i] - mean1) * (data2[i] - mean2);
    }
    covariance = covariance / (data1.size() - 1);
    return covariance;
}


float computeCorrelation(vector<float> data1, vector<float> data2) {
    float covariance = computeCovariance(data1, data2);
    float s1 = sqrtf(computeVariance(data1));
    float s2 = sqrtf(computeVariance(data2));
    return covariance / (s1 * s2);
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


// -----------------------------------------------------------------------------------
// Functions for HW2 Problem 1
// -----------------------------------------------------------------------------------


// vertex data for hw2 problem1
void getAllProblem1Data(vector<float> data, vector<float> &gaussianData, vector<float> &histData,
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


    //define normalization range
    float a = -1.0f;
    float b = 1.0f;

    for (int i = 0; i < hist.size(); i++) {
        
        histBin bi = hist[i];

        float leftRangeNorm = normalize(bi.leftRange, xmin, xmax, a, b); // normalize to [-1, 1] for drawing
        float rightRangeNorm = normalize(bi.rightRange, xmin, xmax, a, b); // normalize to [-1, 1] for drawing

        rightRangeNorm = rightRangeNorm - 0.01f; // make a small gap between each bin

        float baseline = normalize(0.0f, ymin, ymax, a, b); // normalize to [-1, 1] for drawing
        float normCountNorm = normalize(bi.normCount, ymin, ymax, a, b); // normalize to [-1, 1] for drawing


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
    mean = computeMean(data);
    variance = computeVariance(data);

    vector<float> x = linspace(xmin, xmax, 500);
    vector<float> y;

    for (int i = 0; i < x.size(); i++) {
        float xValue = normalize(x[i], xmin, xmax, -1.0f, 1.0f); // normalize to [-1, 1] for drawing
        float yValue = gaussian1D(x[i], mean, variance);
        yValue = normalize(yValue, ymin, ymax, -1.0f, 1.0f);   //  normalize to [0, 1]

        gaussianData.push_back(xValue);
        gaussianData.push_back(yValue);
        gaussianData.push_back(0.0f);

        y.push_back(yValue); // needed to compute ymin and ymax
    }

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




int drawHistGaussian(vector<float> data, const char* title) {
    GLFWwindow* window;
    int EXIT_STAUTS1 = glinit(window, WIDTH, HEIGHT, title);
    if (EXIT_STAUTS1 == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }


    // now drawing start here 
    TextRender textRenderProgram(WIDTH, HEIGHT);
    Shader shaderProgram("vShader.glsl", "fShader.glsl");

    // get all the data for rendering
    vector<float>  gaussianData;
    vector<float> histData;
    float xmin, xmax, ymin, ymax, mean, variance;

    getAllProblem1Data(data, gaussianData, histData, xmin, xmax, ymin, ymax, mean, variance);

    // setup VAOs, VBOs
    const int numData = 2;
    GLuint VBOs[numData], VAOs[numData];
    glGenVertexArrays(numData, VAOs);
    glGenBuffers(numData, VBOs);

    setupVAOVBO(VAOs[0], VBOs[0], gaussianData);
    setupVAOVBO(VAOs[1], VBOs[1], histData);

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
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, histData.size() / 3);

        // gaussian data
        shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_POINTS, 0, gaussianData.size() / 3);


        // unbind VAO
        glBindVertexArray(0);

        // Text Rendering has to be at the end. If you put before the VAO binding for the shaders, the program will malfunction 
        // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
        // we may need to modify the textRender program so that we don't need to do conversion ...
        float ax = 0.0f;
        float bx = WIDTH;
        float ay = 0.0f;
        float by = HEIGHT;

        float xloc_xmin = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_xmin = normalize(-1.0f         , -1.0f, 1.0f, ay, by);
        float xloc_xmax = normalize(1.0f  - 0.30f  , -1.0f, 1.0f, ax, bx);
        float yloc_xmax = normalize(-1.0f         , -1.0f, 1.0f, ay, by);

        float xloc_ymin = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_ymin = normalize(-1.0f + 0.05f, -1.0f, 1.0f, ay, by);
        float xloc_ymax = normalize(-1.0f + 0.01f, -1.0f, 1.0f, ax, bx);
        float yloc_ymax = normalize(1.0f - 0.05f, -1.0f, 1.0f, ay, by);

        textRenderProgram.RenderText("xmin = " + to_string(xmin), xloc_xmin, yloc_xmin, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("xmax = " + to_string(xmax), xloc_xmax, yloc_xmax, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("ymin = " + to_string(ymin), xloc_ymin, yloc_ymin, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("ymax = " + to_string(ymax), xloc_ymax, yloc_ymax, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        
        // outdated way to define location without normalization process ...
        textRenderProgram.RenderText("sample mean = " + to_string(mean), (float)WIDTH - 300.0f, (float)HEIGHT - 20.0f, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));
        textRenderProgram.RenderText("sample variance = " + to_string(variance), (float)WIDTH - 300.0f, (float)HEIGHT - 40.0f, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));


        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(numData, VAOs);
    glDeleteBuffers(numData, VBOs);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------------
// Functions for HW2 Problem 2
// -----------------------------------------------------------------------------------

void getHistData(vector<float> data, int i, int j, int size,
    vector<float> &histData, float& xmin, float& xmax, float& ymin, float& ymax){

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


    // define normalization range
    float ax = -1.0f + j * 2.0f / (float)size;
    float bx = -1.0f + (j + 1) * 2.0f / (float)size;
    float ay = 1.0f - (i+1) * 2.0f / (float)size;
    float by = 1.0f - i * 2.0f / (float)size;

    for (int i = 0; i < hist.size(); i++) {
        histBin bi = hist[i];

        float leftRangeNorm = normalize(bi.leftRange, xmin, xmax, ax, bx); // normalize to [ax, bx] for drawing
        float rightRangeNorm = normalize(bi.rightRange, xmin, xmax, ax, bx); // normalize to [ax, bx] for drawing
        rightRangeNorm = rightRangeNorm - 0.01f; // make a small gap between each bin

        float baseline = normalize(0.0f, ymin, ymax, ay, by); // normalize to [ay, by] for drawing
        float normCountNorm = normalize(bi.normCount, ymin, ymax, ay, by); // normalize to [ay, by] for drawing

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
}



void getScatterData(vector<float> data1, vector<float> data2, int i, int j, int size,
    vector<float>& scatData, float& xmin, float& xmax, float& ymin, float& ymax) {

    // get xmin, xmax, ymin, ymax
    xmin = *min_element(data1.begin(), data1.end());
    xmax = *max_element(data1.begin(), data1.end());
    ymin = *min_element(data2.begin(), data2.end());
    ymax = *max_element(data2.begin(), data2.end());

    // define normalization range
    float ax = -1.0f + j * 2.0f / (float)size;
    float bx = -1.0f + (j + 1) * 2.0f / (float)size;
    float ay = 1.0f - (i + 1) * 2.0f / (float)size;
    float by = 1.0f - i * 2.0f / (float)size;
    // cout << ax << "," << bx << "," << ay << "," << by << endl;

    int numPts = data1.size();
    for (int i = 0; i < numPts; i++) {

        float xValue = normalize(data1[i], xmin, xmax, ax, bx);
        float yValue = normalize(data2[i], ymin, ymax, ay, by);

        scatData.push_back(xValue);
        scatData.push_back(yValue);
        scatData.push_back(0.0f);
    }
}


void getRegressionLine(vector<float> x, vector<float> y, int i, int j, int size,
                                vector<float> &regLineData, float &c0, float &c1){
    // get xmin, xmax, ymin, ymax
    float xmin = *min_element(x.begin(), x.end());
    float xmax = *max_element(x.begin(), x.end());
    float ymin = *min_element(y.begin(), y.end());
    float ymax = *max_element(y.begin(), y.end());

    // define normalization range
    float ax = -1.0f + j * 2.0f / (float)size;
    float bx = -1.0f + (j + 1) * 2.0f / (float)size;
    float ay = 1.0f - (i + 1) * 2.0f / (float)size;
    float by = 1.0f - i * 2.0f / (float)size;

    // get coefficient for c0 and c1 such that y = c0 + c1 * x
    // Psudo code below.
    // c1 = sum( (yi - meanY) * (xi - meanX)) / (xi - meanX)^2;
    // c0 = meanY - c1 * meanX;

    float meanX = computeMean(x);
    float meanY = computeMean(y);

    int numPts = x.size();

    float numerator = 0;
    float denominator = 0;
    for (int i = 0; i < x.size(); i++) {
        numerator = numerator + (y[i] - meanY) * (x[i] - meanX);
        denominator = denominator + (x[i] - meanX) * (x[i] - meanX);
    }
    c1 = numerator / denominator;
    c0 = meanY - c1 * meanX;

    // now create the regressin line vertex data, basically 2 vertex point
    float xminNorm = normalize(xmin, xmin, xmax, ax, bx);
    float yminFitNorm = normalize(c0 + c1 * xmin, ymin, ymax, ay, by);
    
    float xmaxNorm = normalize(xmax, xmin, xmax, ax, bx);
    float Yfitxmax = normalize(c0 + c1 * xmax, ymin, ymax, ay, by);

    regLineData.push_back(xminNorm);
    regLineData.push_back(yminFitNorm);
    regLineData.push_back(0.0f);

    regLineData.push_back(xmaxNorm);
    regLineData.push_back(Yfitxmax);
    regLineData.push_back(0.0f);

    //cout << xminNorm << "," << yminFitNorm << endl;
    //cout << xmaxNorm << "," << Yfitxmax << endl;

}



vector<float> getScatterGridData(int size) {
    
    vector<float> gridData;


    //draw horizontal lines and vertical lines
    for (int i = 1; i < size; i++) {

        float yValue = -1.0f + i * 2.0f / (float) size;
        //left point
        gridData.push_back(-1.0f);
        gridData.push_back(yValue);
        gridData.push_back(0.0f);

        //right point
        gridData.push_back(1.0f);
        gridData.push_back(yValue);
        gridData.push_back(0.0f);

        float xValue = -1.0f + i * 2.0f / (float) size;
        //lower point
        gridData.push_back(xValue);
        gridData.push_back(-1.0f);
        gridData.push_back(0.0f);

        //upper point
        gridData.push_back(xValue);
        gridData.push_back(1.0f);
        gridData.push_back(0.0f);

    }
    return gridData;
}


int drawScatterMatrix(vector<vector<float>> alldata, const char* title, bool regressionFlag) {
    int size = alldata.size();

    GLFWwindow* window;
    int EXIT_STAUTS1 = glinit(window, WIDTH, HEIGHT, title);
    if (EXIT_STAUTS1 == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }


    // now drawing start here 
    TextRender textRenderProgram(WIDTH, HEIGHT);
    Shader shaderProgram("vShader.glsl", "fShader.glsl");

    // get all the data for rendering
    vector<float> gridData = getScatterGridData(size);
    vector<vector<float>> allHistData;
    vector<vector<float>> allScatData;
    vector<vector<float>> allRegLineData;
    vector<float> allxmin, allxmax, allymin, allymax, allc0, allc1;

    // looping through row and col of scatter matrix and poluate the plot data
    for (int i = 0; i < size; i++) {
        vector<float> data1 = alldata[i];
        for (int j = 0; j < size; j++) {
            vector<float> data2 = alldata[j];
            float xmin, xmax, ymin, ymax;
            if (i == j) {
                vector<float> histData;
                getHistData(data1, i, j, size, histData, xmin, xmax, ymin, ymax);
                allHistData.push_back(histData);
            }
            else {
                vector<float> scatData;
                getScatterData(data1, data2, i, j, size, scatData, xmin, xmax, ymin, ymax);
                allScatData.push_back(scatData);

                if (regressionFlag) {
                    vector<float> regLineData;
                    float c0, c1;
                    getRegressionLine(data1, data2, i, j, size, regLineData, c0, c1);
                    allRegLineData.push_back(regLineData);
                    allc0.push_back(c0);
                    allc1.push_back(c1);
                    //cout << c0 << ", " << c1 << endl;
                }
            }

            // I don't use these for text render xmin, xmax, ymin, ymax at the moment, because it will look ugly...
            allxmin.push_back(xmin);
            allxmax.push_back(xmax);
            allymin.push_back(ymin);
            allymax.push_back(ymax);
        }
    }


    // setup VAOs, VBOs
    int numHistData = allHistData.size();
    int numScatData = allScatData.size();

    vector<GLuint> histVBOs(numHistData), histVAOs(numHistData);
    glGenVertexArrays(numHistData, &histVAOs[0]);
    glGenBuffers(numHistData, &histVBOs[0]);

    vector<GLuint> scatVBOs(numScatData), scatVAOs(numScatData);
    glGenVertexArrays(numScatData, &scatVAOs[0]);
    glGenBuffers(numScatData, &scatVBOs[0]);

    GLuint gridVBO, gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    for (int i = 0; i < allHistData.size(); i++) {
        setupVAOVBO(histVAOs[i], histVBOs[i], allHistData[i]);
    }

    for (int i = 0; i < allScatData.size(); i++) {
        setupVAOVBO(scatVAOs[i], scatVBOs[i], allScatData[i]);
    }

    // set up for regression line if flag is true
    int numRegData = allRegLineData.size();
    vector<GLuint> regVBOs(numRegData), regVAOs(numRegData);
    if (regressionFlag) {
        glGenVertexArrays(numRegData, &regVAOs[0]);
        glGenBuffers(numRegData, &regVBOs[0]);
        for (int i = 0; i < allRegLineData.size(); i++) {
            setupVAOVBO(regVAOs[i], regVBOs[i], allRegLineData[i]);
        }
    }

    setupVAOVBO(gridVAO, gridVBO, gridData);

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

        // draw histData
        for (int i = 0; i < allHistData.size(); i++) {
            shaderProgram.setColor("userDefinedColor", 0.3f, 0.3f, 1.0f, 1.0f);
            glBindVertexArray(histVAOs[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, allHistData[i].size() / 3);
        }

        // draw scatData
        for (int i = 0; i < allScatData.size(); i++) {
            shaderProgram.setColor("userDefinedColor", 0.3f, 0.3f, 1.0f, 1.0f);
            glBindVertexArray(scatVAOs[i]);
            glDrawArrays(GL_POINTS, 0, allScatData[i].size() / 3);
        }

        //draw regression line
        if (regressionFlag) {
            for (int i = 0; i < allRegLineData.size(); i++) {
                shaderProgram.setColor("userDefinedColor", 1.0f, 0.0f, 0.0f, 1.0f);
                glBindVertexArray(regVAOs[i]);
                glDrawArrays(GL_LINES, 0, allRegLineData[i].size() / 3);
            }
        }

        // draw gridData
        shaderProgram.setColor("userDefinedColor", 0.3f, 0.3f, 0.3f, 1.0f);
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridData.size() / 3);

        // unbind VAO
        glBindVertexArray(0);

        // render text
        int index = 0; // index for looping allc0 and allc1
        for (int i = 0; i < size; i++) {
            vector<float> data1 = alldata[i];
            for (int j = 0; j < size; j++) {   
                vector<float> data2 = alldata[j];

                // convert the value in rage [-1,1] to [0, WIDTH] for width and [0, HEIGHT]
                // we may need to modify the textRender program so that we don't need to do conversion ...
                float ax = 0.0f;
                float bx = WIDTH;
                float ay = 0.0f;
                float by = HEIGHT;

                float xmin = -1.0f + j * 2.0f / (float)size;
                float xmax = -1.0f + (j + 1) * 2.0f / (float)size;
                float ymin = 1.0f - (i + 1) * 2.0f / (float)size;
                float ymax = 1.0f - i * 2.0f / (float)size;

                // obtain the location within the cell of matrix
                float xloc1 = normalize(xmax - 0.35f, -1.0f, 1.0f, ax, bx);
                float yloc1 = normalize(ymax - 0.025f, -1.0f, 1.0f, ay, by);
                float xloc2 = normalize(xmax - 0.35f, -1.0f, 1.0f, ax, bx);
                float yloc2 = normalize(ymax - 0.05f, -1.0f, 1.0f, ay, by);

                // for the plot title
                float xloc_title = normalize(xmin, -1.0f, 1.0f, ax, bx);
                float yloc_title = normalize(ymax - 0.025f, -1.0f, 1.0f, ay, by);
                string data1title;
                string data2title;
                if (i == 0) {
                    data1title = "latitude";
                }
                else if (i == 1) {
                    data1title = "longitude";
                }
                else {
                    data1title = "depth";
                }

                if (j == 0) {
                    data2title = "latitude";
                }
                else if (j == 1) {
                    data2title = "longitude";
                }
                else {
                    data2title = "depth";
                }

                if (i == j) {
                    float mean = computeMean(data1);
                    float variance = computeVariance(data1);
                    textRenderProgram.RenderText("mean = " + to_string(mean), xloc1, yloc1, 0.3f, glm::vec3(0.9f, 0.9f, 0.9f));
                    textRenderProgram.RenderText("variance = " + to_string(variance), xloc2, yloc2, 0.3f, glm::vec3(0.9f, 0.9f, 0.9f));
                    textRenderProgram.RenderText(data1title, xloc_title, yloc_title, 0.3f, glm::vec3(0.0f, 0.8f, 0.0f));

                }
                else {
                    float covariance = computeCovariance(data1, data2);
                    float correlation = computeCorrelation(data1, data2);
                    textRenderProgram.RenderText("correlation = " + to_string(correlation), xloc1, yloc1, 0.3f, glm::vec3(0.9f, 0.9f, 0.9f));
                    textRenderProgram.RenderText("covariance = " + to_string(covariance), xloc2, yloc2, 0.3f, glm::vec3(0.9f, 0.9f, 0.9f));
                    textRenderProgram.RenderText(data1title + " vs " + data2title, xloc_title, yloc_title, 0.3f, glm::vec3(0.0f, 0.8f, 0.0f));


                    if (regressionFlag) {
                        float xloc3 = normalize(xmax - 0.45f, -1.0f, 1.0f, ax, bx);
                        float yloc3 = normalize(ymax - 0.085f, -1.0f, 1.0f, ay, by);
                        string eqn = "y = " + to_string(allc1[index]) + " * x" + " + " + to_string(allc0[index]);
                        textRenderProgram.RenderText(eqn, xloc3, yloc3, 0.3f, glm::vec3(1.0f, 0.0f, 0.0f));
                        index++;
                    }
                }
            }
        }

        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(numHistData, &histVAOs[0]);
    glDeleteBuffers(numHistData, &histVAOs[0]);
    glDeleteVertexArrays(numScatData, &scatVAOs[0]);
    glDeleteBuffers(numScatData, &scatVBOs[0]);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);

    if (regressionFlag) {
        glDeleteVertexArrays(numRegData, &scatVAOs[0]);
        glDeleteBuffers(numRegData, &scatVBOs[0]);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------------------

void hw2_problem123() {


    cout << "Processing HW2 dataset ..." << endl;
    // read the data - 3 columns
    ifstream  data("../data/Japan earthquakes 2001 - 2018.csv");

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

    // -----------------------------------------------------------------------------------------
    // HW2 Problem 1
    // -----------------------------------------------------------------------------------------

    // draw two histograms with gaussian distribution
    cout << "Visualizing latitude data ..." << endl;
    drawHistGaussian(latitude, "HW2 Problem 1 - Latitude");
    cout << "Visualizing longitude data ..." << endl;
    drawHistGaussian(longitude, "HW2 Problem 1 - Longitude");
    cout << "Visualizing depth data ..." << endl;
    drawHistGaussian(depth, "HW2 Problem 1 - Depth");


    cout << "Compute Bhatacharrya Coefficient ..." << endl;
    float bhattacharyya = getBhattacharyya(latitude, longitude);
    cout << "Bhattacharyya Coefficient of latitude and longitude = " << bhattacharyya << endl;

    cout << endl << endl;


    // -----------------------------------------------------------------------------------------
    // HW2 Problem 2
    // -----------------------------------------------------------------------------------------

    //cout << "Press Enter to start HW2 problem 2 ..." << endl;
    //cin.get();
    //cout << endl << endl;
    vector<vector<float>> alldata = { latitude, longitude, depth};
    cout << "Visualizing Scatter Matrix of latitude, longitude, depth ..." << endl;
    drawScatterMatrix(alldata, "HW2 Problem 2 - Scatter Matrix", false);

    // following is computationally repetitive just for printing into the console ... 
    cout << endl << "Printing covariance matrix ..." << endl;
    for (int i = 0; i < alldata.size(); i++) {
        vector<float> data1 = alldata[i];
        for (int j = 0; j < alldata.size(); j++) {
            vector<float> data2 = alldata[j];
            float covariacne = computeCovariance(data1, data2);
            cout << covariacne << "  ";
        }
        cout << endl;
    }
    cout << endl << endl;

    cout << "Printing correlation matrix ..." << endl;
    for (int i = 0; i < alldata.size(); i++) {
        vector<float> data1 = alldata[i];
        for (int j = 0; j < alldata.size(); j++) {
            vector<float> data2 = alldata[j];
            float correlation = computeCorrelation(data1, data2);
            cout << correlation << "  ";
        }
        cout << endl;
    }

    cout << endl << endl;
    // -----------------------------------------------------------------------------------------
    // HW2 Problem 3
    // -----------------------------------------------------------------------------------------
    cout << "Draw Regression Line on the plot ..." << endl;
    drawScatterMatrix(alldata, "HW2 Problem 3 - Scatter Matrix with Regression Lines", true);
    cout << "HW Problem 1 & 2 & 3 ends ..." << endl;
    cout << endl << endl;
}