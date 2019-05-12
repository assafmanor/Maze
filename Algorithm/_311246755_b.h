#pragma once
#include "../Match/AlgorithmRegistration.h"
#include <cassert>
#include <map>
#include <utility>
#include <stdlib.h>	/* srand, rand	*/
#include <time.h> 	/* time			*/
#include <iostream> /* std::abs		*/
#include <vector>	/* std::vector	*/
#include <stack>	/* std::stack	*/

enum MazeObstacle { UNKNOWN = 0, WALL, SPACE };





class _311246755_b : public AbstractAlgorithm {

	class MazeCell {
	public:
		MazeCell(MazeObstacle);
		MazeCell();
		virtual ~MazeCell();
		MazeCell(const MazeCell&);

		std::map <Move, bool> triedDirection;
		bool hasBookmark;
		int numOfDirsTried;
		MazeObstacle obstacle;
	};

	//private fields
	std::map <int, std::map <int, MazeCell>> mappedMaze;
	std::stack<Move> path;
	////int knownDimensions[2] = { 0 , 0 }; 	// player's knowledge of the number of rows and columns. changes during the game
	int curRow = 0, curCol = 0;
	std::vector <std::pair<int, int>> bookmarks; //key is the sequence number
	bool isWidthKnown, isHeightKnown;
	bool placedBookmark = false;
	bool isDirectionChosen = false;
	bool isBacktracking = false;
	bool movedLastTurn = false;
	int leftmostKnownCol, rightmostKnownCol, lowestKnownRow, highestKnownRow;

	//private methods


	/*
	* Deterministically chooses a direction from the directions not yet tried from the cell.
	*/
	Move chooseUntriedDirection(MazeCell&);


	/*
	* Returns the opposite direction to the last direction the player moved.
	*/
	Move backtrackMove();


	/*
	* Updates the current cell's triedDirection and numOfTriedDirs according to the direction the player came from
	*/
	void updateTriedFromOrigin(MazeCell&);


	/*
	* Cancels the last direction change.
	* Updates the cell's obstacle to MazeObstacle::WALL and removes the last direction change from path.
	*/
	void undoMove(Move);


	/*
	* Calculates the next location the player will be at, according to the given move, the known maze dimentions and the player's current location.
	*/
	std::pair <int, int> calcNextLocation(const Move);


	/*
	* Updates the player's location according to the given direction.
	* Takes into consideration if the width/height are known and updates the location accordingly.
	*/
	void updateLocation(Move);


	/*
	* Adds a new cell at location to the mapped maze, given only an obstacle.
	*/
	void addNewCell(int, int, const MazeObstacle);


	/*
	* Adds a new cell at location to the mapped maze, given an existing cell.
	*/
	void addNewCell(int, int, MazeCell&);


	/*
	* Updates the obstacle of an existing cell - mappedMaze[row][col] to newObstacle.
	*/
	void updateCellObstacle(int, int, MazeObstacle);


	/*
	* Chooses the next move direction.
	*/
	Move chooseMove();


	/*
	* Updates the highest/lowest/leftmost/rightmost known row/col according to the move and current location.
	*/
	void updateKnownDimentions(Move);


	/*
	* Ascertains the true height of the maze, and fixes the value of the highest known rows of the maze.
	* Returns the true height of the maze
	*/
	int setHeightBounds(int);


	/*
	* Ascertains the true width of the maze, and fixes the value of the rightmost known colums of the maze.
	* Returns the true width of the maze
	*/
	int setWidthBounds(int);


	/*
	* Unifies the information gathered from both cells to the receiver cell.
	*/
	void unifyCells(MazeCell&, MazeCell&);


	/*
	* Unifies all the information found in all the cells present in both rows, and stores the unified information inside the receiver row.
	*/
	void unifyRows(std::map <int, MazeCell>, std::map <int, MazeCell>, const int);


	/*
	* Moves the information gathered from cells with wrong row values to the corresponding cells in mappedMaze (the ones that have the correct row value).
	* The information is then unified with the information already gathered by the correct cell, or a new cell is created.
	*/
	void fixRows(const int);


	/*
	* Moves the information gathered from cells with wrong column values to the corresponding cells in mappedMaze (the ones that have the correct column value).
	* The information is then unified with the information already gathered by the correct cell, or a new cell is created.
	*/
	void fixCols(const int);


	/*
	* Fixes the coordinates of all bookmarks according to the given maze width and height.
	* This method works only if exactly one of the values of mazeWidth and mazeHeight is -1 (meaning that the value is irrelevant).
	*/
	void fixBookmarks(const int, const int);


	/*
	* Calculates the correct value for the given coordinate, according to the threshold and mazeDim.
	*/
	int findCorrectCoord(const int, const int, const int);


	/*
	* Returns true iff there exists a cell in mappedMaze with coordinates matching loc.
	* i.e. mappedMaze[loc.first][loc.second].
	*/
	bool doesCellExist(std::pair <int, int>);



public:
	_311246755_b();


	virtual ~_311246755_b();


	/*
	* Chooses the next move, updates the map, bookmarks, and path.
	*/
	virtual Move move();


	/*
	* Cancels the last direction change.
	* Updates the cell's obstacle to MazeObstacle::WALL and removes the last direction change from path.
	*/
	virtual void hitWall();


	/*
	* Compares between the coordinates remembered by bookmarks.at(seq -1) and the current location.
	* If it turns out that they are different, then the player has passed through one of the borders.
	* The method uses this information in order to narrow down the size of the mappedMaze.
	* The information from cells with wrong coordinates is then transfered to the cells with the corresponding correct coordinates.
	*/
	virtual void hitBookmark(int seq);


};