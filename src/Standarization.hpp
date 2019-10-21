#pragma once
#include "Row.hpp"

class Standarization {
private:
	vector<Row> standarizedData;
	vector<Row> inData;
	double average[NUM_OF_FEATURES];
	double deviation[NUM_OF_FEATURES];
	double deviationTime;
	double averageCalcTime;
	double standarizationTime;
	int inDataSize;
public:
	Standarization(vector<Row> inData) :
			inData(inData), inDataSize(inData.size()) {
	}
	;
	void standarize();
	void calcAverage();
	void calcDeviation();
	double getDeviationTime();
	double getAverageCalcTime();
	double getStandarizationTIme();
	vector<Row> getStandarizedData();
};
