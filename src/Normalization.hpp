#pragma once
#include "Row.hpp"
#include "omp.h"

class Normalization {
private:
	vector<Row> normalizedData;
	vector<Row> inData;
	double normalizationTime;
	double minMaxTime;
	int inDataSize;
	double min[NUM_OF_FEATURES];
	double max[NUM_OF_FEATURES];
public:
	Normalization(vector<Row> inData) :
			inData(inData),inDataSize(inData.size()) {};
	double getNormalizationTime();
	void normalize(int argc, char *argv[]);
	void findMinMax(int argc, char *argv[]);
	double getMinMaxTime();
	vector<Row> getNormalizedData();
};
