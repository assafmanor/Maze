#pragma once

#include <vector>
#include <map>
#include <list>
#include <functional>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream> 
#include <iomanip>
#include "AbstractAlgorithm.h"
#include "GameManager.h"
#include "AlgorithmRegistrar.h"


//#include "_311246755_a.h" // should be deleted when  dynamic registration implemented

#define SUCCESS 0
#define FAILURE 1

//possible errors
enum class Errors {
	maze_path, output_path, algorithm_path, algorithm_cannot_be_loaded, algorithm_not_registered
};



class MatchManager {
	std::vector<std::string> mazesFullNames;
	std::vector<std::string> mazesNames;

	std::string mazesPath, algorithmsPath, outputPath;
	//std::map<std::string, std::function<std::unique_ptr<AbstractAlgorithm>()>> Factory;
	std::vector<std::unique_ptr<AbstractAlgorithm>> ListOfAlgorithms;
	std::vector<std::string> algorithmsNames;
	//holds the scores of each algo, scoers[i][j] is the score of algo number i on maze j
	int numOfMazes, numOfAlgorithms;

	int processCommandLineInput();
	int extractPathFromString(std::string &str, std::string tag);
	void printError(Errors error, std::string input, std::string validFormat);
	void extractFullMazesName();
	void extractMazesName();
	void extractAlgorithmNames(std::vector<std::string>&);
	void printScoresTable(std::vector<std::vector<int>> scores);

public:
	MatchManager(const char *_mazesPath = "./", const char *_algorithmsPath = "./", const char *_outputPath = "./") : mazesPath(std::string(_mazesPath)), algorithmsPath(std::string(_algorithmsPath)), outputPath(std::string(_outputPath)) {}; //constructor
	//MatchManager(); //constructor

	//virtual ~MatchManager(); //destructor
	//MatchManager(const MatchManager&); //copy constructor
	MatchManager(MatchManager&&) = default; //move constructor
	int startMatch();




};
