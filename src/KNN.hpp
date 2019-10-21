#include "KNNRow.hpp"
#include "Row.hpp"
#include <mpi.h>

class KNN
{
public:
	vector<KNNRow> trainSet;
	vector<KNNRow> testSet;
	double knnTime;
	double accuracy;
	void knn(vector<Row> rows, int argc , char * argv []);
	void splitData(vector<Row> allData);
	int calcPrediction(KNNRow testVal, int K);
	int testSize;
	int trainSize;
	double calcDist(KNNRow current,KNNRow trainValue);
};
