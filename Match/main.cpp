#include <iostream>
#include "GameManager.h"
#include "MatchManager.h"




int main(int argc, char** argv) {
	std::vector<std::string> paths;
	int numOfThreads;
	bool providedOutputArg;
	if (MatchManager::processCommandLineArgs(argc, argv, paths, numOfThreads, providedOutputArg) == FAILURE)
		return 0;
	MatchManager match(paths.at(0), paths.at(1), paths.at(2), numOfThreads, providedOutputArg);
	match.startMatch();
	return 0;


}
