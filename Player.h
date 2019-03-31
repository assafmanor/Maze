#pragma once
#include <stdlib.h>	/* srand, rand	*/
#include <time.h> 	/* time			*/
#include <iostream> /* std::abs		*/
#include <vector>	/* std::vector	*/
#include <stack>	/* std::stack	*/

enum Direction { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3, BOOKMARK };
enum MazeObstacle { UNKNOWN = 0, WALL, SPACE };


class MazeCell {
public:
	bool triedDirection[4]; //[0]: LEFT, [1]: RIGHT, [2]: UP, [3]: DOWN.
	int numOfDirsTried;
	MazeObstacle obstacle;
	MazeCell();
	virtual ~MazeCell();
	MazeCell(const MazeCell&);
};


class Player {

	//private fields
	std::vector<std::vector<MazeCell>> mappedMaze; // the player's recollection of the maze (places he's been at)
	std::stack<Direction> path;
	int knownDimensions[2] = { 0 , 0 }; 	// player's knowledge of the number of rows and columns. changes during the game
	int curLocation[2] = { 0 , 0 };
	int bookmarkLoc[2] = { -1,-1 };
	bool widthKnown, heightKnown;
	bool placedBookmark;
	bool directionChosen;

	//private methods
	void updateTriedFromOrigin(MazeCell&);
	void undoMove(Direction);
	void updateLocation(Direction);
	void updateMap();

public:
	Player();
	void hitWall();
	void hitBookmark();
	Direction move();
	virtual ~Player();

};