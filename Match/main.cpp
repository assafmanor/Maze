#include <iostream>
#include "GameManager.h"
#include "MatchManager.h"


int main(int argc, char **argv) {
	/*if (argc > 4) {
		std::cout << "too many arguments in command line" << std::endl;
		return 0;
	}*/
	//command line: match
	/*else */if (argc < 2) {
		MatchManager match("./");
	}
	//command line: match [-maze_path <path>]
	else if (argc < 3) {
		MatchManager match(argv[1]);
	}
	//command line: match [-maze_path <path>] [-algorithm_path <algorithm path>]
	else if (argc < 4) {
		MatchManager match(argv[1], argv[2]);
	}
	//command line: match [-maze_path <path>] [-algorithm_path <algorithm path>] [-output <output path>]
	MatchManager match(argv[1], argv[2], argv[3]);

	match.startMatch();
	return 0;
}