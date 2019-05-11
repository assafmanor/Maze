#pragma once
#include "../Match/AlgorithmRegistration.h"
#include <stdlib.h>	/* srand, rand	*/
#include <time.h> 	/* time			*/
#include <iostream> /* std::abs		*/
#include <vector>	/* std::vector	*/
#include <stack>	/* std::stack	*/

enum MazeObstacle { UNKNOWN = 0, WALL, SPACE };


class _311246755_b : public AbstractAlgorithm {

	class MazeCell {
	public:
		bool triedDirection[4]; //[0]: LEFT, [1]: RIGHT, [2]: UP, [3]: DOWN.
		int numOfDirsTried;
		MazeObstacle obstacle;
		MazeCell();
		virtual ~MazeCell();
		MazeCell(const MazeCell&);
	};

	//private fields
	std::vector<std::vector<MazeCell>> mappedMaze; // the player's recollection of the maze (places he's been at)
	std::stack<Move> path;
	int knownDimensions[2] = { 0 , 0 }; 	// player's knowledge of the number of rows and columns. changes during the game
	int curLocation[2] = { 0 , 0 };
	int bookmarkLoc[2] = { -1,-1 };
	bool isWidthKnown, isHeightKnown;
	bool placedBookmark;
	bool directionChosen;
	int lastSeq = 0; //the sequence number of the last bookmark placed

	//private methods
	void updateTriedFromOrigin(MazeCell&);
	void undoMove(Move);
	void updateLocation(Move);
	void updateMap();

public:
	_311246755_b();
	virtual ~_311246755_b();
	virtual Move move();
	virtual void hitWall();
	virtual void hitBookmark(int seq);


};