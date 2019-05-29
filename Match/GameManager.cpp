#include "GameManager.h"


int GameManager::startGame() {
	if (processFiles(mazeFileName)) {
		return -2; // in case of failure no need to run the player
	}
	//read moves from player for a maximum of $maxSteps steps
	AbstractAlgorithm::Move curMove;
	std::pair<int, int> nextPlayerLoc; // nextPlayerLoc.first = next row, nextPlayerLoc.second == next col.
	for (int i = 0; i < maxSteps; ++i) {
		curMove = player.get()->move();
		movePlayerAccordingToAlgorithm(curMove, nextPlayerLoc);
		writeMoveToFile(curMove);
		if (curMove == AbstractAlgorithm::Move::BOOKMARK) continue; //same cell -- no need to handle obstacle
		else {
			char obstacle = (*maze[nextPlayerLoc.first][nextPlayerLoc.second]).obstacle;
			int res = handleObstacle(obstacle, nextPlayerLoc);
			if (res > 0) { //player has found the treasure
				fout << "!";
				if (providedOutputArg) fout.close();
				return i+1;
			}
		}
	}
	//failed to solve maze in $maxSteps steps
	fout << "X";
	if (providedOutputArg)
		fout.close();
	return -1;
}


/**
 * Checks the given obstacle and calls the relevant methods if necessary (hitWall() and hitBookmark()).
 * Also reports if the player has found the treasure: returns 1 if the treasure was found, otherwise 0.
 */
int GameManager::handleObstacle(char obstacle, std::pair<int, int> nextPlayerLoc) {
	switch (obstacle) {
	case WALL:
		player.get()->hitWall();
		break;
	case TREASURE:
		return  1;
	case PLAYER: //if the player has landed on the starting cell -- treat it as a space
	case BOOK_MARK:
	case SPACE:
		playerRow = nextPlayerLoc.first;
		playerCol = nextPlayerLoc.second;
		if ((*maze[nextPlayerLoc.first][nextPlayerLoc.second]).obstacle == 'B') {
			player.get()->hitBookmark((*maze[nextPlayerLoc.first][nextPlayerLoc.second]).bookmarSerial);
		}
		break;
	}
	return 0;
}


/**
 * Moves the player inside the maze according to the given move.
 * i.e: updates the values of nextPlayerLoc.
 */
void GameManager::movePlayerAccordingToAlgorithm(const AbstractAlgorithm::Move move, std::pair<int, int> &nextPlayerLoc) {
	switch (move) {
	case AbstractAlgorithm::Move::LEFT:
		nextPlayerLoc.first = playerRow;
		if (playerCol == 0) {
			nextPlayerLoc.second = numOfCols - 1;
		}
		else {
			nextPlayerLoc.second = playerCol - 1;
		}
		break;

	case AbstractAlgorithm::Move::RIGHT:
		nextPlayerLoc.first = playerRow;
		nextPlayerLoc.second = (playerCol + 1) % numOfCols;
		break;

	case AbstractAlgorithm::Move::UP:
		if (playerRow == 0) {
			nextPlayerLoc.first = numOfRows - 1;
		}
		else {
			nextPlayerLoc.first = playerRow - 1;
		}
		nextPlayerLoc.second = playerCol;
		break;

	case AbstractAlgorithm::Move::DOWN:
		nextPlayerLoc.first = (playerRow + 1) % numOfRows;
		nextPlayerLoc.second = playerCol;
		break;

	default: //Direction::BOOKMARK
		(*maze[playerRow][playerCol]).obstacle = 'B';
		(*maze[playerRow][playerCol]).bookmarSerial = ++currBookmarSerial;
		break;
	}
}


/**
 * Writes the given move in a new row to the output file.
 */
void GameManager::writeMoveToFile(AbstractAlgorithm::Move move) {
	switch (move) {
	case AbstractAlgorithm::Move::LEFT:
		fout << "L" << std::endl;
		break;
	case AbstractAlgorithm::Move::RIGHT:
		fout << "R" << std::endl;
		break;
	case AbstractAlgorithm::Move::UP:
		fout << "U" << std::endl;
		break;
	case AbstractAlgorithm::Move::DOWN:
		fout << "D" << std::endl;
		break;
	default: //Direction::BOOKMARK
		fout << "B" << std::endl;
		break;
	}
}


/*
 * opens the output file
 * in case of success return SUCCESS(0) otherwise FAILURE(1)
 */
