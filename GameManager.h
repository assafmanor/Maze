#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream> 
#include <string>
#include "Player.h"


#define WALL  '#'
#define SPACE ' '
#define PLAYER '@'
#define TREASURE '$'

#define SUCCESS 0
#define FAILURE 1

#define HEADLINE1  "Bad maze file header:"
#define HEADLINE2  "Bad maze in maze file:"


//possible errors
//some kind of logger might be the better choice
enum class ErrorStatus {
	Maze_File_Path, output_File_Path, MaxSteps_Format, Rows_Format, Cols_Format,
	Missing_Player, Missing_Treasure, More_Than_One_Player, More_Than_One_Treasure, Wrong_Character
};

class GameManager {
	
	// maybe a maze of calss cell (cell **maze) is a better choice
	char **maze;
	std::string nameOfMaze;
	std::string mazeFileName,  outputFileName;
	Player player;
	int playerRow, playerCol;
	//std::size_t maxSteps;
	int maxSteps;

	bool occurredError;
	bool occurredWrongFormat;
	bool wrongMazeInput;
	//std::size_t numOfRows;
	//std::size_t numOfCols;
	int numOfRows;
	int numOfCols;
	int bookmarkRow;
	int bookmarkCol;

	int numOfPlayersProvided;
	int numOfTreasuresProvided;

	std::ofstream fout;
	int processFiles(const std::string mazeFilePath, const std::string outputFile);
	int openOutputFile();
	void printMaze();
	int extractNumFromString(std::string str, int &n, std::string firstWord, std::string secondWord);
	void printError(ErrorStatus error, std::string line = "", char c = ' ', std::size_t row = 0, std::size_t col = 0);
	bool validCharacter(const char &c);
public:
	explicit GameManager(const char *mazeFile, const char *outputFile) : mazeFileName(std::string(mazeFile)), outputFileName(std::string(outputFile)), occurredWrongFormat(false), wrongMazeInput(false), numOfPlayersProvided(0), numOfTreasuresProvided(0), bookmarkRow(-1), bookmarkCol(-1){};
	virtual ~GameManager();
	int startGame();
};
