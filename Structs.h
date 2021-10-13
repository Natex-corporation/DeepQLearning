#pragma once
#include <iostream>

struct MovingZScores {
	long double ZScore5;
	long double ZScore10;
	long double ZScore30;
};

struct MovingAvarage {
	long double A5verage;
	long double A10verage;
	long double A30verage;
};

struct RSI {
	long double R5SI;
	long double R10SI;
	long double R30SI;
};

struct SeparateDays {
	//string Date;
	float Open;
	float High;
	float Low;
	float Close;
	int Volume;
	//string Name;
};

/*struct InputValues {
	vector <float> InputNodeOpen;
	vector <float> InputNodeClose;
	vector <float> InputNodeHigh;
	vector <float> InputNodeLow;
	vector <int> InputNodeVolume;
	vector <double> InputNodeAverage;
	//vector <long double> ZScore;
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
};*/

struct InputValues {
	float InputNodeOpen;
	float InputNodeClose;
	float InputNodeHigh;
	float InputNodeLow;
	int InputNodeVolume;
	double InputNodeAverage;
	long double Open5MovingZscores;
	long double Open10MovingZscores;
	long double Open30MovingZscores;
	long double Close5MovingZscores;
	long double Close10MovingZscores;
	long double Close30MovingZscores;
	long double High5MovingZscores;
	long double High10MovingZscores;
	long double High30MovingZscores;
	long double Low5MovingZscores;
	long double Low10MovingZscores;
	long double Low30MovingZscores;
	long double Volume5MovingZscores;
	long double Volume10MovingZscores;
	long double Volume30MovingZscores;
	long double Average5MovingZscores;
	long double Average10MovingZscores;
	long double Average30MovingZscores;
	long double Open5MovingAvevrage;
	long double Open10MovingAvevrage;
	long double Open30MovingAvevrage;
	long double Close5MovingAvevrage;
	long double Close10MovingAvevrage;
	long double Close30MovingAvevrage;
	long double High5MovingAvevrage;
	long double High10MovingAvevrage;
	long double High30MovingAvevrage;
	long double Low5MovingAvevrage;
	long double Low10MovingAvevrage; 
	long double Low30MovingAvevrage;
	long double Volume5MovingAvevrage;
	long double Volume10MovingAvevrage;
	long double Volume30MovingAvevrage;
	long double Average5MovingAvevrage;
	long double Average10MovingAvevrage;
	long double Average30MovingAvevrage;
	long double Relative5StrengthIndex;
	long double Relative10StrengthIndex;
	long double Relative30StrengthIndex;
	long double Relative5Strength;
	long double Relative10Strength;
	long double Relative30Strength;
	//long double Capital;
	//long double Holdings;//vector <long double> ZScore;
};
