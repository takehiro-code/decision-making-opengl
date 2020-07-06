
#include "problems.h"

const float ALPHA = 0.25;
// for now, the decision matrix only works with integer. Use template to work with float values!


// better to use class as ActVector to avoid code duplicate.. too late to change now : (  
struct Act 
{
    string label;
    int max;
    int min;
    vector<int> act;
    float optPsmValue;
    float PoIValue; // Principal of Insufficient value
    
    void getOptPsmValue(float alpha) {
        this->optPsmValue = this->max * alpha + this->min * (1 - alpha);
    }

    void getPoIValue() {
        float size = act.size();
        float sumValue = 0;

        for (auto value : act) {
            sumValue += value;
        }
        this->PoIValue = sumValue / size;
    }
};


// print matrix -- used for debugging. to print matrix, simply call this function.
void printMatrix(vector<vector<int>> vec2d) {
    cout << "printing matrix ..." << endl;
    for (int i = 0; i < vec2d.size(); i++) {
        for (int j = 0; j < vec2d[0].size(); j++) {
            cout << vec2d[i][j] << "\t";
        }
        cout << endl;
    }
}


// create act object from each row of vector and return a vector of act object
vector<Act> setupActVec(vector<vector<int>> vec2d) {
    vector<Act> actVec;
    Act actObj;

    //these two variables for labeling "a1", "a2", ..
    string act;
    char num;
    string label;

    // setup of actVec
    for (int i = 0; i < vec2d.size(); i++) {

        // get max value from each row and get vector as row
        actObj.max = *max_element(vec2d[i].begin(), vec2d[i].end());
        actObj.min = *min_element(vec2d[i].begin(), vec2d[i].end());
        actObj.act = vec2d[i];

        //get label
        act = "a";
        num = (i + 1) + '0'; // convert number to char
        label = act + num;
        actObj.label = label;

        actVec.push_back(actObj);
    }
    return actVec;
}


