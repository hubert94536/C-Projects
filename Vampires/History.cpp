#include "History.h"
#include "globals.h"
#include <iostream>
using namespace std;

History::History(int nRows, int nCols) {
	this->nRows = nRows;
	this->nCols = nCols;
	for (int i = 1; i <= nRows; i++) {
		for (int j = 1; j <= nCols; j++) {
			grid[i - 1][j - 1] = 0;
		}
	}
}

bool History::record(int r, int c) {
	if (r > nRows || r < 1) {
		return false;
	}

	if (c > nCols || c < 1) {
		return false;
	}
	grid[r - 1][c - 1] += 1;
	return true;
}

void History::display() const {
	clearScreen();
	for (int i = 1; i <= nRows; i++) {
		for (int j = 1; j <= nCols; j++) {
			if ((grid[i - 1][j - 1]) == 0) {
				cout << ".";
			}

			else if ((grid[i - 1][j - 1]) < 26) {
				char alphabet = 'A' + grid[i - 1][j - 1] - 1;
				cout << alphabet;
			}

			else {
				cout << 'Z';
			}
		}
		cout << endl;
	}
	cout << endl;
}