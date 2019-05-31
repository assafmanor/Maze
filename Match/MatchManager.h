#pragma once

#include <vector>
#include <list>
#include <functional>
#include <memory>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <cassert>
#include <linux/limits.h>
#include "AbstractAlgorithm.h"
#include "GameManager.h"
#include "AlgorithmRegistrar.h"
#include <thread>
#include <mutex>
#include <iterator>


#define SUCCESS 0
#define FAILURE 1

namespace fs = std::filesystem;

//possible errors
enum class Errors {
	maze_path, output_path, algorithm_path, algorithm_cannot_be_loaded, algorithm_not_registered
};



class MatchManager {
	std::vector<std::string> mazesFullNames;
	std::vector<std::string> mazesNames;

	std::string mazesPath, algorithmsPath, outputPath;
	std::vector<std::unique_ptr<AbstractAlgorithm>> ListOfAlgorithms;
	std::vector<std::string> eachAlgorithmPaths;
	std::list<std::string> algorithmNamesList;
	bool providedOutputArg;
	int numOfMazes;
	size_t numOfAlgorithms;
	int numOfThreads;
	int currAlgo, currMaze; //init
	std::mutex mutex;
	bool finished;
	

	/*
	 * prints the error specified in Errors to the screen
	 */
	void printError(Errors error, std::string input, std::string validFormat);

	/*
	 * extracts the mazes paths and stores in our vector
	 */
	void extractFullMazesName();

	/*
	 * extracts the mazes names and stores in our vector
	 */
	void extractMazesName();

	/*
	 * extracts the names and paths of the algorithms and stores in our vectors
	 */
	void extractAlgorithmNames(std::list<std::string>&);

	/*
	 * prints scores summary to screen
	 */
	void printScoresTable(std::vector<std::vector<int>> scores);

public:

	MatchManager(std::string _mazesPath, std::string _algorithmsPath, std::string _outputPath, int _numOfThreads, bool _providedOutputArg) : mazesPath(_mazesPath), algorithmsPath(_algorithmsPath), outputPath(_outputPath), providedOutputArg(_providedOutputArg), numOfThreads(_numOfThreads), currAlgo(-1), currMaze(0), finished(false){};
	MatchManager(MatchManager&&) = default; //move constructor

	/*
	 * starts the Match by processing the mazes and the algorithms
	 * loads each algorithm
	 * run all Algorithms on all mazes and store the scores
	 * prints the scores Table to screen
	 */
	int startMatch();

	/*
	 * process the command line args
	 * makes sure that each path accompanied with a tag
	 * stores in result[0] the maze_path, in result[1] the algorithm_path, 
	 * in result[2] the output_path
	 * storen false in providedOutputArg In case -output argument is missing, true otherwise
	 */
	static int processCommandLineArgs(int numOfArgs, char** argv, std::vector<std::string>& result, int &numOfThreads, bool& providedOutputArg);

	/*
	 * runs the games, meaning the Algorithms on the mazes
	 * as long as their are games to run, calls safelyIncIndexes to get the next algo and maze
	 * stores the results in the proper entry of the recived scores table
	 */
	void runGames(AlgorithmRegistrar &registrar, std::vector<std::vector<int>> &scores);

	/*
  	 * stores in the given argument i the next maze to run 
	 * stores in the given argument j the next algorithm to run
	 * uses lock_guard to insure that each thread calling this func gets unique pair of maze and algorithm
	 */
	void safelyIncIndexes(int &i, int &j);


};
