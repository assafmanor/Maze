#include <iostream>
#include "GameManager.h"
#include "MatchManager.h"




int main(int argc, char **argv) {
	std::vector<std::string> paths;
	if (MatchManager::processCommandLineArgs(argc, argv, paths) == FAILURE)
		return 0;
	MatchManager match(paths.at(0), paths.at(1), paths.at(2));
	match.startMatch();
	return 0;


}