int GameManager::openOutputFile() {
	//check if the file already exist
	std::ifstream infile(outputFileName);
	if (infile.good()) {
		infile.close();
		return FAILURE;
	}
	infile.close();
	if (providedOutputArg) {
		fout.open(outputFileName);
		if (!fout.is_open())
			return FAILURE;
	}
	return SUCCESS;
}


/*
 * process the input files
 * maze file format:
 * Line 1: maze name
 * Line 2: MaxSteps=<num> MaxSteps for solving this maze.
 * Line 3: Rows=<num> Number of Rows in maze.
 * Line 4: Cols=<num> Number of Cols in maze.
 * Lines 5 and on: the maze itself.
 * valid maze characters:
 * # - represents a wall
 * space - represents a pass
 * @ - represents the player in the maze (initial position of the player)
 * $ - represents the end of the maze (the treasure that we seek)
 * if there were errors while reading or analyzing the input file appropriate massage printed to the screen
 */
int GameManager::processFiles(const std::string mazeFilePath) {

	std::string line;
	std::ifstream fin(mazeFilePath);

	if (fin.is_open()) {
		std::getline(fin, line);
		nameOfMaze = line;

		std::getline(fin, line);
		if (extractNumFromString(line, maxSteps, "MaxSteps", "=")) {
			printError(ErrorStatus::MaxSteps_Format, line);
			occurredError = true;
		}

		std::getline(fin, line);
		if (extractNumFromString(line, numOfRows, "Rows", "=")) {
			printError(ErrorStatus::Rows_Format, line);
			occurredError = true;
		}
		std::getline(fin, line);
		if (extractNumFromString(line, numOfCols, "Cols", "=")) {
			printError(ErrorStatus::Cols_Format, line);
			occurredError = true;
		}

		if (!occurredError) {
			//initialize the maze
			maze.resize(numOfRows);
			for (int i = 0; i < numOfRows; ++i) {
				for (int j = 0; j < numOfCols; ++j) {
					maze[i].push_back(std::make_unique <Cell>());
				}
			}
			 //fill our maze with the given maze in the file
			for (int i = 0; i < numOfRows; ++i) {
				int j = 0;
				if (std::getline(fin, line)) {
					for (int length = (int)line.length(); j < length && j < numOfCols; ++j) {
						if (line[j] == '\r') {
							(*maze[i][j]).obstacle = SPACE;
						}
						else {
							(*maze[i][j]).obstacle = line[j];
						}

						//counts the number of players and treasures provided
						if (line[j] == PLAYER) {
							++numOfPlayersProvided;
							//update the location of the player
							playerRow = i;
							playerCol = j;

						}
						if (line[j] == TREASURE) ++numOfTreasuresProvided;
					}
				}
				while (j < numOfCols) {
					(*maze[i][j]).obstacle = SPACE;
					++j;
				}
			}

			if (numOfPlayersProvided == 0) {
				printError(ErrorStatus::Missing_Player);
				occurredError = true;
			}
			if (numOfTreasuresProvided == 0) {
				printError(ErrorStatus::Missing_Treasure);
				occurredError = true;
			}
			if (numOfPlayersProvided > 1) {
				printError(ErrorStatus::More_Than_One_Player);
				occurredError = true;
			}
			if (numOfTreasuresProvided > 1) {
				printError(ErrorStatus::More_Than_One_Treasure);
				occurredError = true;
			}
			//characters validation provided in maze
			for (int i = 0; i < numOfRows; ++i) {
				for (int j = 0; j < numOfCols; ++j) {
					if (!isValidCharacter((*maze[i][j]).obstacle)) {
						if (!((*maze[i][j]).obstacle == '\r' && j == numOfCols - 1)) {
							printError(ErrorStatus::Wrong_Character, "", (*maze[i][j]).obstacle, i + 1, j + 1);
							occurredError = true;
						}
					}
				}
			}
		}
		fin.close();
		//faild open output file
		if (!occurredError && openOutputFile()) {
			printError(ErrorStatus::output_File_Path);
			occurredError = true;
		}
		if (occurredError) return FAILURE;

		return SUCCESS;
	}
	else {
		printError(ErrorStatus::Maze_File_Path);
		return FAILURE;
	}
}


/*
 *checks whether the given char c is a valid maze character
 */
bool GameManager::isValidCharacter(const char& c) {
	return (c == WALL || c == SPACE || c == PLAYER || c == TREASURE);
}


/*
 * prints the error specified in ErrorStatus to the screen
 */
