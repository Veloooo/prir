#include <string>
#include "Row.hpp"

using namespace std;

class KNNRow{
public:
	double features[NUM_OF_FEATURES];
	int category;
	int prediction;
	double distance;
};

struct CompareKNNRow {
	bool operator ()(const KNNRow &row1, KNNRow  &row2) {
		//kolejność - rosnąco
		if (row1.distance > row2.distance)
			return true;

		if (row1.distance < row2.distance)
			return false;

		return false;
	}
};
