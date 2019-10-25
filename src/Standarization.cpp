#include "Standarization.hpp"
#include "math.h"

void Standarization::standarize(int argc, char *argv[]) {
	calcAverage();
	calcDeviation();

	int i, j;
	int num_of_features = NUM_OF_FEATURES;
	MPI_Status status;
	double global_sum[NUM_OF_FEATURES];
	double local_sum[NUM_OF_FEATURES];

	int myid, numprocs, islave;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	int iterationsPerProcess = inDataSize / numprocs;

	int all_categories[inDataSize];
	double all_features[inDataSize * NUM_OF_FEATURES];

	standarizedData.reserve(inDataSize);




	//lokalne tablice na kategorie
	double local_category[iterationsPerProcess];
	double local_features[iterationsPerProcess * NUM_OF_FEATURES];

	//odbieranie danych
	double receive_category[iterationsPerProcess];
	double receive_features[iterationsPerProcess * NUM_OF_FEATURES];

	int processIdx = myid * iterationsPerProcess;

	double begin = MPI_Wtime();
	//liczenie min max
	for (i = 0; i < iterationsPerProcess; i++) {
		local_category[i] = inData[processIdx].category;
		for (j = 0; j < NUM_OF_FEATURES; j++) {
			local_features[i * NUM_OF_FEATURES + j] =
					(inData[processIdx].features[j] - average[j])
							/ deviation[j];
		}
		++processIdx;
	}

	//pakowanie do 2x tablic kategorii i cech
	if (myid == 0) {
		for (i = 0; i < iterationsPerProcess; i++) {
			all_categories[i] = local_category[i];
			for (j = 0; j < NUM_OF_FEATURES; j++) {
				all_features[i * NUM_OF_FEATURES + j] = local_features[i
						* NUM_OF_FEATURES + j];
			}
			++processIdx;
		}

	}

	//przesłanie do głownego procesów lokalnych elementów

	if (myid != 0) {
		MPI_Send(local_category, iterationsPerProcess, MPI_INTEGER, 0, 5,
				MPI_COMM_WORLD);
		MPI_Send(local_features, iterationsPerProcess * NUM_OF_FEATURES,
				MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);

	} else {

		for (islave = 1; islave < numprocs; islave++) {

			MPI_Recv(local_category, iterationsPerProcess, MPI_INTEGER, islave,
					5, MPI_COMM_WORLD, &status);
			MPI_Recv(local_features, iterationsPerProcess * NUM_OF_FEATURES,
					MPI_DOUBLE, islave, 6, MPI_COMM_WORLD, &status);

			int idx = 0;
			int startProc = islave * iterationsPerProcess;

			for (i = startProc; i < startProc + iterationsPerProcess; i++) {
				all_categories[i] = local_category[idx];
				for (j = 0; j < NUM_OF_FEATURES; j++) {
					all_features[i * NUM_OF_FEATURES + j] = local_features[idx
							* NUM_OF_FEATURES + j];
				}

				++idx;
			}

		}

	}


	//zaktualizowanie all_categories we wszystkich procesach
	if (myid == 0) {
		for (islave = 1; islave < numprocs; islave++) {
			MPI_Send(all_categories, inDataSize, MPI_INTEGER, islave, 13,
					MPI_COMM_WORLD);
			MPI_Send(all_features,
					iterationsPerProcess * NUM_OF_FEATURES * numprocs,
					MPI_DOUBLE, islave, 14, MPI_COMM_WORLD);
		}
	} else {
		MPI_Recv(all_categories, inDataSize, MPI_INTEGER, 0, 13, MPI_COMM_WORLD,
				&status);
		MPI_Recv(all_features,
				iterationsPerProcess * NUM_OF_FEATURES * numprocs, MPI_DOUBLE,
				0, 14, MPI_COMM_WORLD, &status);
	}

	double end = MPI_Wtime();

	int featIdx = 0;
	for (i = 0; i < inDataSize; i++) {
		KNNRow row;
		row.category = all_categories[i];

		for (j = 0; j < NUM_OF_FEATURES; j++) {

			row.features[j] = all_features[featIdx++];
		}
		standarizedData.push_back(row);
	}
	standarizationTime = end - begin;
}

void Standarization::calcAverage() {

	int i, j;
	int num_of_features = NUM_OF_FEATURES;
	MPI_Status status;
	double global_sum[NUM_OF_FEATURES];
	double local_sum[NUM_OF_FEATURES];

	int myid, numprocs, islave;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	for (i = 0; i < num_of_features; i++) {
		local_sum[i] = 0;
	}
	double begin = MPI_Wtime();

	for (i = myid; i < inDataSize; i += numprocs){
		for (j = 0; j < num_of_features; j++) {
			local_sum[j] += inData[i].features[j];
		}
	}


	MPI_Barrier (MPI_COMM_WORLD);

	for (int i = 0; i < NUM_OF_FEATURES; i++) {
		MPI_Reduce(&local_sum[i], &global_sum[i], 1, MPI_DOUBLE, MPI_SUM, 0,
				MPI_COMM_WORLD);
	}
	if(myid == 0){
		for (int i = 0; i < NUM_OF_FEATURES; i++)
			average[i] = global_sum[i] / inDataSize;
		for (islave = 1; islave < numprocs; islave++)
			MPI_Send(average, NUM_OF_FEATURES, MPI_DOUBLE, islave, 10,
					MPI_COMM_WORLD);
	}
	else{
		MPI_Recv(average, NUM_OF_FEATURES, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD,
						&status);
	}
	double end = MPI_Wtime();
	averageCalcTime = end - begin;
}

void Standarization::calcDeviation() {
	int i, j;
	int num_of_features = NUM_OF_FEATURES;
	MPI_Status status;
	double global_deviation[NUM_OF_FEATURES];
	double local_deviation[NUM_OF_FEATURES];

	int myid, numprocs, islave;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	for (i = 0; i < num_of_features; i++) {
		local_deviation[i] = 0;
	}
	double begin = MPI_Wtime();

	for (i = myid; i < inDataSize; i += numprocs){
		for (j = 0; j < num_of_features; j++) {
			local_deviation[j] += pow(inData[i].features[j] - average[j], 2)
							/ inDataSize;
		}
	}


	MPI_Barrier (MPI_COMM_WORLD);

	for (int i = 0; i < NUM_OF_FEATURES; i++) {
		MPI_Reduce(&local_deviation[i], &global_deviation[i], 1, MPI_DOUBLE, MPI_SUM, 0,
				MPI_COMM_WORLD);
	}
	if(myid == 0){
		for (int i = 0; i < NUM_OF_FEATURES; i++)
			deviation[i] = sqrt(global_deviation[i]);
		for (islave = 1; islave < numprocs; islave++)
			MPI_Send(deviation, NUM_OF_FEATURES, MPI_DOUBLE, islave, 10,
					MPI_COMM_WORLD);
	}
	else{
		MPI_Recv(deviation, NUM_OF_FEATURES, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD,
						&status);
	}
	double end = MPI_Wtime();
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

vector<KNNRow> Standarization::getStandarizedData(){
	return standarizedData;
}
