#include "problems.h"
#include "shader.h"


// use gaussian elimination to work with any size of matrix but we only do 2 by 2 matrix only
//vector<float> gaussj(int n, vector<vector<int>> A, vector<int> b);


// for debugging purpose

template <typename T>
void printMatrix(vector<vector<T>> vec2d) {
	cout << "Printing matrix ..." << endl;
	for (int i = 0; i < vec2d.size(); i++) {
		for (int j = 0; j < vec2d[i].size(); j++) {
			cout << vec2d[i][j] << "\t";
		}
		cout << endl;
	}
}


vector<float> getPayoff2by2(vector<vector<pair<int, int>> > vec2d, vector<vector<float>> MSNEsoln) {
	
	int numRows = vec2d.size();
	int numCols = vec2d[0].size();
	vector<float> payoffVec;
	
	vector<vector<float>> confMatrix = 
	{
		{MSNEsoln[0][0] * MSNEsoln[1][0], MSNEsoln[0][0] * MSNEsoln[1][1]},
		{MSNEsoln[0][1] * MSNEsoln[1][0], MSNEsoln[0][1] * MSNEsoln[1][1]}
	};

	// payoff for player 1
	float current_sum = 0;
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++)
			current_sum = current_sum + vec2d[i][j].first * confMatrix[i][j];
	}
	payoffVec.push_back(current_sum);

	// payoff for player 2
	current_sum = 0; // reset this value
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++)
			current_sum = current_sum + vec2d[i][j].second * confMatrix[i][j];
	}
	payoffVec.push_back(current_sum);

	return payoffVec;
}

vector<vector<float>> MSNEsolver2by2(vector<vector<pair<int, int>> > vec2d){
	
	//for player 1
	vector<vector<int>> A = {
	{ vec2d[0][0].second - vec2d[0][1].second, vec2d[1][0].second - vec2d[1][1].second },
	{ 1										 , 1} };
	vector<int> b = { 0, 1 };
	float p = (float) (b[0] * A[1][1] - A[0][1] * b[1]) / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
	vector<float> MSNE1 = { p, 1 - p };

	// for player 2
	A.clear();
	b.clear();
	A = {
	{ vec2d[0][0].first - vec2d[1][0].first, vec2d[0][1].first - vec2d[1][1].first },
	{ 1										 , 1} };
	b = { 0, 1 };
	float q = (float)(b[0] * A[1][1] - A[0][1] * b[1]) / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
	vector<float> MSNE2 = { q, 1 - q };

	return { MSNE1, MSNE2 };
}




void problem6() {
	vector<vector<pair<int, int>> > vec2d = 
	{ { {2, -3},	{1, 2} },
	  { {1,  1},	{4, -1} } };

	vector<vector<float>> MSNEsoln = MSNEsolver2by2(vec2d);
	cout << "Player 1 mixed strategies with probabilities (p, 1-p) = "
		<< "(" << MSNEsoln[0][0] << ", " << MSNEsoln[0][1] << ")" << endl;
	cout << "Player 2 mixed strategies with probabilities (q, 1-q) = "
		<< "(" << MSNEsoln[1][0] << ", " << MSNEsoln[1][1] << ")" << endl;

	vector<float> payoffVec = getPayoff2by2(vec2d, MSNEsoln);
	cout << "Payoff for player 1: " << payoffVec[0] << endl;
	cout << "Payoff for player 2: " << payoffVec[1] << endl;

	cout << "Visualizing this problem ... " << endl;

}