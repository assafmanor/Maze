#include <iostream>
#include <stdlib.h>	/* srand, rand */
#include <time.h> 	/* time */
#include "Player.h"


//////// CLASS: MazeCell ////////

MazeCell::MazeCell() : numOfLocsTried(0), obstacle(MazeObstacle::UNKNOWN) {
	for (int i = 0; i < 4; i++) {
		triedDirection[i] = false;
	}

}

MazeCell::~MazeCell() {}

MazeCell::MazeCell(const MazeCell &copied) {
	numOfLocsTried = copied.numOfLocsTried;
	obstacle = copied.obstacle;
	for (int i = 0; i < 4; i++) {
		triedDirection[i] = copied.triedDirection[i];
	}
}


//////// CLASS: Player ////////

Player::Player() : widthKnown(false), heightKnown(false), placedBookmark(false), directionChosen(false), mappedMaze() {
	//set seed for random
	srand((int)time(NULL));
	//add the starting cell to the memorized map
	std::vector<MazeCell> firstRow;
	MazeCell firstCell;
	firstCell.obstacle = MazeObstacle::SPACE;
	firstRow.push_back(firstCell);
	mappedMaze.push_back(firstRow);

}

Player::~Player() {
	/* TODO:
	for(int i = 0; i < knownDimensions[0]; i++) {
		delete memorizedMaze[i];
	}
	delete memorizedMaze;
	*/
}


/**
 * If this method is called, that means the player has walked a direction, came back from the opposite side
 * and then came back to the same spot.
 * That means that the player now knows the width or height of the maze.
 * This method identifies the width/height and updates it.
 */
void Player::hitBookmark() {
	std::cout << "\t\tHIT BOOKMARK" << std::endl;
	Direction lastDirection = path.top();
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &bookmarkRow = bookmarkLoc[0];
	int &bookmarkCol = bookmarkLoc[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];

	if (!directionChosen) return;
	//choose another direction
	directionChosen = false;

	if (lastDirection == Direction::UP || lastDirection == Direction::DOWN) {
		if (heightKnown) return;
		heightKnown = true;
		int tempRows = rows;
		std::cout << "=======" << "curRow = " << curRow << std::endl;
		rows = std::abs(curRow - bookmarkRow) - 1;
		std::cout << "(*) rows = " << rows << std::endl;
		int rowsToErase = tempRows - rows;
		if (rowsToErase == 0) rowsToErase++;
		std::cout << "(**) rowsToErase = " << rowsToErase << std::endl;
		//fix coordinates
		std::cout << "(***) bookmarkRow = " << bookmarkRow << std::endl;
		if (lastDirection == Direction::UP) {
			bookmarkRow = bookmarkRow - rowsToErase;
			curRow = bookmarkRow;
			std::cout << "bookmarkRow = " << bookmarkRow << std::endl;
			//remove the $rowsToErase highest rows
			for (int i = rowsToErase; i > 0; i--) {
				for (int j = 0; j <= cols; j++) {
					if (i < rowsToErase && mappedMaze.at(0).at(j).obstacle != MazeObstacle::UNKNOWN) {
						mappedMaze.at(mappedMaze.size() - i).at(j) = mappedMaze.at(0).at(j);
					}
				}
				mappedMaze.erase(mappedMaze.begin());
			}
		}
		else { //Direction::DOWN
			if (bookmarkRow > rows) bookmarkRow -= rowsToErase;
			curRow = bookmarkRow;
			std::cout << "bookmarkRow = " << bookmarkRow << std::endl;
			//remove the $rowsToErase lowest rows
			for (int i = rowsToErase; i > 0; i--) {
				for (int j = 0; j <= cols; j++) {
					if (i < rowsToErase && mappedMaze.at(mappedMaze.size() - 1).at(j).obstacle != MazeObstacle::UNKNOWN) {
						mappedMaze.at(i - 1).at(j) = mappedMaze.at(mappedMaze.size() - 1).at(j);
					}
				}
				mappedMaze.erase(mappedMaze.end() - 1);
			}
		}
	}
	else {
		if (widthKnown) return;
		widthKnown = true;
		int tempCols = cols;
		cols = std::abs(curCol - bookmarkCol) - 1;
		std::cout << "(*) cols = " << cols << std::endl;
		int colsToErase = tempCols - cols;
		if (colsToErase == 0) colsToErase++;
		std::cout << "(**) colsToErase = " << colsToErase << std::endl;
		if (lastDirection == Direction::LEFT) {
			bookmarkCol = bookmarkCol - colsToErase;
			std::cout << "(***) bookmarkCol = " << bookmarkCol << std::endl;
			curCol = bookmarkCol;
			//remove the $colsToErase leftmost columns
			for (int j = 0; j <= rows; j++) {
				std::vector<MazeCell> &iRow = mappedMaze.at(j);
				for (int i = colsToErase; i > 0; i--) {
					if (i < colsToErase && iRow.at(0).obstacle != MazeObstacle::UNKNOWN) {
						iRow.at(iRow.size() - i) = iRow.at(0);
					}
					iRow.erase(iRow.begin());
				}
			}
		}
		else { //Direction::RIGHT
			if (bookmarkCol > cols) bookmarkCol -= colsToErase;
			std::cout << "(***) bookmarkCol = " << bookmarkCol << std::endl;
			curCol = bookmarkCol;
			//remove the $colsToErase rightmost columns
			for (int j = 0; j <= rows; j++) {
				std::vector<MazeCell> &iRow = mappedMaze.at(j);
				for (int i = colsToErase; i > 0; i--) {
					if (i < colsToErase && iRow.at(iRow.size() - 1).obstacle != MazeObstacle::UNKNOWN) {
						iRow.at(i - 1) = iRow.at(iRow.size() - 1);
					}
					iRow.erase(iRow.end() - 1);
				}
			}
		}
	}
}


