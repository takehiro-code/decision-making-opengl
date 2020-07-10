#include "problems.h"

typedef vector<vector<pair<int, int>> > Vec2d;
typedef vector< pair<int, int> > Row; // e.g. row {(2,3), (3,5)}
typedef pair<int, int> Cell; // e.g. cell (2,3)

typedef vector<vector<pair<pair<int, int>, pair<bool, bool>>>> Vec2dMarked; //
typedef vector< pair< pair<int, int>, pair<bool, bool> > > RowMarked; //e.g. {(2,3) [0, 0], ... }
typedef pair< pair<int, int>, pair<bool, bool> >  CellMarked; // (2,3) [0, 0] 
// 0 means false as best response
// 1 means true as best response


class Game 
{
private:
	vector<string> stratSet1; // strategy labels for student 1 - "s1", "s2", "s3" ...
	vector<string> stratSet2;  // strategy labels for student 2 - "s1", "s2", "s3" ...
	Vec2dMarked data;

public:
	// constructor as a setup
	Game(Vec2d vec2d) {
		string strategy;
		char num;
		string label;

		// strategies label for player 1 - you can modify naming however you like
		for (int i = 0; i < vec2d.size(); i++) {
			strategy = "s";
			num = (i + 1) + '0';
			label = strategy + num;
			this->stratSet1.push_back(label);
		}

		// strategies label for player 2 - you can modify naming however you like
		for (int i = 0; i < vec2d[0].size(); i++) {
			strategy = "s";
			num = (i + 1) + '0';
			label = strategy + num;			
			this->stratSet2.push_back(label);
		}

		// data setup
		this->data.resize(vec2d.size()); // allocate memory instead of using push_back
		for (int i = 0; i < vec2d.size(); i++) {
			this->data[i].resize(vec2d.size()); // allocate memory instead of using push_back
			for (int j = 0; j < vec2d[0].size(); j++) {
				this->data[i][j].first = vec2d[i][j]; // copying cell e.g. (2,3) 
				this->data[i][j].second = {false, false}; // make it unmarked (false, false)
			}
		}
	}


	// Solver to find best Pure Strategies Nash Equilibrium (PSNE)
	pair<string, string> solve() {
		int numRows = this->data.size();
		int numCols = this->data[0].size();
		pair<int, int> mutBestRespIndex = {-1, -1}; // assume no PSNE initialized as -1
		
		//Step 1.  Marking the best responses

		// for player 1
		// finding maximum and mark it as true for all columns
		for (int i = 0; i < numRows; i++) {

			// finding maximum and mark it as true at specific column
			int current_max = this->data[0][i].first.first;
			for (int j = 0; j < numCols; j++) {
				if (j > 0) {
					if (this->data[j][i].first.first > current_max) {

						// clear the flags to all false as soon as you found new max at specific columnn
						for (int k = 0; k < numRows; k++) {
							this->data[k][i].second.first = false;
						}
						//update max and mark the max as true
						current_max = this->data[j][i].first.first; // you need this for sure
						this->data[j][i].second.first = true;
					}
					// if you find the same value as maximum
					else if (this->data[j][i].first.first == current_max) {
						this->data[j][i].second.first = true;
					}
				}
				else {
					// assume the first value (j=0) is maximum
					this->data[j][i].second.first = true;
				}
			}
		}

		// for player 2
		// finding maximum and mark it as true for all columns
		for (int i = 0; i < numRows; i++) {

			// finding maximum and mark it as true at specific column
			int current_max = this->data[i][0].first.second;
			for (int j = 0; j < numCols; j++) {
				if (j > 0) {
					if (this->data[i][j].first.second > current_max) {

						// clear the flags to all false as soon as you found new max
						for (int k = 0; k < numCols; k++) {
							this->data[i][k].second.second = false;
						}
						//update max and mark the max as true
						current_max = this->data[i][j].first.second; // you need this for sure
						this->data[i][j].second.second = true;
					}
					// if you find the same value as maximum
					else if (this->data[i][j].first.second == current_max) {
						this->data[i][j].second.second = true;
					}
				}
				else {
					// assume the first value (j=0) is maximum
					this->data[i][j].second.second = true;
				}
			}
		}

		// Step 2. look for mutual best responses and get the corresponding indexes
		for (int i = 0; i < numRows; i++) {
			
			for (int j = 0; j < numCols; j++) {
				// if the specified cell is marked as true (butual best responses)
				if (this->data[i][j].second.first && this->data[i][j].second.second) {
					mutBestRespIndex = { i, j };
				}
			}
		}

		// Step 3. return the corresponding strategies set labels
		if (mutBestRespIndex.first == -1 && mutBestRespIndex.second == -1) {
			return {"None", "None"};
		}
		else {
			return { this->stratSet1[mutBestRespIndex.first], 
				     this->stratSet1[mutBestRespIndex.second] };
		}
	}

	// for debugging purpose
	void printInfo() {
		cout << "Printing matrix ... (payoff) [markedBestResponses]" << endl;
		for (RowMarked row : this->data) {
			for (CellMarked cell : row) {
				cout << "(" << cell.first.first << "," << cell.first.second << ")" << ","
					        << "[" << cell.second.first << ","<< cell.second.second << "]"
					 << "\t\t";
			}
			cout << endl;
		}
		cout << endl << endl;

		cout << "Player 1 strategies set label: ";
		for (string strategy: this->stratSet1) {
			cout  << strategy << "\t";
		}
		cout << endl;

		cout << "Player 2 strategies set label: ";
		for (string strategy : this->stratSet2) {
			cout << strategy << "\t";
		}
		cout << endl << endl;
	}
};


void problem5() {

	// Prisoner Dillenma Matrix
	Vec2d vec2d =
	{ { {-2, -2},	{-10, -1} },
	  { { -1, -10},	{-5,  -5} } };


	// game setup - Prisoner Dillenma
	Game PD(vec2d);
	pair<string, string> result = PD.solve();
	PD.printInfo();

	if (result.second == "None", result.second == "None") {
		cout << "No mutual best response found" << endl;
	}
	else {
		cout << "Mutual best responses is: (strategy for player 1, strategy for player 2) = "
			<< "(" << result.first << ", " << result.second << ")" << endl;
	}
}
