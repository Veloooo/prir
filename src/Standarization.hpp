#include "Row.hpp"
#include "omp.h"
#include "KNNRow.hpp"

class Standarization {
private:
	vector<KNNRow> standarizedData;
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
	void standarize(int argc, char *argv[]);
	void calcAverage();
	void calcDeviation();
	double getDeviationTime();
	double getAverageCalcTime();
	double getStandarizationTIme();
	vector<KNNRow> getStandarizedData();
};
