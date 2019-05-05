#include "MatchManager.h"

/*
MatchManager::~MatchManager() {

}
*/
/*
MatchManager::MatchManager(const MatchManager&) {

}
*/

int MatchManager::startMatch() {
	//parse command line args, in case of error we do not proceed
	if (processCommandLineInput()) return FAILURE;
	/**
	//temporary hard coded 2 algo
	ListOfAlgorithms.push_back(std::make_unique<_311246755_a>());
	algorithmsNames.push_back("_311246755_A");
	ListOfAlgorithms.push_back(std::make_unique<_311246755_a>());
	algorithmsNames.push_back("_311246755_B");
	*/
	extractFullMazesName();
	extractMazesName();

	//sanity check: assert vector lengths are of equal size
	assert(mazesFullNames.size() == mazesNames.size());


	/*
	numOfMazes = mazesFullNames.size();
	numOfAlgorithms = ListOfAlgorithms.size();
	//run all Algorithms on all mazes and store the scores
	for (int i = 0; i < numOfAlgorithms; ++i) {
		for (int j = 0; j < numOfMazes; ++j) {
			GameManager game(mazesFullNames.at(j), mazesFullNames.at(j)+algorithmsNames.at(i)+".output", ListOfAlgorithms.at(i).get());
			if ((scores.at(i).at(j) = game.startGame()) == -2) {
				std::cout << "Failed Processing the maze: " << mazesFullNames.at(j)<< std::endl;
				break;
			}

		}

	}*/
	std::list<std::string> algorithmNamesList;
	extractAlgorithmNames(algorithmNamesList);

	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	//loading each algorithm
	for (const auto& algorithmSoFileName : algorithmNamesList) {
		int result = registrar.loadAlgorithm(algorithmsPath + algorithmSoFileName + ".so", algorithmSoFileName);
		if (result == RegistrationStatus::FILE_CANNOT_BE_LOADED) {
			printError(Errors::algorithm_cannot_be_loaded, algorithmSoFileName, "");
			return FAILURE;
		}
		if (result == RegistrationStatus::NO_ALGORITHM_REGISTERED) {
			printError(Errors::algorithm_not_registered, algorithmSoFileName, "");
			return FAILURE;
		}
	}
	if (registrar.size() == 0) {
		// TODO: no algorithms loaded - print usage etc.
		// TOOD: for Oleg: i'm not sure what you mean by print usage etc.
		return FAILURE;
	}

	auto& algorithmNames = registrar.getAlgorithmNames();

	numOfMazes = mazesFullNames.size();
	numOfAlgorithms = registrar.size();

	std::vector<std::vector<int>> scores(numOfAlgorithms, std::vector<int>(numOfMazes,0));
	
	//run all Algorithms on all mazes and store the scores
	for (int i = 0; i < numOfAlgorithms; ++i) {
		auto algorithms = registrar.getAlgorithms();
		auto pName = algorithmNames.begin();
		auto algos = algorithms.begin();
		for (int j = 0; j < numOfMazes; ++j) {
			GameManager game(mazesFullNames.at(j), mazesFullNames.at(j) + *(pName++) + ".output", *(algos++));
			if ((scores.at(i).at(j) = game.startGame()) == -2) {
				std::cout << "Failed Processing the maze: " << mazesFullNames.at(j) << std::endl;
				break;
			}
		}
	}

	printScoresTable(scores);

	return SUCCESS;

}

// listing the files in a directory and add them to our vector of names
void MatchManager::extractFullMazesName() {
		for (auto& entry : fs::directory_iterator(mazesPath)) {
			mazesFullNames.push_back(entry.path().string());
	}
}


void MatchManager::extractAlgorithmNames(std::list<std::string> &names) {
	std::string fullPath;
	for (auto& entry : fs::directory_iterator(algorithmsPath)) {
		fullPath = entry.path().string();
		size_t lastDot = fullPath.find_last_of(".");
		// add filename to vector if it ends with an ".so" extension
		if (lastDot > 0 && lastDot != std::string::npos && fullPath.substr(lastDot).compare(".so") == 0) {
			//leave only the filename with extension
			std::string  filenameWithExtension = fullPath.substr(fullPath.find_last_of("/\\") + 1);
			//remove the extension
			std::string  filenameNoExtension = fullPath.substr(0, fullPath.find_last_of("."));
			names.push_back(filenameNoExtension);
		}
	}



}


void MatchManager::extractMazesName() {
	const std::string delimiter = "/";
	for (int j = 0; j < numOfMazes; ++j) {
		std::string s = mazesFullNames.at(j);
		size_t pos = 0;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			s.erase(0, pos + delimiter.length());
		}
		mazesNames.push_back(s);
	}
}

void MatchManager::printScoresTable(std::vector<std::vector<int>> scores) {
	//calculate the max name size from the mazes  
	int maxMaze = 0;
	int currSize;
	for (int i = 0; i < numOfMazes; ++i) {
		currSize = mazesNames.at(i).size();
		if (currSize > maxMaze)	maxMaze = currSize;
	}

	//calculate the max name size from the algorithms  
	int maxAlgo = 0;
	for (int i = 0; i < numOfAlgorithms; ++i) {
		currSize = algorithmsNames.at(i).size();
		if (currSize > maxAlgo)	maxAlgo = currSize;
	}
	int maxName = maxMaze > maxAlgo ? maxMaze : maxAlgo;

	const std::string sep = "|";
	const int total_width = (numOfMazes+1) * maxName + numOfMazes +2 + numOfMazes + 1;
	const std::string line = std::string(total_width+2, '-');
	const std::string empty = std::string(maxName+1, ' ');
	std::cout << line << std::endl;
	std::cout << sep << std::setw(maxName + 1) << empty << "  ";
	for (int i = 0; i < numOfMazes; ++i) {
		std::cout << sep << std::setw(maxName) << mazesNames.at(i) + " ";
	}
	std::cout << sep << std::endl;
	std::cout << line << std::endl;
	for (int i = 0; i < numOfAlgorithms; ++i) {
		std::cout << sep << std::setw(maxName + 1) << algorithmsNames.at(i) + " ";
		for (int j = 0; j < numOfMazes; ++j) {
			std::cout << sep << std::setw(maxName + 1) << scores.at(i).at(j);
		}
		std::cout << sep << std::endl;
		std::cout << line << std::endl;
	}


}

int MatchManager::processCommandLineInput() {
	if (extractPathFromString(mazesPath, "-maze_path")) {
		printError(Errors::maze_path, mazesPath, "-maze_path <path>");
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
	case Errors::maze_path:
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
	case Errors::algorithm_cannot_be_loaded:
		std::cout << "Algorithm could not be loaded." << std::endl;
		std::cout << "provided: " << input << std::endl;
		break;
	case Errors::algorithm_not_registered:
		std::cout << "Algorithm could not be registered..";
		std::cout << "provided: " << input << std::endl;
		break;
	default:
		break;
	}

}

