#include "MatchManager.h"

int MatchManager::processCommandLineArgs(int numOfArgs, char** argv, std::vector<std::string>& result, int &numOfThreads, bool& providedOutputArg) {
	if (numOfArgs != 1 && numOfArgs != 3 && numOfArgs != 5 && numOfArgs != 7 && numOfArgs != 9) {
		std::cout << "Wrong number of arguments in command line" << std::endl;
		std::cout << "each argument should be accompanied with a tag as follows:" << std::endl;
		std::cout << "match -maze_path <path> -algorithm_path <algorithm path> -output <output path> -num_threads <num of threads>" << std::endl;
		return FAILURE;
	}
	std::vector<std::string> temp;
	providedOutputArg = false;
	numOfThreads = 1;
	for (int i = 0; i < 3; ++i) {
		result.push_back("./");
	}
	for (int i = 0; i < numOfArgs; ++i) {
		temp.push_back(std::string(argv[i]));
	}
	for (int j = 1; j < numOfArgs; ++j) {
		if (temp.at(j).compare("-num_threads") == 0) {
			//try convert number represented as string to int 
			try {
				numOfThreads = std::stoi(temp.at(j+1));
				if (numOfThreads <= 0) {
					std::cout << "number Of Threads int command line arguments has to be at least 1" << std::endl;
					return FAILURE;
				}
				++j;
			}
			//if no conversion could be performed
			catch (const std::invalid_argument & e) {
				(void)e; // avoid "unreferenced local variable e" warnings
				std::cout << "expected number after the -num_threads tag" << std::endl;
				std::cout << "provided: " << temp.at(j+1) << std::endl;
				return FAILURE;
			}
		}
		else if (temp.at(j).compare("-maze_path") == 0) {
			result.at(0) = temp.at(j + 1);
			++j;
		}
		else if (temp.at(j).compare("-algorithm_path") == 0) {
			result.at(1) = temp.at(j + 1);
			++j;
		}
		else if (temp.at(j).compare("-output") == 0) {
			providedOutputArg = true;
			if (temp.at(j + 1).back() == '/')
				result.at(2) = temp.at(j + 1);
			else
				result.at(2) = temp.at(j + 1) + "/";
			++j;
		}
		else {
			std::cout << "missing tag in the command line arguments" << std::endl;
			return FAILURE;
		}
	}
	return SUCCESS;
}

int MatchManager::startMatch() {

	extractFullMazesName();
	extractMazesName();

	//sanity check: assert vector lengths are of equal size
	assert(mazesFullNames.size() == mazesNames.size());

	extractAlgorithmNames(algorithmNamesList);

	AlgorithmRegistrar & registrar = AlgorithmRegistrar::getInstance();
	//loading each algorithm
	int i = 0;
	for (const auto& algorithmSoFileName : algorithmNamesList) {
		int result = registrar.loadAlgorithm(eachAlgorithmPaths.at(i++), algorithmSoFileName);
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
		return FAILURE;
	}

	numOfMazes = mazesFullNames.size();
	numOfAlgorithms = registrar.size();

	//vector for holding the scores
	std::vector<std::vector<int>> scores(numOfAlgorithms, std::vector<int>(numOfMazes, 0));

	std::vector<std::thread> threads;
	// define and run threads
	for (int i = 0; i < numOfThreads; ++i) {
		threads.push_back(std::thread(&MatchManager::runGames, this, std::ref(registrar), std::ref(scores)));
		
	}
	//std::cout << "after define and run threads" << std::endl;

	for (auto& th : threads)	th.join();

	printScoresTable(scores);
	return SUCCESS;
}

void MatchManager::runGames(AlgorithmRegistrar &registrar, std::vector<std::vector<int>> &scores) {
	int i, j;
	while (true) {
		safelyIncIndexes(i, j);
		if (finished) break;
		auto algoItr = algorithmNamesList.begin();
		std::advance(algoItr, j);
		std::unique_ptr<AbstractAlgorithm> algorithm;
		registrar.getAlgorithmAt(algorithm, j);
		GameManager game(mazesFullNames.at(i), outputPath + mazesNames.at(i) + *algoItr + ".output", algorithm, providedOutputArg);
		scores[j][i] = game.startGame();
	}
}

void MatchManager::safelyIncIndexes(int &i, int &j) {
	std::lock_guard<std::mutex> guard(mutex);
	if (currMaze == numOfMazes - 1 && currAlgo == ((int)numOfAlgorithms) - 1) finished = true;
	else if (currAlgo < ((int)numOfAlgorithms) - 1) {
		++currAlgo;
	}
	else{
		currAlgo = 0;
		++currMaze;
	}
	i = currMaze;
	j = currAlgo;
}

// listing the files in a directory and add them to our vector of names if the suffix is .maze
void MatchManager::extractFullMazesName() {
	std::string str;
	std::string suffix = ".maze";
	for (auto& entry : fs::directory_iterator(mazesPath)) {
		str = entry.path().string();
		if (str.size() >= 6 && str.substr(str.size() - 5).compare(suffix) == 0)
			mazesFullNames.push_back(str);
	}
	numOfMazes = mazesFullNames.size();
}


void MatchManager::extractAlgorithmNames(std::list<std::string> & names) {
	std::string fullPath;
	for (auto& entry : fs::directory_iterator(algorithmsPath)) {
		fullPath = entry.path().string();
		size_t lastDot = fullPath.find_last_of(".");
		// add filename to vector if it ends with an ".so" extension
		if (lastDot > 0 && lastDot != std::string::npos && fullPath.substr(lastDot).compare(".so") == 0) {
			//leave only the filename with extension
			std::string  filenameWithExtension = fullPath.substr(fullPath.find_last_of("/") + 1);
			//remove the extension
			std::string  filenameNoExtension = filenameWithExtension.substr(0, filenameWithExtension.find_last_of("."));
			names.push_back(filenameNoExtension);
			//convert from a possibly relative path to the real path
			char resolved_path[260];
			realpath(fullPath.c_str(), resolved_path);
			eachAlgorithmPaths.push_back(resolved_path);
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
		s.erase(s.find(".maze"));
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
	for (std::string name : algorithmNamesList) {
		currSize = name.size();
		if (currSize > maxAlgo)	maxAlgo = currSize;
	}
	int maxName = maxMaze > maxAlgo ? maxMaze : maxAlgo;

	const std::string sep = "|";
	const int total_width = (numOfMazes + 1) * maxName + numOfMazes + 2 + numOfMazes + 1;
	const std::string line = std::string(total_width, '-');
	const std::string empty = std::string(maxName - 1, ' ');
	std::cout << line << std::endl;
	std::cout << sep << std::setw(maxName - 1) << empty << "  ";
	for (int i = 0; i < numOfMazes; ++i) {
		std::cout << sep << std::setw(maxName + 1) << mazesNames.at(i) + " ";
	}
	std::cout << sep << std::endl;
	std::cout << line << std::endl;
	auto pName = algorithmNamesList.begin();

	for (int i = 0; i < static_cast<int>(numOfAlgorithms); ++i, ++pName) {
		std::cout << sep << std::setw(maxName + 1) << *pName + " ";
		for (int j = 0; j < numOfMazes; ++j) {
			if (scores.at(i).at(j) == -2)
				std::cout << sep << std::setw(maxName + 1) << "";
			else
				std::cout << sep << std::setw(maxName + 1) << scores.at(i).at(j);
		}
		std::cout << sep << std::endl;
		std::cout << line << std::endl;
	}

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
		std::cout << "Algorithm could not be registered." << std::endl;
		std::cout << "provided: " << input << std::endl;
		break;
	default:
		break;
	}

}
