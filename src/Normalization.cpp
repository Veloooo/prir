#include "Normalization.hpp"
#include<limits>
#include<iostream>

using namespace std;

int** alloc_2d_int(int rows, int cols) {
	int *data = (int*) malloc(rows * cols * sizeof(int));
	int **array = (int**) malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; i++)
		array[i] = &(data[cols * i]);

	return array;
}

void Normalization::normalize(int argc, char *argv[]) {
	findMinMax(argc, argv);

	int num_of_features = NUM_OF_FEATURES;
	int numprocs, myid, islave;
	int iterationsPerProcess;

	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int i, j;
	int all_categories[inDataSize];
	double all_features[inDataSize * NUM_OF_FEATURES];

	normalizedData.reserve(inDataSize);

	double begin = MPI_Wtime();

	iterationsPerProcess = inDataSize / numprocs;

	//lokalne tablice na kategorie
	double local_category[iterationsPerProcess];
	double local_features[iterationsPerProcess * NUM_OF_FEATURES];

	//odbieranie danych
	double receive_category[iterationsPerProcess];
	double receive_features[iterationsPerProcess * NUM_OF_FEATURES];

	int processIdx = myid * iterationsPerProcess;

	//liczenie min max
	for (i = 0; i < iterationsPerProcess; i++) {
		local_category[i] = inData[processIdx].category;
		for (j = 0; j < NUM_OF_FEATURES; j++) {
			local_features[i * NUM_OF_FEATURES + j] =
					(inData[processIdx].features[j] - min[j])
							/ (max[j] - min[j]);
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
		normalizedData.push_back(row);
	}



	normalizationTime = end - begin;
}



void Normalization::findMinMax(int argc, char *argv[]) {
	int i, j;
	int num_of_features = NUM_OF_FEATURES;
	MPI_Status status;
	double global_min[NUM_OF_FEATURES];
	double global_max[NUM_OF_FEATURES];

	double local_min[NUM_OF_FEATURES];
	double local_max[NUM_OF_FEATURES];

	for (i = 0; i < num_of_features; i++) {
		local_min[i] = numeric_limits<int>::max();
		local_max[i] = numeric_limits<int>::min();
	}

	int myid, numprocs, islave;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	double begin = MPI_Wtime();

	for (i = myid; i < inDataSize; i += numprocs) {
		for (j = 0; j < num_of_features; j++) {

			if (local_min[j] > inData[i].features[j]) {
				local_min[j] = inData[i].features[j];
			}

			if (local_max[j] < inData[i].features[j]) {
				local_max[j] = inData[i].features[j];
			}
		}
	}

	MPI_Barrier (MPI_COMM_WORLD);

	for (int i = 0; i < NUM_OF_FEATURES; i++) {
		MPI_Reduce(&local_min[i], &min[i], 1, MPI_DOUBLE, MPI_MIN, 0,
				MPI_COMM_WORLD);
		MPI_Reduce(&local_max[i], &max[i], 1, MPI_DOUBLE, MPI_MAX, 0,
				MPI_COMM_WORLD);
	}

//pass min /max to all
	if (myid == 0) {
		for (islave = 1; islave < numprocs; islave++) {
			MPI_Send(min, NUM_OF_FEATURES, MPI_DOUBLE, islave, 10,
					MPI_COMM_WORLD);
			MPI_Send(max, NUM_OF_FEATURES, MPI_DOUBLE, islave, 11,
					MPI_COMM_WORLD);

		}
	} else {
		MPI_Recv(min, NUM_OF_FEATURES, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD,
				&status);

		MPI_Recv(max, NUM_OF_FEATURES, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD,
				&status);
	}

	double end = MPI_Wtime();
	minMaxTime = end - begin;

}

double Normalization::getNormalizationTime() {
	return normalizationTime;
}

double Normalization::getMinMaxTime() {
	return minMaxTime;
}

vector<KNNRow> Normalization::getNormalizedData() {
	return normalizedData;

}
