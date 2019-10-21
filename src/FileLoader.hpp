#include <vector>
#include "Row.hpp"

using namespace std;

class FileLoader {
	public:
	vector<Row> readFile();
	Row loadRow(string line);
	string prepareData(string category);
	string prepareCategory(string category);
};