void GameManager::printError(ErrorStatus error, std::string line, char c, size_t row, size_t col) {

	switch (error) {
	case ErrorStatus::Maze_File_Path:
		std::cout << "Command line argument for maze: " << mazeFileName << " doesn't lead to a maze file or leads to a file that cannot be opened" << std::endl;
		break;
	case ErrorStatus::output_File_Path:
		std::cout << "Command line argument for output file: " << outputFileName << " points to a bad path or to a file that already exists" << std::endl;
		break;
	case ErrorStatus::MaxSteps_Format:
		if (!occurredWrongFormat) {
			std::cout << HEADLINE1 << std::endl;
			occurredWrongFormat = true;
		}
		std::cout << "expected in line 2 - MaxSteps = <num>" << std::endl;
		std::cout << "got: " << line << std::endl;
		break;
	case ErrorStatus::Rows_Format:
		if (!occurredWrongFormat) {
			std::cout << HEADLINE1 << std::endl;
			occurredWrongFormat = true;
		}
		std::cout << "expected in line 3 - Rows = <num>" << std::endl;
		std::cout << "got: " << line << std::endl;
		break;
	case ErrorStatus::Cols_Format:
		if (!occurredWrongFormat) {
			std::cout << HEADLINE1 << std::endl;
			occurredWrongFormat = true;
		}
		std::cout << "expected in line 4 - Cols = <num>" << std::endl;
		std::cout << "got: " << line << std::endl;
		break;
	case ErrorStatus::Missing_Player:
		if (!wrongMazeInput) {
			std::cout << HEADLINE2 << std::endl;
			wrongMazeInput = true;
		}
		std::cout << "Missing @ in maze" << std::endl;
		break;
	case ErrorStatus::Missing_Treasure:
		if (!wrongMazeInput) {
			std::cout << HEADLINE2 << std::endl;
			wrongMazeInput = true;
		}
		std::cout << "Missing $ in maze" << std::endl;
		break;
	case ErrorStatus::More_Than_One_Player:
		if (!wrongMazeInput) {
			std::cout << HEADLINE2 << std::endl;
			wrongMazeInput = true;
		}
		std::cout << "More than one @ in maze" << std::endl;
		break;
	case ErrorStatus::More_Than_One_Treasure:
		if (!wrongMazeInput) {
			std::cout << HEADLINE2 << std::endl;
			wrongMazeInput = true;
		}
		std::cout << "More than one $ in maze" << std::endl;
		break;
	case ErrorStatus::Wrong_Character:
		if (!wrongMazeInput) {
			std::cout << HEADLINE2 << std::endl;
			wrongMazeInput = true;
		}
		if (c == '\t')
			std::cout << "Wrong character in maze: " << "TAB" << " in row " << row << ", " << "col " << col << std::endl;
		else
			std::cout << "Wrong character in maze: " << c << " in row " << row << ", " << "col " << col << std::endl;
		break;
	default:
		break;
	}

}


/*
 * extract the number from received string of the form: "blabla = 5"
 * checks that the first and the second words in the string indeed matches expected "firstWord" and "scondWord"
 * checks that the extracted number greater than 0
 * stores the result in the passed arg agument
 * in case of success return SUCCESS(0) otherwise FAILURE(1)
 */
int GameManager::extractNumFromString(std::string str, int& arg, std::string firstWord, std::string secondWord) const {
	std::vector<std::string> result;
	std::istringstream iss(str);
	//break the strings to tokens
	for (std::string s; iss >> s; )
		result.push_back(s);
	//in case no space before and after the '='
	if (result.size() == 1) {
		if (result.at(0).find('=')) {
			//checks that first word matches expected firstWord
			if (result.at(0).substr(0, result.at(0).find("=")).compare(firstWord) != 0)
				return FAILURE;
			//try convert number represented as string to int 
			try {
				arg = std::stoi(result.at(0).substr(result.at(0).find('=') + 1));
				if (arg <= 0)	return FAILURE;
				return SUCCESS;

			}
			//if no conversion could be performed
			catch (const std::invalid_argument & e) {
				(void)e; // avoid "unreferenced local variable e" warnings
				return FAILURE;
			}

		}
		return FAILURE;
	}
	//otherwise must be 3 tokens/words
	else if (result.size() != 3 || result.at(0).compare(firstWord) != 0 || result.at(1).compare(secondWord) != 0)
		return FAILURE;

	try {
		arg = std::stoi(result.at(2));
		if (arg <= 0)	return FAILURE;

	}
	//if no conversion could be performed
	catch (const std::invalid_argument & e) {
		(void)e; // avoid "unreferenced local variable e" warnings
		return FAILURE;
	}

	return SUCCESS;
}
