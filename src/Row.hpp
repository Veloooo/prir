#pragma once

#include <string>
#include <vector>
#include "mpi.h"

#define NUM_OF_FEATURES 6
# define MTAG1 1
# define MTAG2 2
# define MTAG3 3
# define MTAG4 4

using namespace std;

class Row {
public:
	vector<double> features;
	int category;
	double distance;
	int prediction;
	Row(vector<double> m_features, int m_category) :
			features(m_features), category(m_category) {
	}
	;
	Row(){};
};

struct CompareRows {
	bool operator ()(const Row &row1, const Row &row2) {
		//kolejność - rosnąco
		if (row1.distance > row2.distance)
			return true;

		if (row1.distance < row2.distance)
			return false;

		return false;
	}
};
