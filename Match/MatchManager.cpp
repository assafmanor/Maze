#include "MatchManager.h"


MatchManager::~MatchManager() {

}


MatchManager::MatchManager(const MatchManager&) {

}

void MatchManager::startMatch() {
	//parse command line args, in case of error we do not proceed
	if (processCommandLineInput()) return;
	//temporary hard coded 2 algo
	ListOfAlgorithms.push_back(std::make_unique<_311246755_a>());
	algorithmsNames.push_back("_311246755_A");
	ListOfAlgorithms.push_back(std::make_unique<_311246755_a>());
	algorithmsNames.push_back("_311246755_B");
	extractFullMazesName();
	extractMazesName();
	numOfMazes = mazesFullNames.size();
	numOfAlgorithms = ListOfAlgorithms.size();
	//run all Algorithms on all mazes and store the scores
	for (int i = 0; i < numOfAlgorithms; ++i) {
		for (int j = 0; j < numOfMazes; ++j) {
			//GameManager game(mazesFullNames.at(j), mazesNames.at(j)+ algorithmsNames.at(i));	
			//scores.at(i).at(j) = game.startGame();
		}

	}
	//print scores...

}

// listing the files in a directory and add them to our vector of names
void MatchManager::extractFullMazesName() {
	for (const auto & entry : std::filesystem::directory_iterator(mazesPath))
		mazesFullNames.push_back(entry.path().string());
}

void MatchManager::extractMazesName() {
	std::string delimiter = "/";
	for (int j = 0; j < numOfMazes; ++j) {
		std::string s = mazesFullNames.at(j);
		size_t pos = 0;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			s.erase(0, pos + delimiter.length());
		}
		mazesNames.push_back(s);
	}
}

int MatchManager::processCommandLineInput() {
	if (extractPathFromString(mazesPath, "-maze_path")) {
		printError(Errors::Maze_Path, mazesPath, "-maze_path <path>");
		return FAILURE;
	}
	if (extractPathFromString(algorithmsPath, "-algorithm_path")) {
		printError(Errors::algorithm_path, algorithmsPath, "-algorithm_path <algorithm path>");
		return FAILURE;
	}
	if (extractPathFromString(outputPath, "-output")) {
		printError(Errors::output_path, outputPath, "-output <output path>");
		return FAILURE;
	}
	return SUCCESS;
}

int MatchManager::extractPathFromString(std::string &str, std::string tag) {
	if (str.compare("/.") == 0)	return SUCCESS;
	std::vector<std::string> result;
	std::istringstream iss(str);
	//break the strings to tokens
	for (std::string s; iss >> s; )
		result.push_back(s);
	if (result.size() != 2 || result.at(0).compare(tag) != 0) return FAILURE;
	str = result.at(1);
	return SUCCESS;
}

void MatchManager::printError(Errors error, std::string input, std::string validFormat) {

	switch (error) {
	case Errors::Maze_Path:
		std::cout << "Wrong command line argument for maze" << std::endl;
		std::cout << "provided: " << input << std::endl;
		std::cout << "expected: " << validFormat << std::endl;
		break;
	case Errors::algorithm_path:
		std::cout << "Wrong command line argument for algorithms" << std::endl;
		std::cout << "provided: " << input << std::endl;
		std::cout << "expected: " << validFormat << std::endl;
		break;
	case Errors::output_path:
		std::cout << "Wrong command line argument for output" << std::endl;
		std::cout << "provided: " << input << std::endl;
		std::cout << "expected: " << validFormat << std::endl;
		break;
	default:
		break;
	}

}

