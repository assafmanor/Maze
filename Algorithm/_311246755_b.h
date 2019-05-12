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
	Move chooseUntriedDirection(MazeCell&);
	Move backtrackMove();
	void updateTriedFromOrigin(MazeCell&);
	void undoMove(Move);
	std::pair <int, int> calcNextLocation(const Move);
	void updateLocation(Move);
	void updateMap();
	void addNewCell(int, int, const MazeObstacle);
	void addNewCell(int, int, MazeCell&);
	void updateCellObstacle(int, int, MazeObstacle);
	Move chooseMove();
	void updateKnownDimentions(Move);
	int setHeightBounds(int);
	int setWidthBounds(int);
	void unifyCells(MazeCell&, MazeCell&);
	void unifyRows(std::map <int, MazeCell>, std::map <int, MazeCell>, const int);
	void fixRows(const int);
	void fixCols(const int);
	void fixBookmarks(const int, const int);
	int findCorrectCoord(const int, const int, const int);
	bool doesCellExist(std::pair <int, int>);



public:
	_311246755_b();
	virtual ~_311246755_b();
	virtual Move move();
	virtual void hitWall();
	virtual void hitBookmark(int seq);


};