vector<Act> maximin(vector<vector<int>> vec2d) {

    vector<Act> actVec = setupActVec(vec2d);

    vector<Act> bestActVec; // chosen best acts from actVec

    //using iterator to find maximum min value from actVec
    vector<Act>::iterator it = actVec.begin();
    Act actObj_max = *it;
    int current_max = actObj_max.min;
    for (; it != actVec.end(); ++it) {
        // for the first element, no operation
        if (it != actVec.begin()) {
            // if the value is greater than current_max, clear the bestActVec and push_back
            if ((*it).min > current_max) {
                bestActVec.clear();
                current_max = (*it).min;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
            // if the value is equal to the current_max, don't clear but just push back
            else if ((*it).min == current_max) {
                current_max = (*it).min;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
        }
        else {
            bestActVec.push_back(actObj_max);
        }
    }
    return bestActVec;
}


vector<Act> maximax(vector<vector<int>> vec2d) {

    vector<Act> actVec = setupActVec(vec2d);

    vector<Act> bestActVec; // chosen best acts from actVec

    //using iterator to find maximum max value from actVec
    vector<Act>::iterator it = actVec.begin();
    Act actObj_max = *it;
    int current_max = actObj_max.max;
    for (; it != actVec.end(); ++it) {
        if (it != actVec.begin()) {
            if ((*it).max > current_max) {
                bestActVec.clear();
                current_max = (*it).max;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
            else if ((*it).max == current_max) {
                current_max = (*it).max;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
        }
        else {
            bestActVec.push_back(actObj_max);
        }
    }
    return bestActVec;
}


vector<Act> minimaxRegret(vector<vector<int>> vec2d) {

    //subtract the max value from each column
    for (int i = 0; i < vec2d.size(); i++) {

        // get maximum value from each column
        int current_max = vec2d[0][i];
        for (int j = 0; j < vec2d.size(); j++) {
            if (vec2d[j][i] > current_max) {
                current_max = vec2d[j][i];
            }
        }

        // subtract its maximum value from the column
        for (int j = 0; j < vec2d.size(); j++) {
            vec2d[j][i] = vec2d[j][i] - current_max;
        }
    }

    // choose acts that maximizes (minimizes the regret value) the minimum value
    // (maximum regret value) from each act
    return maximin(vec2d);
}


vector<Act> optimismPessimism(vector<vector<int>> vec2d, float alpha) {
    
    vector<Act> actVec = setupActVec(vec2d);
    vector<Act> bestActVec;

    for (int i = 0; i < actVec.size(); i++) {
        actVec[i].getOptPsmValue(alpha);
    }
     
    //// this one won't work because this doesn't update the actVec!
    //for (Act act : actVec) {
    //    act.getOptPsmValue(alpha);
    //}

    //using iterator to find maximum optPsm value from actVec
    vector<Act>::iterator it = actVec.begin();
    Act actObj_max = *it;
    float current_max = actObj_max.optPsmValue;
    for (; it != actVec.end(); ++it) {
        // for the first element, no operation
        if (it != actVec.begin()) {
            // if the value is greater than current_max, clear the bestActVec and push_back
            if ((*it).optPsmValue > current_max) {
                bestActVec.clear();
                current_max = (*it).optPsmValue;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
            // if the value is equal to the current_max, don't clear but just push back
            else if ((*it).optPsmValue == current_max) {
                current_max = (*it).optPsmValue;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
        }
        else {
            bestActVec.push_back(actObj_max);
        }
    }
    return bestActVec;
}


vector<Act> principleOfInsufficient(vector<vector<int>> vec2d) {

    vector<Act> actVec = setupActVec(vec2d);
    vector<Act> bestActVec;

    for (int i = 0; i < actVec.size(); i++) {
        actVec[i].getPoIValue();
    }

    //using iterator to find maximum optPsm value from actVec
    vector<Act>::iterator it = actVec.begin();
    Act actObj_max = *it;
    float current_max = actObj_max.PoIValue;
    for (; it != actVec.end(); ++it) {
        if (it != actVec.begin()) {
            // if the value is greater than current_max, clear the bestActVec and push_back
            if ((*it).PoIValue > current_max) {
                bestActVec.clear();
                current_max = (*it).PoIValue;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
            // if the value is equal to the current_max, don't clear but just push back
            else if ((*it).PoIValue == current_max) {
                current_max = (*it).PoIValue;
                actObj_max = *it;
                bestActVec.push_back(actObj_max);
            }
        }
        else {
            bestActVec.push_back(actObj_max);
        }
    }
    return bestActVec;
}



void problem3() {

    //2d vector
    vector<vector<int> > vec2d{ { 20, 20, 0,  10 },
                               { 10, 10, 10, 10 },
                               { 0,  40, 0,  0  },
                               { 10, 30, 0,  0  } };
    vector<Act> bestActVec;
    printMatrix(vec2d);

    cout << "(a) Using Maximin Rule ..." << endl;
    bestActVec = maximin(vec2d);
    cout << "Choose act(s): " << endl;
    for (Act bestAct : bestActVec) {
        cout << "\t - " << bestAct.label << " with minimum value " << bestAct.min << endl;
    }

    cout << "(b) Using Maximax Rule ..." << endl;
    bestActVec = maximax(vec2d);
    cout << "Choose act(s): " << endl;
    for (Act bestAct : bestActVec) {
        cout << "\t - " << bestAct.label << " with minimum value " << bestAct.max << endl;
    }

    cout << "(c) Using Minimax Regret Rule ..." << endl;
    bestActVec = minimaxRegret(vec2d);
    cout << "Choose act(s): " << endl;
    for (Act bestAct : bestActVec) {
        cout << "\t - " << bestAct.label << " with maximum regret value " << bestAct.max << endl;
    }

    cout << "(d) Using the optimism-pessimism Rule ..." << endl;
    bestActVec = optimismPessimism(vec2d, ALPHA);
    cout << "Choose act(s): " << endl;
    for (Act bestAct : bestActVec) {
        cout << "\t - " << bestAct.label << " with Optimism-Persimism value " << bestAct.optPsmValue << endl;
    }

    cout << "(e) Using the Principal of Insufficient Rule ..." << endl;
    bestActVec = principleOfInsufficient(vec2d);
    cout << "Choose act(s): " << endl;
    for (Act bestAct : bestActVec) {
        cout << "\t - " << bestAct.label << " with expected value " << bestAct.PoIValue << endl;
    }
}

