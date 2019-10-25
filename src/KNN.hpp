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
	void knn(vector<KNNRow> rows, int argc , char * argv []);
	void splitData(vector<KNNRow> allData);
	int calcPrediction(KNNRow testVal, int K);
	int testSize;
	int trainSize;
	double calcDist(KNNRow current,KNNRow trainValue);
};
