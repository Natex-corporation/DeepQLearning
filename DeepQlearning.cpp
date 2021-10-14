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
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <cmath>
//#include <algorithm>
#include "Structs.h"



using namespace std;

void shrink(vector<float>Vector);
//vector<MovingZScores> Zmoving(int LengthOfTrainingPeriod, int RandomFilePicker, int RandomStartingPoint, vector < MovingZScores>& Zscores);
long double shrinking(long double input);
int shrinkin(int input);

vector <float> Open;
vector <float> High;
vector <float> Low;
vector <float> Close;
vector <int> Volume;
vector <vector <long double>> Wights;

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




vector <vector <float>> bias;


int main() {
	//////////training length///////////////
	int SampleSize = 1000;												//Add here the number of tries for choosing the company/How many companies will be learned on
	int LengthOfTrainingPeriod = 365;									//How many days are wee leting the model train
	int NumberOfRepetitions = 20;										//How many times to look at the specified time period	
	
	//////////network parameters////////////
	int NumberOfHiddenLayers = 30;
	int NumberOfinputs = 50;
	int NumberOfOutputs = 3;
	double UpAndDownScaling = 2;
	long double LearningRate = 0;
	double NumberOfWeights = 0;
	vector <vector <float>> weights;

	//////////result evaluation/////////////
	//int PercentageGain = 100;											//How many percent gained in the time frame
	int StartingCapital = 25000;										//with how much money does the model start
	vector <string> FilePaths;
	string Path = "LearningData" /*"test"*/;							//Location of training data
	string TrainingControler;

	//vector <float> ZerothLayer;										//substuitude for vector of all layers

	const long double e = 2.71828182845904523536;
	

	cout << "Do you want to train the model with the current data?" << endl << "Yes/No" << endl;
	cin >> TrainingControler;

	if (TrainingControler == "Y") {
		//designed for linear fully connected networks 
		NumberOfWeights = ((NumberOfHiddenLayers - 1) * (NumberOfinputs * UpAndDownScaling * NumberOfinputs * UpAndDownScaling)) + (NumberOfinputs * NumberOfinputs * UpAndDownScaling) + (NumberOfinputs * UpAndDownScaling * NumberOfOutputs);

		cout << NumberOfWeights << " number of weigts" << "\n";

		for (int aqua = 0; aqua < NumberOfHiddenLayers + 1; aqua++) {																															//random weight generation
			srand((unsigned int)time(NULL));
			float a = 5.0;
			float OL;
			vector <float> LayerWeights;
			double Short = NumberOfOutputs * NumberOfinputs * UpAndDownScaling;
			double Long = NumberOfinputs * NumberOfinputs * UpAndDownScaling;
			double Longest = NumberOfinputs * UpAndDownScaling * NumberOfinputs * UpAndDownScaling;

			if (aqua == 0) {
				//cout << "am here8" << "\n";
				for (size_t i = 0; i < Long; i++) {
					OL = (float(rand()) / float((RAND_MAX)) * a);
					LayerWeights.push_back(OL);
				}
			}

			if (aqua == (NumberOfHiddenLayers + 1)) {
				//cout << "am here9" << "\n";
				for (size_t i = 0; i < Short; i++) {
					OL = (float(rand()) / float((RAND_MAX)) * a);
					LayerWeights.push_back(OL);
				}
			}

			if (0 < aqua < (NumberOfHiddenLayers + 1)) {
				//cout << "am here10" << "\n";
				for (size_t i = 0; i < Longest; i++) {
					//cout << i << "\n";
					OL = (float(rand()) / float((RAND_MAX)) * a);
					//cout << i << "haha" << "\n";
					//cout << aqua << "lasflaskdjf" << "\n";
					//cout << OL << typeid(OL).name() << " this is inwriten value\n";
					LayerWeights.push_back(OL);

					//cout << i << " lool" << "\n";
				}
				weights.push_back(LayerWeights);
				//cout << "am here 11" << "\n";
				LayerWeights.clear();
			}

		}

		cout << "generation of weights done" << "\n";

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

				int SeparateVolumePrice;
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

		for (int i = 0; i < All.size(); i++) {
			if (All[i].size() < (LengthOfTrainingPeriod + 30)) {
				All.erase(All.begin() + i);
				cout << All[i].size() << " this is too small" << i << "\n";
			}
			else {
				cout << "vectro is long enough" << "\n";
			}
		}

		for (int i = 0; i < All.size(); i++) {
			if (All[i].size() < (LengthOfTrainingPeriod + 30)) {
				All.erase(All.begin() + i);
				cout << All[i].size() << " this is too small" << i << "\n";
			}
			else {
				cout << "vectro is long enough" << "\n";
			}
		}

		for (int i = 0; i < All.size(); i++) {
			cout << i << "All[i].size: " << All[i].size() << "\n";
		}


		for (int i = 1; i <= SampleSize; i++) {																							// Random file picker
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> dist6(0, All.size() - 1);													 // distribution range of 0 - number of imported companies 
			int RandomFilePicker = dist6(rng);
			cout << "RandomFilePicker: " << RandomFilePicker << " " << i << "\n";

			for (int i = 1; i <= NumberOfRepetitions; i++) {																			// Random Range picker
				int Result = 0;
				int MaxLength = 0;
				int OffsetFromEnd = 0;
				MaxLength = All[RandomFilePicker].size();
				OffsetFromEnd = LengthOfTrainingPeriod + 1;
				Result = MaxLength - OffsetFromEnd;
				//cout << "Max Length: " << MaxLength << "\n" << "OffsetFromEnd: " << OffsetFromEnd << "\n" << "Result: " << Result << "\n";
				//cout << All[RandomFilePicker].size() - (LengthOfTrainingPeriod + 1) << "before random" << "\n";
				//cout << All[RandomFilePicker].size() << "size at index random file" << "\n";

				random_device dev;
				mt19937 rng(dev());
				uniform_int_distribution<mt19937::result_type> dist6(31, All[RandomFilePicker].size() - (LengthOfTrainingPeriod + 1));	 // distribution range of 
				int RandomStartingPoint = dist6(rng);
				//cout << "randomstarting point: " << RandomStartingPoint << endl;

				//Input Preparation
				vector <InputValues> InputValues;

				//things in an input
				//long double Capital = StartingCapital;																					//in money
				//long double Holdings = 0;																								//in stocks owned
				vector <float> InputNodeOpen;
				vector <float> InputNodeClose;
				vector <float> InputNodeHigh;
				vector <float> InputNodeLow;
				vector <int> InputNodeVolume;
				vector <double> InputNodeAverage;
				vector<MovingZScores> OpenMovingZscores;
				vector<MovingZScores> CloseMovingZscores;
				vector<MovingZScores> HighMovingZscores;
				vector<MovingZScores> LowMovingZscores;
				vector<MovingZScores> VolumeMovingZscores;
				vector<MovingZScores> AverageMovingZscores;
				vector<MovingAvarage> OpenMovingAvevrage;
				vector<MovingAvarage> CloseMovingAvevrage;
				vector<MovingAvarage> HighMovingAvevrage;
				vector<MovingAvarage> LowMovingAvevrage;
				vector<MovingAvarage> VolumeMovingAvevrage;
				vector<MovingAvarage> AverageMovingAvevrage;
				vector<RSI> RelativeStrengthIndex;
				vector<RSI> RelativeStrength;

				//int NumberOfWeights = 0;



				cout << "am here1" << "\n";
																																			//Normal values preparation 6nodes
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					All[RandomFilePicker][RandomStartingPoint + i].Open = shrinking(All[RandomFilePicker][RandomStartingPoint + i].Open);
					All[RandomFilePicker][RandomStartingPoint + i].Close = shrinking(All[RandomFilePicker][RandomStartingPoint + i].Close);
					All[RandomFilePicker][RandomStartingPoint + i].High = shrinking(All[RandomFilePicker][RandomStartingPoint + i].High);
					All[RandomFilePicker][RandomStartingPoint + i].Low = shrinking(All[RandomFilePicker][RandomStartingPoint + i].Low);
					//All[RandomFilePicker][RandomStartingPoint + i].Open = shrinking(All[RandomFilePicker][RandomStartingPoint + i].Open);
					All[RandomFilePicker][RandomStartingPoint + i].Volume = shrinking(All[RandomFilePicker][RandomStartingPoint + i].Volume);
					InputNodeOpen.push_back(All[RandomFilePicker][RandomStartingPoint + i].Open);
					//cout << "Open: " << All[RandomFilePicker][RandomStartingPoint + i].Open << "\n";
					InputNodeClose.push_back(All[RandomFilePicker][RandomStartingPoint + i].Close);
					//cout << "Close: " << All[RandomFilePicker][RandomStartingPoint + i].Close << "\n";
					InputNodeHigh.push_back(All[RandomFilePicker][RandomStartingPoint + i].High);
					//cout << "High: " << All[RandomFilePicker][RandomStartingPoint + i].High << "\n";
					InputNodeLow.push_back(All[RandomFilePicker][RandomStartingPoint + i].Low);
					//cout << "Low: " << All[RandomFilePicker][RandomStartingPoint + i].Low << "\n";
					InputNodeAverage.push_back((All[RandomFilePicker][RandomStartingPoint + i].Close + All[RandomFilePicker][RandomStartingPoint + i].High + All[RandomFilePicker][RandomStartingPoint + i].Low + All[RandomFilePicker][RandomStartingPoint + i].Open) / 4);
					//cout << "Average: " << (All[RandomFilePicker][RandomStartingPoint + i].Close + All[RandomFilePicker][RandomStartingPoint + i].High + All[RandomFilePicker][RandomStartingPoint + i].Low + All[RandomFilePicker][RandomStartingPoint + i].Open) / 4 << "\n";
					InputNodeVolume.push_back(All[RandomFilePicker][RandomStartingPoint + i].Volume);
					//cout << "One cycle done" << i << "\n";
				}

				cout << "am here2" << "\n";
																																			//Zscores generation 6nodes
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					//cout << i << " = i" << "\n";

					//if (RandomStartingPoint < 5) {
					for (int k = 0; k < 5; k++) {
						S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].Close;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].Close;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].Close;
						//cout << "too small" << "\n";
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].Close - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].Close - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].Close - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Close - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Close - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Close - M30ean) / St30deviation;

					ZScore5 = shrinking(ZScore5);
					ZScore10 = shrinking(ZScore10);
					ZScore30 = shrinking(ZScore30);

					CloseMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}
				
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					//cout << i << " = i" << "\n";

					//if (RandomStartingPoint < 5) {
					for (int k = 0; k < 5; k++) {
						S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].Open;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].Open;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].Open;
						//cout << "too small" << "\n";
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].Open - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].Open - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].Open - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M30ean) / St30deviation;

					ZScore5 = shrinking(ZScore5);
					ZScore10 = shrinking(ZScore10);
					ZScore30 = shrinking(ZScore30);

					OpenMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}
				
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					//cout << i << " = i" << "\n";

					//if (RandomStartingPoint < 5) {
					for (int k = 0; k < 5; k++) {
						S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].High;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].High;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].High;
						//cout << "too small" << "\n";
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].High - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].High - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].High - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].High - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].High - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].High - M30ean) / St30deviation;

					ZScore5 = shrinking(ZScore5);
					ZScore10 = shrinking(ZScore10);
					ZScore30 = shrinking(ZScore30);

					HighMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}
			
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					//cout << i << " = i" << "\n";

					//if (RandomStartingPoint < 5) {
					for (int k = 0; k < 5; k++) {
						S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].Low;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].Low;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].Low;
						//cout << "too small" << "\n";
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].Low - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].Low - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].Low - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Low - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Low - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Low - M30ean) / St30deviation;

					ZScore5 = shrinking(ZScore5);
					ZScore10 = shrinking(ZScore10);
					ZScore30 = shrinking(ZScore30);

					LowMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}
				
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					//cout << i << " = i" << "\n";

					//if (RandomStartingPoint < 5) {
					for (int k = 0; k < 5; k++) {
						S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].Volume;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].Volume;
						//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].Volume;
						//cout << "too small" << "\n";
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].Volume - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].Volume - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].Volume - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M30ean) / St30deviation;
					ZScore5 = shrinkin(ZScore5);
					ZScore10 = shrinkin(ZScore10);
					ZScore30 = shrinkin(ZScore30);

					VolumeMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}
			
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					//Sleep(100);
					double S5um = 0;
					double M5ean = 0;
					double S10um = 0;
					double M10ean = 0;
					double S30um = 0;
					double M30ean = 0;
					double St5deviation = 0;
					double St10deviation = 0;
					double St30deviation = 0;
					double Sum = 0;
					double sUm = 0;
					double suM = 0;
					double ZScore5 = 0;
					double ZScore30 = 0;
					double ZScore10 = 0;

					for (int k = 0; k < 5; k++) {
						S5um = S5um + ((All[RandomFilePicker][(RandomStartingPoint - 6) + i + k].Close + All[RandomFilePicker][(RandomStartingPoint - 6) + i + k].High + All[RandomFilePicker][(RandomStartingPoint - 6) + i + k].Low + All[RandomFilePicker][(RandomStartingPoint - 6) + i + k].Open) / 4);
					}
					for (int k = 0; k < 10; k++) {
						S10um = S10um + ((All[RandomFilePicker][(RandomStartingPoint - 11) + i + k].Close + All[RandomFilePicker][(RandomStartingPoint - 11) + i + k].High + All[RandomFilePicker][(RandomStartingPoint - 11) + i + k].Low + All[RandomFilePicker][(RandomStartingPoint - 11) + i + k].Open) / 4);
					}
					for (int k = 0; k < 30; k++) {
						S30um = S30um + ((All[RandomFilePicker][(RandomStartingPoint - 31) + i + k].Close + All[RandomFilePicker][(RandomStartingPoint - 31) + i + k].High + All[RandomFilePicker][(RandomStartingPoint - 31) + i + k].Low + All[RandomFilePicker][(RandomStartingPoint - 31) + i + k].Open) / 4);
					}

					M5ean = S5um / 5;
					M10ean = S10um / 10;
					M30ean = S30um / 30;

					for (int ik = 0; ik < 5; ik++) {
						Sum = Sum + pow(((All[RandomFilePicker][(RandomStartingPoint - 6) + i + ik].Close + All[RandomFilePicker][(RandomStartingPoint - 6) + i + ik].High + All[RandomFilePicker][(RandomStartingPoint - 6) + i + ik].Low + All[RandomFilePicker][(RandomStartingPoint - 6) + i + ik].Open) / 4) - M5ean, 2);
					}
					for (int ik = 0; ik < 10; ik++) {
						sUm = sUm + pow(((All[RandomFilePicker][(RandomStartingPoint - 11) + i + ik].Close + All[RandomFilePicker][(RandomStartingPoint - 11) + i + ik].High + All[RandomFilePicker][(RandomStartingPoint - 11) + i + ik].Low + All[RandomFilePicker][(RandomStartingPoint - 11) + i + ik].Open) / 4) - M10ean, 2);
					}
					for (int ik = 0; ik < 30; ik++) {
						suM = suM + pow(((All[RandomFilePicker][(RandomStartingPoint - 31) + i + ik].Close + All[RandomFilePicker][(RandomStartingPoint - 31) + i + ik].High + All[RandomFilePicker][(RandomStartingPoint - 31) + i + ik].Low + All[RandomFilePicker][(RandomStartingPoint - 31) + i + ik].Open) / 4) - M30ean, 2);
					}

					St5deviation = sqrt(Sum / 4);
					St10deviation = sqrt(sUm / 9);
					St30deviation = sqrt(suM / 29);

					ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M5ean) / St5deviation;
					ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M10ean) / St10deviation;
					ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Volume - M30ean) / St30deviation;

					ZScore5 = shrinking(ZScore5);
					ZScore10 = shrinking(ZScore10);
					ZScore30 = shrinking(ZScore30);

					AverageMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}

				cout << "am here3" << "\n";
																																			//Dayli moving avarage 6nodes
				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Open;
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Open;
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Open;
					}
					Sum5D = shrinking(Sum5D);
					Sum10D = shrinking(Sum10D);
					Sum30D = shrinking(Sum30D);

					OpenMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].High;
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].High;
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].High;
					}
					Sum5D = shrinking(Sum5D);
					Sum10D = shrinking(Sum10D);
					Sum30D = shrinking(Sum30D);

					HighMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Low;
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Low;
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Low;
					}
					Sum5D = shrinking(Sum5D);
					Sum10D = shrinking(Sum10D);
					Sum30D = shrinking(Sum30D);

					LowMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Volume;
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Volume;
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Volume;
					}
					Sum5D = shrinkin(Sum5D);
					Sum10D = shrinkin(Sum10D);
					Sum30D = shrinkin(Sum30D);

					VolumeMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Close;
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Close;
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Close;
					}
					Sum5D = shrinking(Sum5D);
					Sum10D = shrinking(Sum10D);
					Sum30D = shrinking(Sum30D);

					CloseMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				for (int day = 0; day < LengthOfTrainingPeriod; day++) {
					double Sum5D = 0;
					double Sum10D = 0;
					double Sum30D = 0;

					for (int i = 0; i < 5; i++) {
						Sum5D = Sum5D + ((All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Low + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Open) / 4);
					}
					for (int i = 0; i < 10; i++) {
						Sum10D = Sum10D + ((All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Low + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Open) / 4);
					}
					for (int i = 0; i < 30; i++) {
						Sum30D = Sum30D + ((All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Low + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Open) / 4);
					}
					Sum5D = shrinking(Sum5D);
					Sum10D = shrinking(Sum10D);
					Sum30D = shrinking(Sum30D);

					AverageMovingAvevrage.push_back({ Sum5D / 5, Sum10D / 10, Sum30D / 30 });
				}

				cout << "am here4" << "\n";
																																			//RSI (relative strength index) 1 node, RS (relative strengrh) node;
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					vector <long double> AverageValue;
					long double PositiveGrowth = 0;
					long double NegativeGrowth = 0;
					long double Relative5Strength = 0;
					long double Relative5StrengthIndex = 0;
					long double Relative10Strength = 0;
					long double Relative10StrengthIndex = 0;
					long double Relative30Strength = 0;
					long double Relative30StrengthIndex = 0;

					for (int day = 0; day < 5; day++) {
						AverageValue.push_back((All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Open + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 6) + i + day].Low) / 4);
					}
					for (int ik = 1; ik < 5; ik++) {
						long double PositiveNegative = AverageValue[ik - 1] - AverageValue[ik];
						if (PositiveNegative > 0) {
							//cout << PositiveNegative << "positive" << "\n";
							PositiveGrowth = PositiveGrowth + PositiveNegative;
						}
						if (PositiveNegative < 0) {
							//cout << PositiveNegative << "negative" << "\n";
							NegativeGrowth = NegativeGrowth + PositiveNegative;
						}
					}
					Relative5Strength = ((PositiveGrowth / 4) / (NegativeGrowth / 4));
					Relative5StrengthIndex = 100 - (100 / (1 + Relative5Strength));
					cout << Relative5Strength << Relative5StrengthIndex << "\n";
					Relative5Strength = shrinking(Relative5Strength);
					Relative5StrengthIndex = shrinking(Relative5StrengthIndex);

					AverageValue.clear();

					for (int day = 0; day < 10; day++) {
						AverageValue.push_back((All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Open + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 11) + i + day].Low) / 4);
					}
					for (int ik = 1; ik < 10; ik++) {
						long double PositiveNegative = AverageValue[ik - 1] - AverageValue[ik];
						if (PositiveNegative > 0) {
							//cout << PositiveNegative << "positive" << "\n";
							PositiveGrowth = PositiveGrowth + PositiveNegative;
						}
						if (PositiveNegative < 0) {
							//cout << PositiveNegative << "negative" << "\n";
							NegativeGrowth = NegativeGrowth + PositiveNegative;
						}
					}
					Relative10Strength = ((PositiveGrowth / 9) / (NegativeGrowth / 9));
					Relative10StrengthIndex = 100 - (100 / (1 + Relative5Strength));
					cout << Relative10Strength << Relative10StrengthIndex << "\n";
					Relative10Strength = shrinking(Relative10Strength);
					Relative10StrengthIndex = shrinking(Relative10StrengthIndex);

					AverageValue.clear();

					for (int day = 0; day < 30; day++) {
						AverageValue.push_back((All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Open + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Close + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].High + All[RandomFilePicker][(RandomStartingPoint - 31) + i + day].Low) / 4);
					}
					for (int ik = 1; ik < 30; ik++) {
						long double PositiveNegative = AverageValue[ik - 1] - AverageValue[ik];
						if (PositiveNegative > 0) {
							//cout << PositiveNegative << "positive" << "\n";
							PositiveGrowth = PositiveGrowth + PositiveNegative;
						}
						if (PositiveNegative < 0) {
							//cout << PositiveNegative << "negative" << "\n";
							NegativeGrowth = NegativeGrowth + PositiveNegative;
						}
					}
					Relative30Strength = ((PositiveGrowth / 29) / (NegativeGrowth / 29));
					Relative30StrengthIndex = 100 - (100 / (1 + Relative5Strength));
					cout << Relative30Strength << Relative30StrengthIndex << "\n";
					Relative30Strength = shrinking(Relative30Strength);
					Relative30StrengthIndex = shrinking(Relative30StrengthIndex);

					AverageValue.clear();
					cout << "am here4,1\n";

					RelativeStrength.push_back({ Relative5Strength, Relative10Strength, Relative30Strength });
					RelativeStrengthIndex.push_back({ Relative5StrengthIndex, Relative10StrengthIndex, Relative30StrengthIndex });
				}

				cout << "am here5" << "\n";
																																			// Input preperations
				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					InputValues.push_back({ InputNodeOpen[i], InputNodeClose[i], InputNodeHigh[i], InputNodeLow[i], InputNodeVolume[i], InputNodeAverage[i], OpenMovingZscores[i].ZScore5, OpenMovingZscores[i].ZScore10, OpenMovingZscores[i].ZScore30, CloseMovingZscores[i].ZScore5,
						CloseMovingZscores[i].ZScore10, CloseMovingZscores[i].ZScore30, HighMovingZscores[i].ZScore5, HighMovingZscores[i].ZScore10, HighMovingZscores[i].ZScore30, LowMovingZscores[i].ZScore5, LowMovingZscores[i].ZScore10, LowMovingZscores[i].ZScore30,
						VolumeMovingZscores[i].ZScore5, VolumeMovingZscores[i].ZScore10, VolumeMovingZscores[i].ZScore30, AverageMovingZscores[i].ZScore5, AverageMovingZscores[i].ZScore10, AverageMovingZscores[i].ZScore30, OpenMovingAvevrage[i].A5verage,
						OpenMovingAvevrage[i].A10verage, OpenMovingAvevrage[i].A30verage, CloseMovingAvevrage[i].A5verage, CloseMovingAvevrage[i].A10verage, CloseMovingAvevrage[i].A30verage, HighMovingAvevrage[i].A5verage, HighMovingAvevrage[i].A10verage,
						HighMovingAvevrage[i].A30verage, LowMovingAvevrage[i].A5verage, LowMovingAvevrage[i].A10verage, LowMovingAvevrage[i].A30verage, VolumeMovingAvevrage[i].A5verage, VolumeMovingAvevrage[i].A10verage, VolumeMovingAvevrage[i].A30verage,
						AverageMovingAvevrage[i].A5verage, AverageMovingAvevrage[i].A10verage, AverageMovingAvevrage[i].A30verage, RelativeStrengthIndex[i].R5SI, RelativeStrengthIndex[i].R10SI, RelativeStrengthIndex[i].R30SI, RelativeStrength[i].R5SI,
						RelativeStrength[i].R10SI/*, RelativeStrength[i].R30SI, Capital, Holdings*/
						});
				}

				cout << "am here6" << "\n";

				for (int i = 0; i < 1/*LengthOfTrainingPeriod*/; i++) {
					cout << InputNodeOpen[i] <<" " << InputNodeClose[i] <<" "<< InputNodeHigh[i] << " " << " " << InputNodeLow[i] << " " << InputNodeVolume[i] << " " << InputNodeAverage[i] << OpenMovingZscores[i].ZScore5 << " " << OpenMovingZscores[i].ZScore10 << " " << OpenMovingZscores[i].ZScore30 << " " << CloseMovingZscores[i].ZScore5 << " " <<
						CloseMovingZscores[i].ZScore10 << " " << CloseMovingZscores[i].ZScore30 << " " << HighMovingZscores[i].ZScore5 << " " << HighMovingZscores[i].ZScore10 << " " << HighMovingZscores[i].ZScore30 << " " << LowMovingZscores[i].ZScore5 << " " << LowMovingZscores[i].ZScore10 << " " << LowMovingZscores[i].ZScore30 << " " <<
						VolumeMovingZscores[i].ZScore5 << " " << VolumeMovingZscores[i].ZScore10 << " " << VolumeMovingZscores[i].ZScore30 << " " << AverageMovingZscores[i].ZScore5 << " " << AverageMovingZscores[i].ZScore10 << " " << AverageMovingZscores[i].ZScore30 << " " << OpenMovingAvevrage[i].A5verage << " " <<
						OpenMovingAvevrage[i].A10verage << " " << OpenMovingAvevrage[i].A30verage << " " << CloseMovingAvevrage[i].A5verage << " " << CloseMovingAvevrage[i].A10verage << " " << CloseMovingAvevrage[i].A30verage << " " << HighMovingAvevrage[i].A5verage << " " << HighMovingAvevrage[i].A10verage << " " <<
						HighMovingAvevrage[i].A30verage << " " << LowMovingAvevrage[i].A5verage << " " << LowMovingAvevrage[i].A10verage << " " << LowMovingAvevrage[i].A30verage << " " << VolumeMovingAvevrage[i].A5verage << VolumeMovingAvevrage[i].A10verage << " " << VolumeMovingAvevrage[i].A30verage << " " <<
						AverageMovingAvevrage[i].A5verage << " " << AverageMovingAvevrage[i].A10verage << " " << AverageMovingAvevrage[i].A30verage << " " << RelativeStrengthIndex[i].R5SI << " " << RelativeStrengthIndex[i].R10SI << " " << RelativeStrengthIndex[i].R30SI << " " << RelativeStrength[i].R5SI << " " << 
						RelativeStrength[i].R10SI << " " << RelativeStrength[i].R30SI; /*Capital, Holdings*/
						;
				}
																																				//environment creation
					long double Capital = StartingCapital;																						//in money
					long double Holdings = 0;																									//in stocks owned																												
					vector <vector  <long double>> Nodes;

					for (size_t day = 0; day < LengthOfTrainingPeriod; day++) {

						long double BuingPower = 0;

						for (int layer = 0; layer < (NumberOfHiddenLayers + 1); layer++) {
							//cout << layer << "layer\n";
							if (layer == 0) {
								vector <long double> FirstRow;
								for (int input = 0; input < (NumberOfinputs * 2); input++) {

									//cout << "am here7" << "\n";
									long double result = 0;
									/*NodesOfFirst[layer].push_back */
									result = ((weights[layer][input] * InputValues[day].InputNodeOpen) +
										(weights[layer][input + 48] * InputValues[day].InputNodeClose) +
										(weights[layer][input + (48 * 2)] * InputValues[day].InputNodeHigh) +
										(weights[layer][input + (48 * 3)] * InputValues[day].InputNodeLow) +
										(weights[layer][input + (48 * 4)] * InputValues[day].InputNodeVolume) +
										(weights[layer][input + (48 * 5)] * InputValues[day].InputNodeAverage) +
										(weights[layer][input + (48 * 6)] * InputValues[day].Open5MovingZscores) +
										(weights[layer][input + (48 * 7)] * InputValues[day].Open10MovingZscores) +
										(weights[layer][input + (48 * 8)] * InputValues[day].Open30MovingZscores) +
										(weights[layer][input + (48 * 9)] * InputValues[day].Close5MovingZscores) +
										(weights[layer][input + (48 * 10)] * InputValues[day].Close10MovingZscores) +
										(weights[layer][input + (48 * 11)] * InputValues[day].Close30MovingZscores) +
										(weights[layer][input + (48 * 12)] * InputValues[day].High5MovingZscores) +
										(weights[layer][input + (48 * 13)] * InputValues[day].High10MovingZscores) +
										(weights[layer][input + (48 * 14)] * InputValues[day].High30MovingZscores) +
										(weights[layer][input + (48 * 15)] * InputValues[day].Low5MovingZscores) +
										(weights[layer][input + (48 * 16)] * InputValues[day].Low10MovingZscores) +
										(weights[layer][input + (48 * 17)] * InputValues[day].Low30MovingZscores) +
										(weights[layer][input + (48 * 18)] * InputValues[day].Volume5MovingZscores) +
										(weights[layer][input + (48 * 19)] * InputValues[day].Volume10MovingZscores) +
										(weights[layer][input + (48 * 20)] * InputValues[day].Volume30MovingZscores) +
										(weights[layer][input + (48 * 21)] * InputValues[day].Average5MovingZscores) +
										(weights[layer][input + (48 * 22)] * InputValues[day].Average10MovingZscores) +
										(weights[layer][input + (48 * 23)] * InputValues[day].Average30MovingZscores) +
										(weights[layer][input + (48 * 24)] * InputValues[day].Open5MovingAvevrage) +
										(weights[layer][input + (48 * 25)] * InputValues[day].Open10MovingAvevrage) +
										(weights[layer][input + (48 * 26)] * InputValues[day].Open30MovingAvevrage) +
										(weights[layer][input + (48 * 27)] * InputValues[day].Close5MovingAvevrage) +
										(weights[layer][input + (48 * 28)] * InputValues[day].Close10MovingAvevrage) +
										(weights[layer][input + (48 * 29)] * InputValues[day].Close30MovingAvevrage) +
										(weights[layer][input + (48 * 30)] * InputValues[day].High5MovingAvevrage) +
										(weights[layer][input + (48 * 31)] * InputValues[day].High10MovingAvevrage) +
										(weights[layer][input + (48 * 32)] * InputValues[day].High30MovingAvevrage) +
										(weights[layer][input + (48 * 33)] * InputValues[day].Low5MovingAvevrage) +
										(weights[layer][input + (48 * 34)] * InputValues[day].Low10MovingAvevrage) +
										(weights[layer][input + (48 * 35)] * InputValues[day].Low30MovingAvevrage) +
										(weights[layer][input + (48 * 36)] * InputValues[day].Volume5MovingAvevrage) +
										(weights[layer][input + (48 * 37)] * InputValues[day].Volume10MovingAvevrage) +
										(weights[layer][input + (48 * 38)] * InputValues[day].Volume30MovingAvevrage) +
										(weights[layer][input + (48 * 39)] * InputValues[day].Average5MovingAvevrage) +
										(weights[layer][input + (48 * 40)] * InputValues[day].Average10MovingAvevrage) +
										(weights[layer][input + (48 * 41)] * InputValues[day].Average30MovingAvevrage) +
										(weights[layer][input + (48 * 42)] * InputValues[day].Relative5StrengthIndex) +
										(weights[layer][input + (48 * 43)] * InputValues[day].Relative10StrengthIndex) +
										(weights[layer][input + (48 * 44)] * InputValues[day].Relative30StrengthIndex) +
										(weights[layer][input + (48 * 45)] * InputValues[day].Relative5Strength) +
										(weights[layer][input + (48 * 46)] * InputValues[day].Relative10Strength) +
										(weights[layer][input + (48 * 47)] * InputValues[day].Relative30Strength) +
										(weights[layer][input + (48 * 48)] * Capital) + (weights[layer][input + (48 * 49)] * Holdings));
									//(Nodes[0]).push_back(1);

									result = 1 / (1 + pow(e, -result));

									//cout << result << "\n";
									FirstRow.push_back(result);
									//(Nodes[layer]).push_back(result);
									//cout << "am here8\n";
								}
								Nodes.push_back(FirstRow);
								//cout << "am here9\n";
							}

							if (layer > 0) {
								//cout << "am here10\n";
								vector <long double> MidleRows;
								for (int out = 0; out < (NumberOfinputs * 2); out++) {
									long double result = 0;
									for (int input = 0; input < (NumberOfinputs * 2); input++) {
										result = result + (Nodes[layer - 1][input] * weights[layer][out * input]);
									}
									//cout << "am here11\n";
									result = 1 / (1 + pow(e, -result));

									MidleRows.push_back(result);
								}
								//cout << "am here12\n";
								Nodes.push_back(MidleRows);
								//cout << layer << "\n";
							}
							cout << "am here13\n";
							if (layer == (NumberOfHiddenLayers + 1)) {
								vector <long double> LastRows;
								for (int output = 0; output < NumberOfOutputs; output++) {
									long double result = 0;
									for (int input = 0; input < (NumberOfinputs * 2); input++) {
										result = result + (Nodes[layer - 1][input] * weights[layer][input * output]);
									}
									result = 1 / (1 + pow(e, -result));
									LastRows.push_back(result);
								}
								Nodes.push_back(LastRows);
							}

						}
						cout << "am here14\n";

						//0 = buy
						//1 = sell
						//2 = hold
						cout << Nodes.size() << "\n";
						cout << Nodes[NumberOfHiddenLayers][0] << " " << Nodes[NumberOfHiddenLayers][1] << " " << Nodes[NumberOfHiddenLayers][2] << "\n";

						if (Nodes[NumberOfHiddenLayers][0] >= Nodes[NumberOfHiddenLayers][1] && Nodes[NumberOfHiddenLayers][0] >= Nodes[NumberOfHiddenLayers][2]) {
							cout << "am here15\n";
							Holdings = (Capital * Nodes[NumberOfHiddenLayers][0]) / All[RandomFilePicker][RandomStartingPoint + day + 1].Open;
						}
						if (Nodes[NumberOfHiddenLayers][1] >= Nodes[NumberOfHiddenLayers][2] && Nodes[NumberOfHiddenLayers][1] >= Nodes[NumberOfHiddenLayers][0]) {
							cout << "am here16\n";
							Capital = (Holdings * Nodes[NumberOfHiddenLayers][1]) * All[RandomFilePicker][RandomStartingPoint + day + 1].Open;
						}

						/*if (Nodes[NumberOfHiddenLayers + 1][2] >= Nodes[NumberOfHiddenLayers + 1][0] && Nodes[NumberOfHiddenLayers + 1][2] >= Nodes[NumberOfHiddenLayers + 1][1]) {

						}*/

						/*long double a = 0;
						long double b = 0;

						while (InputNodeClose[i] > 1) {
							InputNodeClose[i] = InputNodeClose[i] / 10;
						}
						while (InputNodeOpen[i] > 1) {
							InputNodeOpen[i] = InputNodeOpen[i] / 10;
						}

						a = InputNodeClose[i] * ZerothLayer[i] + InputNodeOpen[i] * ZerothLayer[i + 1];
						b = InputNodeClose[i] * ZerothLayer[i + 2] + InputNodeOpen[i] * ZerothLayer[i + 3];

						a = 1 / (1 + pow(e, -a));			//must use Relu instead
						b = 1 / (1 + pow(e, -b));
						//cout << "this is a: " << a << "\n" << "this is b: " << b << "\n";*/
					}
					//evaluation of the run

				}
			}
		}


	
	else {
			cout << "Training data was skiped" << endl;
	
	}
}

