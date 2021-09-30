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



using namespace std;

void shrink(vector<float>Vector);
//vector<MovingZScores> Zmoving(int LengthOfTrainingPeriod, int RandomFilePicker, int RandomStartingPoint, vector < MovingZScores>& Zscores);

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
	int SampleSize = 1000;													//Add here the number of tries for choosing the company 
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

		for (int i = 0; i < All.size(); i++) {
			if (All[i].size() < (LengthOfTrainingPeriod + 30)) {
				All.erase(All.begin() + i);
				cout << All[i].size() << " this is too small" << i <<"\n";
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
			cout << i <<"All[i].size: " << All[i].size() << "\n";
		}




		for (int i = 1; i <= SampleSize; i++) {																							// Random file picker
			random_device dev;
			mt19937 rng(dev());
			uniform_int_distribution<mt19937::result_type> dist6(0, All.size() - 1);													 // distribution range of 0 - number of imported companies 
			int RandomFilePicker = dist6(rng);
			cout << "RandomFilePicker: " << RandomFilePicker << endl;

			for (int i = 1; i <= NumberOfRepetitions; i++) {																			// Random Range picker
				int Result = 0;
				int MaxLength = 0;
				int OffsetFromEnd = 0;
				MaxLength = All[RandomFilePicker].size();
				OffsetFromEnd = LengthOfTrainingPeriod + 1;
				Result = MaxLength - OffsetFromEnd;
				cout << "Max Length: " << MaxLength << "\n" << "OffsetFromEnd: " << OffsetFromEnd << "\n" << "Result: " << Result << "\n";
				cout << All[RandomFilePicker].size() - (LengthOfTrainingPeriod + 1) << "before random" << "\n";
				cout << All[RandomFilePicker].size() << "size at index random file" << "\n";
				
				random_device dev;
				mt19937 rng(dev());
				uniform_int_distribution<mt19937::result_type> dist6(31, All[RandomFilePicker].size() - (LengthOfTrainingPeriod + 1));	 // distribution range of 
				int RandomStartingPoint = dist6(rng);
				cout << "randomstarting point: " << RandomStartingPoint << endl;

				//Input Preparation
				vector <float> InputNodeOpen;
				vector <float> InputNodeClose;
				vector <float> InputNodeHigh;
				vector <float> InputNodeLow;
				vector <int> InputNodeVolume;
				vector <long double> ZScore;
				struct MovingZScores {
						long double ZScore5;
						long double ZScore10;
						long double ZScore30;
						//long double CloseZScore;
						//long double VolumeZScore;
				};
				vector<MovingZScores> OpenMovingZscores;
				vector<MovingZScores> CloseMovingZscores;
				vector<MovingZScores> HighMovingZscores;
				vector<MovingZScores> LowMovingZscores;
				vector<MovingZScores> VolumeMovingZscores;

				

				cout << "am here" << "\n";

				for (int i = 0; i < LengthOfTrainingPeriod; i++) {
					InputNodeOpen.push_back(All[RandomFilePicker][RandomStartingPoint + i].Open);
					//cout << "Open: " << All[RandomFilePicker][RandomStartingPoint + i].Open << "\n";
					InputNodeClose.push_back(All[RandomFilePicker][RandomStartingPoint + i].Close);
					//cout << "Close: " << All[RandomFilePicker][RandomStartingPoint + i].Close << "\n";
					InputNodeHigh.push_back(All[RandomFilePicker][RandomStartingPoint + i].High);
					//cout << "High: " << All[RandomFilePicker][RandomStartingPoint + i].High << "\n";
					InputNodeLow.push_back(All[RandomFilePicker][RandomStartingPoint + i].Low);
					//cout << "Low: " << All[RandomFilePicker][RandomStartingPoint + i].Low << "\n";
					InputNodeVolume.push_back(All[RandomFilePicker][RandomStartingPoint + i].Volume);
					//cout << "One cycle done" << i << "\n";
				}

				cout << "here am i" << "\n";

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
					for (int k = 0; k < 5; k++)	{
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

					LowMovingZscores.push_back({ ZScore5, ZScore10, ZScore30});
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

					VolumeMovingZscores.push_back({ ZScore5, ZScore10, ZScore30 });
				}

				/*for (int y = 0; y < OpenMovingZscores.size(); y++) {
					cout << OpenMovingZscores[y].ZScore5 << " 5day; " << OpenMovingZscores[y].ZScore10 << " 10day; " << OpenMovingZscores[y].ZScore30 << " 30day" << "\n";
					cout << VolumeMovingZscores[y].ZScore5 << " 5day; " << VolumeMovingZscores[y].ZScore10 << " 10day; " << VolumeMovingZscores[y].ZScore30 << " 30day" << "\n";
					cout << LowMovingZscores[y].ZScore5 << " 5day; " << LowMovingZscores[y].ZScore10 << " 10day; " << LowMovingZscores[y].ZScore30 << " 30day" << "\n";
					cout << HighMovingZscores[y].ZScore5 << " 5day; " << HighMovingZscores[y].ZScore10 << " 10day; " << HighMovingZscores[y].ZScore30 << " 30day" << "\n";
					cout << CloseMovingZscores[y].ZScore5 << " 5day; " << CloseMovingZscores[y].ZScore10 << " 10day; " << CloseMovingZscores[y].ZScore30 << " 30day" << "\n";
				}*/

				


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
					
					a = 1 / (1 + pow(e, -a));			//must use Relu instead
					b = 1 / (1 + pow(e, -b));
					//cout << "this is a: " << a << "\n" << "this is b: " << b << "\n";
				}
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

void CostFunction() {

}
	

