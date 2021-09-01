//#include "rapidcsv.h"
#include <string>
#include <iostream>
#include <iostream>
#include <vector>
#include <random>
#include <filesystem>
#include <string>
#include <sstream>

using namespace std;

void DataLoading() {
	string PointerOpneFile("LearningData/");
	string CompanyName("ADBE_data.csv");
	PointerOpneFile.append(CompanyName);

	rapidcsv::Document doc(PointerOpneFile);
	vector<float> OpeningPrice = doc.GetColumn<float>("open");
	vector<float> HighestPrice = doc.GetColumn<float>("high");
	vector<float> LowestPrice = doc.GetColumn<float>("low");
	vector<float> ClosingPrice = doc.GetColumn<float>("close");
	vector<int> VolumeTraded = doc.GetColumn<int>("volume");

	cout << "Read " << ClosingPrice.size() << " values." << endl;
	for (int i = 0; i < OpeningPrice.size(); i++) {
		cout << OpeningPrice[i] << " " << HighestPrice[i] << " " << LowestPrice[i] << " " << ClosingPrice[i] << " " << VolumeTraded[i] << endl;
	}

};