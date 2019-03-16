

#include "GameManager.h"


GameManager::~GameManager() {

	for (int i = 0; i < numOfRows; ++i)
		delete maze[i];
	delete[] maze;

}

void GameManager::startGame() {
	
	if (processFiles(mazeFileName, outputFileName))
		return; // in case of failure no need to run the player

	//sanity check
	printMaze();
	
	//here: read moves from player...

	fout.close();
}
int GameManager::openOutputFile() {
	fout.open(outputFileName);
	if (!fout.is_open())
		return FAILURE;
	return SUCCESS;
}
int GameManager::processFiles(const std::string mazeFilePath, const std::string outputFile) {

	std::string line;
	std::ifstream fin(mazeFilePath);

	if (fin.is_open()) {
		
		//faild open output file
		if (openOutputFile())
			printError(ErrorStatus::output_File_Path);

		std::getline(fin, line);
		nameOfMaze = line;

		std::getline(fin, line);
		if(extractNumFromString(line, maxSteps, "MaxSteps", "="))
			printError(ErrorStatus::MaxSteps_Format, line);

		std::getline(fin, line);
		if (extractNumFromString(line, numOfRows, "Rows", "="))
			printError(ErrorStatus::Rows_Format, line);

		std::getline(fin, line);
		if (extractNumFromString(line, numOfCols, "Cols", "="))
			printError(ErrorStatus::Cols_Format, line);
	
		//iniitalize the maze
		maze = new char*[numOfRows];
		for (int i = 0; i < numOfRows; ++i)
			maze[i] = new char[numOfCols];
		
		//fill our maze with the given maze in the file
		for (int i = 0; i < numOfRows; ++i) {
			int j = 0;
			if (std::getline(fin, line)) {
				for (int length = (int)line.length(); j < length; ++j) {
					maze[i][j] = line[j];
				}
			}
			
			while (j < numOfCols) {
				maze[i][j] = SPACE;
				++j;
			}
		}
		fin.close();
		return SUCCESS;
	}
	else {
		printError(ErrorStatus::Maze_File_Path);
		//faild open output file
		if(openOutputFile())
			printError(ErrorStatus::output_File_Path);
		return FAILURE;
	}
}

void GameManager::printMaze() {
	std::cout << "The name of the maze: " << nameOfMaze << std::endl;
	std::cout << "MaxSteps: " << maxSteps << std::endl;
	std::cout << "number of rows: " << numOfRows << std::endl;
	std::cout << "number of cols: " << numOfCols << std::endl;
	
	for (int i = 0; i < numOfRows; ++i) {
		for (int j = 0; j < numOfCols; ++j) {
			std::cout << maze[i][j];
		}
		std::cout << std::endl;
	}
}

void GameManager::printError(ErrorStatus error, std::string line, char c, size_t row, size_t col) {

	switch (error) {
	case ErrorStatus::Maze_File_Path:
		std::cout << "Command line argument for maze:" << mazeFileName << "doesn't lead to a maze file or leads to a file that cannot be opened" << std::endl;
		break;
	case ErrorStatus::output_File_Path:
		std::cout << "Command line argument for output file:" << outputFileName << "points to a bad path or to a file that already exists" << std::endl;
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
		std::cout << "Wrong character in maze:" << c << " in row " << row << ", " << "col " << col <<std::endl;
		break;
	default: 
		break;
	}

}

//extract the number from string of the form: "blabla = 5"
//we need to make sure that this is the only form that can be provided
int GameManager::extractNumFromString(std::string str, int &maxSteps, std::string firstWord, std::string secondWord) {
	std::vector<std::string> result;
	std::istringstream iss(str);
	for (std::string s; iss >> s; ) 
		result.push_back(s);
	if (result.size() != 3 || result.at(0).compare(firstWord) != 0 || result.at(1).compare(secondWord) != 0)
		return FAILURE;
	maxSteps = std::stoi(result.at(2));
	return SUCCESS;
}


	