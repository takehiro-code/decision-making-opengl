
#include "main.h"

const bool HW1 = false;
const bool HW2 = true;


// The MAIN function that runs from HW1 problem 1 to problem 6
int main()
{
	if (HW1) {
		cout << "HW1 Program starts ..." << endl;
		cout << "Press Enter to start HW1 problem 1 ..." << endl;
		cin.get();
		hw1_problem1();
		cout << endl << endl;


		cout << "Press Enter to start HW1 problem 2 ..." << endl;
		cin.get();
		hw1_problem2();
		cout << endl << endl;


		cout << "Press Enter to start HW1 problem 3 ..." << endl;
		cin.get();
		cin.get();
		hw1_problem3();
		cout << endl << endl;


		cout << "Press Enter to start HW1 problem 4 ..." << endl;
		cin.get();
		hw1_problem4();
		cout << endl << endl;


		cout << "Press Enter to start HW1 problem 5 ..." << endl;
		cin.get();
		hw1_problem5();
		cout << endl << endl;


		cout << "Press Enter to start HW1 problem 6 ..." << endl;
		cin.get();
		hw1_problem6();
		cout << endl << endl;
	}

	if (HW2) {
		cout << "HW2 Program starts ..." << endl;

	}


}
