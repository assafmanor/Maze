#pragma once
#include "AbstractAlgorithm.h"
#include <stdlib.h>	/* srand, rand	*/
#include <time.h> 	/* time			*/
#include <iostream> /* std::abs		*/
#include <vector>	/* std::vector	*/
#include <stack>	/* std::stack	*/

//after the dynamic registration need to change back WALLL to WALL and SPACEE to SPACE
enum MazeObstacle { UNKNOWN = 0, WALLL, SPACEE };


class MazeCell {
public:
	bool triedDirection[4]; //[0]: LEFT, [1]: RIGHT, [2]: UP, [3]: DOWN.
	int numOfDirsTried;
	MazeObstacle obstacle;
	MazeCell();
	virtual ~MazeCell();
	MazeCell(const MazeCell&);
};


class _311246755_a : public AbstractAlgorithm {

	//private fields
	std::vector<std::vector<MazeCell>> mappedMaze; // the player's recollection of the maze (places he's been at)
	std::stack<Move> path;
	int knownDimensions[2] = { 0 , 0 }; 	// player's knowledge of the number of rows and columns. changes during the game
	int curLocation[2] = { 0 , 0 };
	int bookmarkLoc[2] = { -1,-1 };
	bool isWidthKnown, isHeightKnown;
	bool placedBookmark;
	bool directionChosen;

	//private methods
	void updateTriedFromOrigin(MazeCell&);
	void undoMove(Move);
	void updateLocation(Move);
	void updateMap();

public:
	_311246755_a();
	virtual ~_311246755_a();
	virtual Move move();
	virtual void hitWall();
	virtual void hitBookmark(int seq);


};