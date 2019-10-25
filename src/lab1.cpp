#include "FileLoader.hpp"
#include "KNN.hpp"
#include <math.h>
#include <iostream>
#include "Normalization.hpp"
#include "Standarization.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	vector<Row> rows = FileLoader().readFile();
	KNN knn = KNN();
	Normalization normalize = Normalization(rows);
	Standarization standarization = Standarization(rows);
	int myid;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	//knn.knn(rows, argc, argv);
	normalize.normalize(argc, argv);
	standarization.standarize(argc, argv);





	knn.knn(standarization.getStandarizedData(), argc, argv);

	if(myid==0){
		cout << "\n Normalization time " << endl;
		cout << normalize.getMinMaxTime() + normalize.getNormalizationTime()
				<< endl;
		cout << "\n Standarization time " << endl;
		cout << standarization.getAverageCalcTime() + standarization.getStandarizationTIme() +
				standarization.getDeviationTime()
				<< endl;
		cout<<"********KNN**************"<<endl;
		cout << "ACC : " << knn.accuracy<< endl;
		cout << "TIME : " << knn.knnTime << endl<<endl;

	}


	MPI_Finalize();


	return 0;
}

