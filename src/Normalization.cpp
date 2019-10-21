#include "Normalization.hpp"
#include<limits>
#include<iostream>
#include "KNNRow.hpp"
#include "KNNRow2.hpp"

using namespace std;

void Normalization::normalize(int argc, char *argv[]) {
	int num_of_features = NUM_OF_FEATURES;
	int i, j, numprocs, myid;
	int  islave;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Status status;
	KNNRow2 data[inDataSize];
	KNNRow2 data2[inDataSize];
	for (i = myid; i < inDataSize; i+=numprocs)
	{
		KNNRow2 temp;
		temp.f1 = inData[i].features[0];
		temp.f2 = inData[i].features[1];
		temp.f3 = inData[i].features[2];
		temp.f4 = inData[i].features[3];
		temp.f5 = inData[i].features[4];
		temp.f6 = inData[i].features[5];
		temp.f7 = inData[i].features[6];
		temp.f8 = inData[i].features[7];
		data[i]=temp;
	}
	cout << "Tutaj 2";
	findMinMax(argc, argv);

	cout << "Tutaj";
	MPI_Datatype dt_point;
	MPI_Type_contiguous(8, MPI_DOUBLE, &dt_point);
	MPI_Type_commit(&dt_point);
	MPI_Barrier(MPI_COMM_WORLD);
while(true){
	for(int i = 0 ; i < 7 ; i++)
	{
		printf("Min: %d Max: %d \n", min[i], max[i]);
	}
}
	double begin = MPI_Wtime();
	/*
	for (i = myid; i < inDataSize; i+=numprocs) {
	  data[i].f1 = ((data[i].f1 - min[0])
				/ (max[0] - min[0]));
	  data[i].f2 = ((data[i].f2 - min[1])
	  				/ (max[1] - min[1]));
	  data[i].f3 = ((data[i].f3 - min[2])
	  				/ (max[2] - min[2]));
	  data[i].f4 = ((data[i].f4 - min[3])
	  				/ (max[3] - min[3]));
	  data[i].f5 = ((data[i].f5 - min[4])
	  				/ (max[4] - min[4]));
	  data[i].f6 = ((data[i].f6 - min[5])
	  				/ (max[5] - min[5]));
	  data[i].f7 = ((data[i].f7 - min[6])
	  				/ (max[6] - min[6]));
	  data[i].f8 = ((data[i].f8 - min[7])
	  				/ (max[7] - min[7]));
	}

	if (myid != 0) {
			MPI_Send(&data, 8, dt_point, 1, 2, MPI_COMM_WORLD);
		}
		else { // Process 1 receives
			  MPI_Recv(&data, 8, dt_point, 0, 0, MPI_COMM_WORLD, &status);
			  for (islave = 1; islave < numprocs; islave++) {
					  MPI_Recv(&data, 8, dt_point, islave, 2, MPI_COMM_WORLD, &status);
			  }
		}

	while(true)
	{
		for (int i=0; i < inDataSize; ++i) {
		  printf("data2[%d].f1 = %f \n",i, data[i].f1);
		}
	}
*/
	double end = MPI_Wtime();
	normalizationTime = end - begin;
	normalizedData = inData;
}

void Normalization::findMinMax(int argc, char *argv[]) {
	int i, j;
	int num_of_features = NUM_OF_FEATURES;
	MPI_Status status;

	double local_min[NUM_OF_FEATURES];
	double local_max[NUM_OF_FEATURES];

	for( i=0;i<num_of_features;i++){
		min[i]= numeric_limits<int>::max();
		local_min[i]= numeric_limits<int>::max();
	}

	int myid, numprocs, islave;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	double begin = MPI_Wtime();

	for ( i = myid; i < inDataSize; i+=numprocs) {
		for (j = 0; j < num_of_features; j++) {
			//printf("PRoces %d przetwarza dane inData[%d][%d] ktore jest rowne : %f\n", world_rank
			//		, i, j, inData[i].features[j]);
			if (local_min[j] > inData[i].features[j]) {
				local_min[j] = inData[i].features[j];
			}

			if (local_max[j] < inData[i].features[j]) {
				local_max[j] = inData[i].features[j];
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	for(int i = 0 ; i < NUM_OF_FEATURES; i++)
	{
		MPI_Reduce(&local_min[i], &min[i], 1, MPI_DOUBLE, MPI_MIN, 0,
			             MPI_COMM_WORLD);
		MPI_Reduce(&local_max[i], &max[i], 1, MPI_DOUBLE, MPI_MAX, 0,
					             MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);


		printf("Local min for process %d - %f %f feature %d\n",
				myid, local_min[i], MPI_Wtime()-begin, i);

		printf("Local max for process %d - %f %f feature %d\n",
				myid, local_max[i], MPI_Wtime()-begin, i);

		MPI_Barrier(MPI_COMM_WORLD);

		if (myid == 0) {
					printf("Total min = %f, max = %f %f feature %d \n\n\n", min[i],
						   max[i] , MPI_Wtime()-begin, i);
				 }
	}
	double end = MPI_Wtime();
	minMaxTime = end - begin;
}

double Normalization::getNormalizationTime() {
	return normalizationTime;
}

double Normalization::getMinMaxTime(){
	return minMaxTime;
}

vector<Row> Normalization::getNormalizedData(){
	return normalizedData;

}