/**
 *
 */
void Player::hitWall() {
	std::cout << "\t\tHIT WALL" << std::endl;
	// Cancel the last location update.
	////std::cout << "hitWall(): 1" << std::endl;
	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	cell.obstacle = MazeObstacle::WALL;
	directionChosen = false;
	Direction lastDir = path.top();
	std::cout << "(****) lastDir = " << lastDir << std::endl;
	undoMove(lastDir);
	path.pop();
	if (curLocation[0] != bookmarkLoc[0] || curLocation[1] != bookmarkLoc[1]) { //check if the bookmark is at the same location as the player, and place it if it isn't
		placedBookmark = false;
	}
}


/*
bool Player::checkDirection(Direction dir) {
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];
	int i;
	switch(dir) {
	case Direction::LEFT:
		if
		i = curCol;
		do {
			if(mappedMaze[curRow][i].obstacle == MazeObstacle::WALL) return false;
			if(i == 0 && widthKnown) i = cols;
			else i--;
		}
		while(i != curCol && i > 0);
		break;
	case Direction::RIGHT:
		i = curCol;
		do {
			if(mappedMaze[curRow][i].obstacle == MazeObstacle::WALL) return false;
			if(i == cols && widthKnown) i = 0;
			else i++;
		}
		while(i != curCol && i <= cols);
		break;
	case Direction::UP:
		i = curRow;
		do {
			if(mappedMaze[i][curCol].obstacle == MazeObstacle::WALL) return false;
			if(i == 0 && heightKnown) i = rows;
			else i--;
		}
		while(i != curRow && i > 0);
		break;
	case Direction::DOWN:
		i = curRow;
		do {
			if(mappedMaze[i][curCol].obstacle == MazeObstacle::WALL) return false;
			if(i == rows && heightKnown) i = 0;
			else i++;
		}
		while(i != curRow && i <= rows);
		break;
	default:
		break;
	}
	return true;
}
*/

/**
 *
 */
