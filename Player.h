#pragma once
#include <vector>
#include <stack>
#include <string>
//#include <bits/stdc++.h>
#include <cstdlib>

enum Direction { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3, BOOKMARK };
enum MazeObstacle { UNKNOWN = 0, WALL, SPACE };


class MazeCell {
public:
	bool triedDirection[4]; //[0]: LEFT, [1]: RIGHT, [2]: UP, [3]: DOWN.
	int numOfLocsTried;
	MazeObstacle obstacle;
	MazeCell();
	virtual ~MazeCell();
	MazeCell(const MazeCell&);
};


class Player {
	// the player's recollection of the maze (places he's been at)
	std::vector<std::vector<MazeCell>> mappedMaze;
	std::stack<Direction> path;
	int knownDimensions[2] = { 0,0 }; 	// player's knowledge of the number of rows and columns. changes during the game
	int bookmarkLoc[2] = { -1,-1 };
	bool widthKnown, heightKnown;
	bool placedBookmark;
	bool directionChosen;
	Direction lastDirection;
	void undoMove(Direction);
	void updateLocation(Direction);
	void updateMap();
	bool checkDirection(Direction);

public:
	/*TODO: move to private*/int curLocation[2] = { 0 , 0 };
	Player();
	void hitWall();
	void hitBookmark();
	Direction move();
	virtual ~Player();

	//TODO: for debugging
	void printMaze();

};


