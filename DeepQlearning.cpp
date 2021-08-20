// DeepQlearning.cpp : Program was written by Thomas Nazler : Started on 5. 8. 2021.

#include <iostream>
#include <vector>
#include <random>
#include <filesystem>
#include <string>
#include <sstream>
#include <curl/curl.h>
//#include <csv.h>
//#include "ParsingCsv.cpp"
//#include <rapidcsv.h>


using namespace std;

void DataLoading();

int main() {
	int SampleSize = 10000;												//Add here the number of tries for choosing the company 
	int LengthOfTrainingPeriod = 365;									//How many days are wee leting the model train
	int NumberOfRepetitions = 5;										//How many times to look at the specified time period
	int PercentageGain = 100;											//How many percent gained in the time frame
	int StartingCapital = 100000;										//with how much money does the model start
	vector <string> FilePaths;
	string path = "LearningData";										//Location of training data
	

	for (const auto& file : filesystem::directory_iterator(path)) {	//Creating vector including file paths to all trained companies
		string CompanyName;
		stringstream ss;
		ss << file.path();
		ss >> CompanyName;
		FilePaths.push_back(CompanyName);
	}

	CURL* hnd = curl_easy_init();

	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_URL, "https://twelve-data1.p.rapidapi.com/quote?symbol=AMZN&interval=1day&outputsize=30&format=json");

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "x-rapidapi-key: a1580b46cfmsh1f5c152d28a988bp10b686jsn46baf1858144");
	headers = curl_slist_append(headers, "x-rapidapi-host: twelve-data1.p.rapidapi.com");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

	CURLcode ret = curl_easy_perform(hnd);


	for (int i = 0; i < SampleSize; i++) {
		random_device dev;
		mt19937 rng(dev());
		//cout << FilePaths.size() << " This is the total amount of files" << endl;
		uniform_int_distribution<std::mt19937::result_type> dist6(0,FilePaths.size()); 
		int  FilePiccker;
		//cout << dist6(rng) << endl;
		FilePiccker = dist6(rng);
		cout << FilePiccker << "  " ;
		
	}

	//DataLoading();*/
}

/*void DataLoading() {
	string PointerOpneFile ("LearningData/");
	string CompanyName ("ADBE_data.csv");
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
	
};*/

void AccuracyCalculation(int x, int y) {

}
// Tipy pro zahájení práce:
//   1. K přidání nebo správě souborů použijte okno Průzkumník řešení.
//   2. Pro připojení ke správě zdrojového kódu použijte okno Team Explorer.
//   3. K zobrazení výstupu sestavení a dalších zpráv použijte okno Výstup.
//   4. K zobrazení chyb použijte okno Seznam chyb.
//   5. Pokud chcete vytvořit nové soubory kódu, přejděte na Projekt > Přidat novou položku. Pokud chcete přidat do projektu existující soubory kódu, přejděte na Projekt > Přidat existující položku.
//   6. Pokud budete chtít v budoucnu znovu otevřít tento projekt, přejděte na Soubor > Otevřít > Projekt a vyberte příslušný soubor .sln.