Direction Player::move() {
	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	////updateAvailableDirections(curLocation[0], curLocation[1]);
	// change current cell.obstacle to SPACE, as the player is on it
	if (cell.obstacle == MazeObstacle::UNKNOWN) {
		cell.obstacle = MazeObstacle::SPACE;
	}

	Direction nextDirection;
	std::cout << "move(): 0" << std::endl;
	std::string lDir = lastDirection == Direction::RIGHT ? "RIGHT" : (lastDirection == Direction::LEFT ? "LEFT" : (lastDirection == Direction::UP ? "UP" : (lastDirection == Direction::DOWN ? "DOWN" : "BOOKMARK")));
	std::cout << "lastDirection = " << lDir << std::endl;

	if (!path.empty() && path.top() != Direction::BOOKMARK) {
		switch (path.top()) {
		case Direction::LEFT:
			if (!cell.triedDirection[1]) cell.numOfLocsTried++;
			cell.triedDirection[1] = true; //Direction::RIGHT
			break;
		case Direction::RIGHT:
			if (!cell.triedDirection[0]) cell.numOfLocsTried++;
			cell.triedDirection[0] = true; //Direction::LEFT
			break;
		case Direction::UP:
			if (!cell.triedDirection[3]) cell.numOfLocsTried++;
			cell.triedDirection[3] = true; //Direction::DOWN
			break;
		case Direction::DOWN:
			if (!cell.triedDirection[2]) cell.numOfLocsTried++;
			cell.triedDirection[2] = true; //Direction::UP
			break;
		default:
			break;
		}

	}
	//place a bookmark if the player is not at the same cell as the bookmark, he haven't placed one and he isn't backtracking
	if ((bookmarkLoc[0] != curLocation[0] || bookmarkLoc[1] != curLocation[1]) && !placedBookmark && cell.numOfLocsTried < 4) {
		std::cout << "move(): 1" << std::endl;
		placedBookmark = true;
		bookmarkLoc[0] = curLocation[0];
		bookmarkLoc[1] = curLocation[1];
		nextDirection = Direction::BOOKMARK;
	}
	else {
		int chosenDir = 0;
		if (directionChosen) { //continue along the same direction
			nextDirection = path.top();
			chosenDir = nextDirection;
			if (!cell.triedDirection[chosenDir]) {
				cell.triedDirection[chosenDir] = true;
				cell.numOfLocsTried++;
			}
		}
		else if (cell.numOfLocsTried < 4) { //choose a random direction that wasn't tried yet
			////std::cout << "move(): 1" << std::endl;
			int numAvailable = 4 - cell.numOfLocsTried;
			std::cout << "numAvailable = " << numAvailable << std::endl;
			int randNum = rand() % numAvailable;
			for (; randNum >= 0; chosenDir++) {
				if (!cell.triedDirection[chosenDir]) randNum--;
			}
			chosenDir--;
			std::cout << "chosenDir = " << chosenDir << std::endl;
			nextDirection = Direction(chosenDir);
			if (cell.triedDirection[chosenDir]) {
				std::cout << "ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT ALERT" << std::endl;
			}
			////std::cout << "move(): 5" << std::endl;
			cell.numOfLocsTried++;
			cell.triedDirection[chosenDir] = true;
			directionChosen = true;
		}
		else { //backtrack the walked path
			std::cout << "!!!!!!backtracking!!!!!!" << std::endl;
			directionChosen = false;
			placedBookmark = false;
			switch (path.top()) {
			case Direction::LEFT:
				nextDirection = Direction::RIGHT;
				break;
			case Direction::RIGHT:
				nextDirection = Direction::LEFT;
				break;
			case Direction::UP:
				nextDirection = Direction::DOWN;
				break;
			case Direction::DOWN:
				nextDirection = Direction::UP;
				break;
			default:
				break;
			}
			path.pop();
		}
	}
	if (nextDirection != Direction::BOOKMARK) {
		updateLocation(nextDirection);
		if (directionChosen) {
			std::string nDir = nextDirection == Direction::RIGHT ? "RIGHT" : (nextDirection == Direction::LEFT ? "LEFT" : (nextDirection == Direction::UP ? "UP" : (nextDirection == Direction::DOWN ? "DOWN" : "BOOKMARK")));
			path.push(nextDirection);
			updateMap();
		}
	}
	lastDirection = nextDirection;
	std::cout << "cell.numOfLocsTried: " << cell.numOfLocsTried << std::endl;
	return nextDirection;
}


