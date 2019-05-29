#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream> 
#include <string>
#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"
//#include "Player.h"


#define WALL  '#'
#define SPACE ' '
#define PLAYER '@'
#define TREASURE '$'
#define BOOK_MARK 'B'

#define SUCCESS 0
#define FAILURE 1

#define HEADLINE1  "Bad maze file header:"
#define HEADLINE2  "Bad maze in maze file:"


//possible errors
enum class ErrorStatus {
	Maze_File_Path, output_File_Path, MaxSteps_Format, Rows_Format, Cols_Format,
	Missing_Player, Missing_Treasure, More_Than_One_Player, More_Than_One_Treasure, Wrong_Character
};

class GameManager {

	class Cell {
	public:
		char obstacle;
		int bookmarSerial;
		Cell() :obstacle(' '), bookmarSerial(0) {};

	};

	//char **maze; //holds the maze given in the input file
	std::vector< std::vector<std::unique_ptr<Cell>>> maze;
	std::string nameOfMaze; //holds the name of the maze given in the input file
	std::string mazeFileName, outputFileName; //maze file path, output file path
	std::unique_ptr<AbstractAlgorithm>& player;

	int playerRow, playerCol; //the row and col of the player at any phase
	int maxSteps; //holds the max number of steps the player can move
	bool occurredError; //indicates whether error occurred while processing input files
	bool occurredWrongFormat; //indicates whether Wrong Format given in the input file
	bool wrongMazeInput; //indicates whether Wrong maze given in the input file
	int numOfPlayersProvided; //holds the number of players given in the input maze
	int numOfTreasuresProvided; //holds the number of Treasures given in the input maze
	int numOfRows; //number of rows in the maze
	int numOfCols; //number of cols in the maze
	int bookmarkRow; //the row of the placed bookmark
	int bookmarkCol; //the col of the placed bookmark
	int currBookmarSerial;
	std::ofstream fout;
	bool providedOutputArg;


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
	int processFiles(const std::string mazeFilePath);


	/*
	 * opens the output file
	 * in case of success return SUCCESS(0) otherwise FAILURE(1)
	 */
	int openOutputFile();


	/*
	 * extract the number from received string of the form: "blabla = 5"
	 * checks that the first and the second words in the string indeed matches expected "firstWord" and "scondWord"
	 * checks that the extracted number greater than 0
	 * stores the result in the passed arg agument
	 * in case of success return SUCCESS(0) otherwise FAILURE(1)
	 */
	int extractNumFromString(std::string str, int& arg, std::string firstWord, std::string secondWord) const;


	/*
	 * prints the error specified in ErrorStatus to the screen
	 */
	void printError(ErrorStatus error, std::string line = "", char c = ' ', std::size_t row = 0, std::size_t col = 0);


	/*
	 *checks whether the given char c is a valid maze character
	 */
	bool isValidCharacter(const char& c);


public:

	/*
	 * Constructor
	 * first arg is the maze file
	 * second arg is the output file
	 */
	explicit GameManager(const std::string mazeFile, const std::string outputFile, std::unique_ptr<AbstractAlgorithm>& _player, bool _providedOutputArg)
		: mazeFileName(mazeFile), outputFileName(outputFile), player(_player), occurredError(false), occurredWrongFormat(false),
		wrongMazeInput(false), numOfPlayersProvided(0), numOfTreasuresProvided(0), bookmarkRow(-1), bookmarkCol(-1), currBookmarSerial(0), providedOutputArg(_providedOutputArg) {};


	/*
	 * starts the game by processing the files
	 * conduct interaction with the player
	 * output to output file each step done by the player
	 * and the result: X (in case of failing to solve the maze), ! (in case the maze was solved)
	 * prints to screen the result
	 * return number of steps the algo has taken
	 */
	int startGame();


	/**
	 * Checks the given obstacle and calls the relevant methods if necessary (hitWall() and hitBookmark()).
	 * Also reports if the player has found the treasure: returns 1 if the treasure was found, otherwise 0.
	 */
	int handleObstacle(char, std::pair<int, int>);


	/**
	 * Moves the player inside the maze according to the given move.
	 * i.e: updates the values of nextPlayerLoc.
	 */
	void movePlayerAccordingToAlgorithm(const AbstractAlgorithm::Move, std::pair<int, int>&);


	/**
	 * Writes the given move in a new row to the output file.
	 */
	void writeMoveToFile(AbstractAlgorithm::Move);
};