/////////////////////////////functions//////////////////////////
long double shrinking(long double input) {
	while (input >= 10 && input <= -10) {
		input = input / 10;
	}
	return input;
}

int shrinkin(int input) {
	while (input >= 10 && input <= -10) {
		input = input / 10;
	}
	return input;
}

/*void CostFunction() {

}

vector<MovingZScores> Zmoving(int LengthOfTrainingPeriod, int RandomFilePicker, int RandomStartingPoint, vector <MovingZScores> Zscores) {
	for (int i = 0; i < LengthOfTrainingPeriod; i++) {
		//Sleep(100);
		double S5um = 0;
		double M5ean = 0;
		double S10um = 0;
		double M10ean = 0;
		double S30um = 0;
		double M30ean = 0;
		double St5deviation = 0;
		double St10deviation = 0;
		double St30deviation = 0;
		double Sum = 0;
		double sUm = 0;
		double suM = 0;
		double ZScore5 = 0;
		double ZScore30 = 0;
		double ZScore10 = 0;

		//cout << i << " = i" << "\n";

		//if (RandomStartingPoint < 5) {
		for (int k = 0; k < 5; k++) {
			S5um = S5um + All[RandomFilePicker][(RandomStartingPoint - 6) + k + i].Open;
			//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
		}
		for (int k = 0; k < 10; k++) {
			S10um = S10um + All[RandomFilePicker][(RandomStartingPoint - 11) + k + i].Open;
			//cout << All[RandomFilePicker][i].Open << "too small" << "\n";
		}
		for (int k = 0; k < 30; k++) {
			S30um = S30um + All[RandomFilePicker][(RandomStartingPoint - 31) + k + i].Open;
			//cout << "too small" << "\n";
		}

		M5ean = S5um / 5;
		M10ean = S10um / 10;
		M30ean = S30um / 30;

		for (int ik = 0; ik < 5; ik++) {
			Sum = Sum + pow(All[RandomFilePicker][(RandomStartingPoint - 6) + ik + i].Open - M5ean, 2);
		}
		for (int ik = 0; ik < 10; ik++) {
			sUm = sUm + pow(All[RandomFilePicker][(RandomStartingPoint - 11) + ik + i].Open - M10ean, 2);
		}
		for (int ik = 0; ik < 30; ik++) {
			suM = suM + pow(All[RandomFilePicker][(RandomStartingPoint - 31) + ik + i].Open - M30ean, 2);
		}

		St5deviation = sqrt(Sum / 4);
		St10deviation = sqrt(sUm / 9);
		St30deviation = sqrt(suM / 29);

		ZScore5 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M5ean) / St5deviation;
		ZScore10 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M10ean) / St10deviation;
		ZScore30 = (All[RandomFilePicker][RandomStartingPoint + i].Open - M30ean) / St30deviation;

		Zscores.push_back({ ZScore5, ZScore10, ZScore30 });
		return Zscores;
	}
}
*/
	

