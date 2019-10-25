#include "KNN.hpp"
#include "math.h"
#include <iostream>
#include<algorithm>
#include<random>
#include<queue>

#define CATEGORY_NUM 18

using namespace std;
void KNN::knn(vector<KNNRow> rows, int argc, char *argv[]) {
	int correct = 0;
	int i, islave;
	int K = 5;
	int myCorrect = 0;



//	cout<<"BEFORe PACKING"<<endl;
//	cout<<"SIIIIZEEE : "<<inrows.size()<<endl;
//	for( i=0;i<inrows.size();i++){
//		Row row;
//		row.category = inrows[i].category;
//		for(int j=0;j<NUM_OF_FEATURES;j++){
//			vector<Row> rows;
//			rows[i].features.push_back(inrows[i].features[j]);
//		}
//		rows.push_back(row);
//	}
//	cout<<"AFTER PACKING"<<endl;

	double begin = MPI_Wtime();
	int numprocs, myid;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	cout<<"OSTATNI ELEMENT PRZED SPLIT"<< rows[28055].category << "  feat : "<< rows[28055].features[0]<<endl;
	splitData(rows);



	cout<<"OSTATNI ELEMENT "<< rows[28055].category << "  feat : "<< rows[28055].features[3]<<endl;
	//send information about data size
	if (myid == 0) {
		for (islave = 1; islave < numprocs; islave++) {
			MPI_Send(&testSize, 1, MPI_INTEGER, islave, MTAG1, MPI_COMM_WORLD);
		}
	} else {
		MPI_Recv(&testSize, 1, MPI_INTEGER, 0, MTAG1, MPI_COMM_WORLD, &status);
	}

	//each processor counts prediction for given rows
	for (i = myid; i <= testSize; i += numprocs) {

		int prediction = calcPrediction(testSet[i], K);
		testSet[i].prediction = prediction;
	}

	cout<<"COUTET PREDICTION"<<endl;
	//wait for all processes
	MPI_Barrier (MPI_COMM_WORLD);

	//calc prediction
	for (i = myid; i <= testSize; i += numprocs) {
		if (testSet[i].prediction == testSet[i].category) {
			myCorrect += 1;
		}
	}

	//sum up all processes correct value
	if (myid != 0) {
		MPI_Send(&myCorrect, 1, MPI_INTEGER, 0, MTAG2, MPI_COMM_WORLD);
	} else {
		correct += myCorrect;
		for (islave = 1; islave < numprocs; islave++) {
			MPI_Recv(&myCorrect, 1, MPI_INTEGER, islave,
			MTAG2, MPI_COMM_WORLD, &status);
			correct += myCorrect;
		}
	}

	double end = MPI_Wtime();

	knnTime = end - begin;
	accuracy = correct / (double) testSize * 100.0;

}

int KNN::calcPrediction(KNNRow testVal, int K) {
	int j;
	int votes[CATEGORY_NUM] = { };
	priority_queue<KNNRow, vector<KNNRow>, CompareKNNRow> neighbours;

	for (j = 0; j < trainSize; j++) {
		double dist = calcDist(testVal, trainSet[j]);
		trainSet[j].distance = dist;
		neighbours.push(trainSet[j]);
	}

	for (int i = 0; i < K; i++) {
		int voteCategory = neighbours.top().category;
		neighbours.pop();
		votes[voteCategory] += 1;
	}

	int maxVotes = 0;
	int category = 0;
	for (int i = 0; i < CATEGORY_NUM; i++) {
		if (votes[i] > maxVotes) {
			maxVotes = votes[i];
			category = i;
		}
	}

	return category;
}

double KNN::calcDist(KNNRow test, KNNRow train) {
	double dist = 0;
	int num_of_feaqtures = NUM_OF_FEATURES;
	int k;

	for (k = 0; k < num_of_feaqtures; k++) {
		dist += pow(test.features[k] - train.features[k], 2);
	}

	return dist;
}

void KNN::splitData(vector<KNNRow> inData) {

	auto randomEngine = default_random_engine { };
	shuffle(begin(inData), end(inData), randomEngine);
	trainSize = 0.7 * inData.size();
	testSize = inData.size() - trainSize;

	for (int i = 0; i < trainSize; i++) {
		KNNRow r = KNNRow();
		r.category = inData[i].category;
		for (int j = 0; j < NUM_OF_FEATURES; j++) {
			r.features[j] = inData[i].features[j];
		}

		trainSet.push_back(r);
	}

	for (int i = trainSize; i < inData.size(); i++) {
		KNNRow r = KNNRow();
		r.category = inData[i].category;
		for (int j = 0; j < NUM_OF_FEATURES; j++) {
			r.features[j] = inData[i].features[j];
		}
		testSet.push_back(r);
	}

}
