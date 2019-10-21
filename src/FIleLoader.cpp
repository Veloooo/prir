#include "FileLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

vector<Row> FileLoader::readFile() {

	ifstream file("krkopt.csv", ios::in);
	string line;
	vector<Row> rows;

	while (getline(file, line)) {
		rows.push_back(loadRow(line));
	}

	file.close();
	return rows;
}

Row FileLoader::loadRow(string line) {
	vector<double> features;
	string word;
	stringstream s(line);

	for (int i = 0; i < NUM_OF_FEATURES; i++) {
		getline(s, word, ',');
		string preparedWord = prepareData(word);
		features.push_back(atof(preparedWord.c_str()));
	}

	getline(s, word, ',');
	string category = prepareCategory(word);
	return Row(features, atoi(category.c_str()));

}

string FileLoader::prepareData(string word) {


	if(word.compare("a")==0){
		return "1";
	}else if(word.compare("b")==0){
		return "2";
	}else if(word.compare("c")==0){
		return "3";
	}else if(word.compare("d")==0){
		return "4";
	}else if(word.compare("e")==0){
		return "5";
	}else if(word.compare("f")==0){
		return "6";
	}else if(word.compare("g")==0){
		return "7";
	}else if(word.compare("h")==0){
		return "8";
	}

	return word;
}

string FileLoader::prepareCategory(string word){
	if(word.compare("draw")==0){
		return "0";
	}else if(word.compare("zero")==0){
		return "1";
	}else if(word.compare("one")==0){
		return "2";
	}else if(word.compare("two")==0){
		return "3";
	}else if(word.compare("three")==0){
		return "4";
	}else if(word.compare("four")==0){
		return "5";
	}else if(word.compare("five")==0){
		return "6";
	}else if(word.compare("six")==0){
		return "7";
	}else if(word.compare("seven")==0){
		return "8";
	}else if(word.compare("eight")==0){
		return "9";
	}else if(word.compare("nine")==0){
		return "10";
	}else if(word.compare("ten")==0){
		return "11";
	}else if(word.compare("eleven")==0){
		return "12";
	}else if(word.compare("twelve")==0){
		return "13";
	}else if(word.compare("thirteen")==0){
		return "14";
	}else if(word.compare("fourteen")==0){
		return "15";
	}else if(word.compare("fifteen")==0){
		return "16";
	}else if(word.compare("sixteen")==0){
		return "17";
	}

	return word;
}
