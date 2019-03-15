#include <iostream>
#include "GameManager.h"

int main(int argc, char **argv) {

	if (argc < 2) {
		std::cout << "Missing maze file argument in command line" << std::endl;
		std::cout << "Missing output file argument in command line" << std::endl;
		return 1;
	}
	else if (argc < 3) {
		std::cout << "Missing output file argument in command line" << std::endl;
	}

	GameManager game(argv[1], argv[2]);
	game.startGame();

	return 0;
}