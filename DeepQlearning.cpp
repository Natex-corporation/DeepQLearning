// DeepQlearning.cpp : Program was written by Thomas Nazler : Started on 5. 8. 2021.

#include <iostream>
#include <vector>
#include <random>
#include <filesystem>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <fstream>
#include <numeric>


using namespace std;

void shrink(vector<float>Vector);

struct SeparateDays{
	//string Date;
	float Open;
	float High;
	float Low;
	float Close;
	int Volume;
	//string Name;
};
vector <float> Open;
vector <float> High;
vector <float> Low;
vector <float> Close;
vector <int> Volume;

vector<SeparateDays> comp;
vector<vector<SeparateDays>> All;





class CSVRow {

public:
	string_view operator[](size_t index) const
	{
		return string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
	}
	size_t size() const
	{
		return m_data.size() - 1;
	}
	void readNextRow(istream& str)
	{
		getline(str, m_line);

		m_data.clear();
		m_data.emplace_back(-1);
		string::size_type pos = 0;
		while ((pos = m_line.find(',', pos)) != string::npos)
		{
			m_data.emplace_back(pos);
			++pos;
		}
		// This checks for a trailing comma with no data after it.
		pos = m_line.size();
		m_data.emplace_back(pos);
	}

private:
	string         m_line;
	vector<int>    m_data;
};

istream& operator>>(istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}


int main() {
	int SampleSize = 1;													//Add here the number of tries for choosing the company 
	int LengthOfTrainingPeriod = 365;									//How many days are wee leting the model train
	int NumberOfRepetitions = 5;										//How many times to look at the specified time period
	

	int PercentageGain = 100;											//How many percent gained in the time frame
	int StartingCapital = 100000;										//with how much money does the model start
	vector <string> FilePaths;
	string Path = "LearningData" /*"test"*/;							//Location of training data
	string TrainingControler;

	vector <float> ZerothLayer;											//substuitude for vector of all layers

	const long double e = 2.71828182845904523536;
	

	cout << "Do you want to train the model with the current data?" << endl << "Yes/No" << endl;
	cin >> TrainingControler;

	if (TrainingControler == "Y") {
		for (const auto& file : filesystem::directory_iterator(Path)) {	//Creating vector including file paths to all trained companies
			string CompanyName;
			stringstream ss;
			ss << file.path();
			ss >> CompanyName;
			CompanyName.erase(0, 1);
			CompanyName.erase(CompanyName.size() - 1, 1);
			FilePaths.push_back(CompanyName);

		}


		for (int i = 0; i < FilePaths.size(); i++) {
			ifstream file(FilePaths[i]);

			if (!file.is_open()) {
				cout << "ERROR: File Not Opend" << endl;
				exit(EXIT_FAILURE);
			}

			CSVRow row;
			while (file >> row) {
				float SeparateOpenPrice;
				// cout << "4th Element(" << row[3] << ")\n";
				stringstream ss;
				ss << row[1];
				ss >> SeparateOpenPrice;
				Open.push_back(SeparateOpenPrice);

				float SeparateHighPrice;
				stringstream aa;
				aa << row[2];
				aa >> SeparateHighPrice;
				High.push_back(SeparateHighPrice);

				float SeparateLowPrice;
				stringstream bb;
				bb << row[3];
				bb >> SeparateLowPrice;
				Low.push_back(SeparateLowPrice);

				float SeparateClosePrice;
				stringstream cc;
				cc << row[4];
				cc >> SeparateClosePrice;
				Close.push_back(SeparateClosePrice);

				float SeparateVolumePrice;
				stringstream dd;
				dd << row[5];
				dd >> SeparateVolumePrice;
				Volume.push_back(SeparateVolumePrice);

			}

			for (size_t i = 0; i < Open.size(); i++) {
				comp.push_back(SeparateDays{ Open[i], High[i], Low[i], Close[i], Volume[i] });
			}

			cout << i << endl;
			Open.clear();
			High.clear();
			Low.clear();
			Close.clear();
			Volume.clear();

			All.push_back(comp);
			comp.clear();
		}

		/*for (int i = 0; i < All.size(); i++) {																	// Read out of all parameters of all companies
			for (int j = 0; j < All[i].size(); j++) {
				cout << All[i][j].Close << " " << All[i][j].Open << endl;

			}
			cout << i << endl;
		}*/	



		for (int i = 1; i <= SampleSize; i++) {																							// Random file picker
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> dist6(0, All.size() - 1);													 // distribution range of 0 - number of imported companies 
			int RandomFilePicker = dist6(rng);
			cout << "RandomFilePicker: " << RandomFilePicker << endl;

			for (int i = 1; i <= NumberOfRepetitions; i++) {																			// Random Range picker
				vector <float> InputNodeOpen;
				vector <float> InputNodeClose;
			
				random_device dev;
				mt19937 rng(dev());
				uniform_int_distribution<mt19937::result_type> dist6(0, All[RandomFilePicker].size() - (LengthOfTrainingPeriod - 1));	 // distribution range of 
				int random = dist6(rng);
				cout << "random: " << random << endl;

				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					InputNodeOpen.push_back(All[RandomFilePicker][random + i].Open);
					InputNodeClose.push_back(All[RandomFilePicker][random + i].Close);
				}

				cout << InputNodeClose.size() << "\n";

				
				srand((unsigned int)time(NULL));
				float a = 5.0;
				for (size_t i = 0; i < (InputNodeClose.size() * 4); i++) {
					float OL;
					OL = (float(rand()) / float((RAND_MAX)) * a);	
					ZerothLayer.push_back(OL);
				}

				//shrink(InputNodeOpen);
				//shrink(InputNodeClose);
				

				for (size_t i = 0; i < LengthOfTrainingPeriod; i++) {
					long double a = 0;
					long double b = 0;
					
					while (InputNodeClose[i] > 1) {
						InputNodeClose[i] = InputNodeClose[i] / 10;
					}
					while (InputNodeOpen[i] > 1) {
						InputNodeOpen[i] = InputNodeOpen[i] / 10;
					}
					
					a = InputNodeClose[i] * ZerothLayer[i] + InputNodeOpen[i] * ZerothLayer[i + 1];
					b = InputNodeClose[i] * ZerothLayer[i + 2] + InputNodeOpen[i] * ZerothLayer[i + 3];
					
					a = 1 / (1 + pow(e, -a));
					b = 1 / (1 + pow(e, -b));
					cout << "this is a: " << a << "\n" << "this is b: " << b << "\n";
				}




				float ZLongScore; int n = LengthOfTrainingPeriod;																		// Z score calculations
				float XLine = 0;
				//cout << "lool";
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//cout << All[0][0 + i].Open << endl;
					XLine = XLine + All[0][0 + i].Open;
				}
				//cout << "XLine" << XLine << endl;
				//cout << All[0][random].Open;
				//cout << dist6(rng) << endl;
			}
		}
	}
	


	else {
			cout << "Training data was skiped" << endl;
	
	}
}

/////////////////////////////functions//////////////////////////
void shrink(vector<float>Vector) {
	for (size_t i = 0; i < Vector.size(); i++) {
		while (Vector[i] > 1) {
			Vector[i] = Vector[i] / 10;
			//cout << Vector[i] << "\n";
		}
	}
	
}
	

