#include "Standarization.hpp"
#include "omp.h"
#include "math.h"
void Standarization::standarize() {
	calcAverage();
	calcDeviation();

	int i,j;
	int num_of_features = NUM_OF_FEATURES;

	double begin = omp_get_wtime();
	#pragma omp parallel for default(none) shared(inData,deviation,average,num_of_features,inDataSize) private (i,j)
	for (i = 0; i < inDataSize; i++) {
		for (j = 0; j < num_of_features; j++) {
			inData[i].features[j] = (inData[i].features[j] - average[j])
					/ deviation[j];
		}
	}

	double end = omp_get_wtime();
	standarizationTime = end-begin;
	standarizedData = inData;
}

void Standarization::calcAverage() {
	double sum[NUM_OF_FEATURES];
	int num_of_features = NUM_OF_FEATURES;
	int i, j;

	double begin = omp_get_wtime();

	#pragma omp parallel for default(none) shared(sum,inData,inDataSize,num_of_features) private(i,j)
	for (i = 0; i < inDataSize; i++) {
		for (j = 0; j < num_of_features; j++) {
			sum[j] += inData[i].features[j];
		}
	}

	#pragma omp parallel for default(none) shared(sum,average,num_of_features,inDataSize) private(j)
	for (j = 0; j < num_of_features; j++) {
		average[j] = sum[j] / inDataSize;
	}

	double end = omp_get_wtime();
	averageCalcTime = end - begin;
}

void Standarization::calcDeviation() {
	double variation[NUM_OF_FEATURES];
	int num_of_features = NUM_OF_FEATURES;
	int i, j;
	double begin = omp_get_wtime();

	#pragma omp parallel for default(none) shared(inData,variation,average,inDataSize,num_of_features) private(i,j)
	for (i = 0; i < inDataSize; i++) {
		for (j = 0; j < num_of_features; j++) {
			variation[j] += pow(inData[i].features[j] - average[j], 2)
					/ inDataSize;
		}
	}

	#pragma omp parallel for default(none) shared(num_of_features,deviation,variation) private(j)
	for (j = 0; j < num_of_features; j++) {
		deviation[j] = sqrt(variation[j]);
	}

	double end = omp_get_wtime();
	deviationTime = end - begin;
}

double Standarization::getDeviationTime(){
	return deviationTime;
}
double Standarization::getAverageCalcTime(){
	return averageCalcTime;
}
double Standarization::getStandarizationTIme(){
	return standarizationTime;
}

vector<Row> Standarization::getStandarizedData(){
	return standarizedData;
}