/**
 *
 */
void Player::undoMove(Direction dir) {
	switch (dir) {
	case Direction::LEFT:
		updateLocation(Direction::RIGHT);
		break;
	case Direction::RIGHT:
		updateLocation(Direction::LEFT);
		break;
	case Direction::UP:
		updateLocation(Direction::DOWN);
		break;
	case Direction::DOWN:
		updateLocation(Direction::UP);
		break;
	default: //BOOKMARK. do nothing as it cannot get here.
		break;
	}
}

/**
 * Updates the player's location.
 */
void Player::updateLocation(Direction dir) {
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];
	switch (dir) {
	case Direction::UP:
		if (heightKnown && curRow == 0) {
			curRow = rows;
		}
		else {
			curRow--;
		}
		break;
	case Direction::DOWN:
		if (heightKnown && curRow == rows) {
			curRow = 0;
		}
		else {
			curRow++;
		}
		break;
	case Direction::LEFT:
		if (widthKnown && curCol == 0) {
			curCol = cols;
		}
		else {
			curCol--;
		}
		break;
	case Direction::RIGHT:
		if (widthKnown && curCol == cols) {
			curCol = 0;
		}
		else {
			curCol++;
		}
		break;
	default: //BOOKMARK. do nothing as it cannot get there.
		break;
	}
}

void Player::updateMap() {
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];
	std::vector<MazeCell> newRow;
	// the player went lower than previously known
	if (curRow > rows) {
		rows++;
		// add the number of known columns to the row.
		for (int i = 0; i <= cols; i++) {
			newRow.push_back(MazeCell());
		}
		//insert row at the end (the lowest row)
		mappedMaze.push_back(newRow);
	}

	// the player went higher than previously known
	else if (curRow < 0) {
		rows++;
		//adjust location so there won't be any negative coordinations
		curRow++;
		bookmarkLoc[0]++;
		// add the number of known columns to the row
		for (int i = 0; i <= cols; i++) {
			newRow.push_back(MazeCell());
		}
		//insert row at the beginning (the highest row)
		mappedMaze.insert(mappedMaze.begin(), newRow);
	}

	// the player went more right than previously known
	else if (curCol > cols) { // column needs to be added
		cols++;
		// add a cell at the end of each row
		for (int i = 0; i <= rows; i++) {
			//insert cell at the end of the row (the rightmost cell)
			mappedMaze[i].push_back(MazeCell());
		}
	}

	// the player went more left than previously known
	else if (curCol < 0) {
		cols++;
		//adjust location so there won't be any negative coordinations
		curCol++;
		bookmarkLoc[1]++;
		for (int i = 0; i <= rows; i++) {

			std::vector<MazeCell> &iRow = mappedMaze.at(i);
			//insert cell at the beginning of the row (the leftmost cell)
			iRow.insert(iRow.begin(), MazeCell());
		}
	}
}


void Player::printMaze() {
	std::cout << "\t\tPrinted Maze:" << std::endl;
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];
	std::cout << "\t\trows = " << rows << ", cols = " << cols << std::endl;
	std::cout << "\t\tcurRow = " << curRow << ", curCol = " << curCol << std::endl;
	for (int i = 0; i <= rows; i++) {
		std::cout << "\t\t";
		for (int j = 0; j <= cols; j++) {
			MazeCell cell = mappedMaze[i][j];
			if (i == curRow && j == curCol) {
				std::cout << "@";
				continue;
			}
			if (i == bookmarkLoc[0] && j == bookmarkLoc[1]) {
				std::cout << "B";
				continue;
			}
			switch (cell.obstacle) {
			case MazeObstacle::SPACE:
				std::cout << "_";
				break;
			case MazeObstacle::WALL:
				std::cout << "#";
				break;

			case MazeObstacle::UNKNOWN:
				std::cout << "?";
				break;
			default:
				std::cout << "PROBLEM";
			}
		}
		std::cout << std::endl;
	}
}
