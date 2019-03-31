#include "Player.h"


/*******************************/
/******* CLASS: MazeCell *******/
/*******************************/

//Constructor
MazeCell::MazeCell() : numOfDirsTried(0), obstacle(MazeObstacle::UNKNOWN) {
	for (int i = 0; i < 4; i++) {
		triedDirection[i] = false;
	}

}

MazeCell::~MazeCell() {}

/**
 * Copy constructor
 * Copies each field by value
 */
MazeCell::MazeCell(const MazeCell &copied) {
	numOfDirsTried = copied.numOfDirsTried;
	obstacle = copied.obstacle;
	for (int i = 0; i < 4; i++) {
		triedDirection[i] = copied.triedDirection[i];
	}
}



/*******************************/
/******** CLASS: Player ********/
/*******************************/

//Constructor
Player::Player() : mappedMaze(), widthKnown(false), heightKnown(false), placedBookmark(false), directionChosen(false) {
	//set seed for random
	srand((int)time(NULL));
	//add the starting cell to the memorized map
	std::vector<MazeCell> firstRow;
	MazeCell firstCell;
	firstCell.obstacle = MazeObstacle::SPACE;
	firstRow.push_back(firstCell);
	mappedMaze.push_back(firstRow);
}

//No memory is needed to be freed
Player::~Player() {}


/**
 * Fixes the player's location, according to the value of dir.
 * For example: if dir==Direction::LEFT -- it moves the player one cell to the right.
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
 * Updates the player's location according to the given direction.
 * Takes into consideration if the width/height are known and updates the location accordingly.
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


/**
 * Updates mappedMaze according to the player's updated location.
 * Also updates the known dimensions, and the player's and bookmark's coordinates according to the updated map.
 * NOTE: this method should ALWAYS be used after a updateLocation() has been invoked.
 */
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


/*
 * Updates the current cell's triedDirection and numOfTriedDirs according to the direction the player came from
 */
void Player::updateTriedFromOrigin(MazeCell &cell) {
	if (!path.empty() && path.top() != Direction::BOOKMARK) {
		switch (path.top()) {
		case Direction::LEFT:
			if (!cell.triedDirection[1]) cell.numOfDirsTried++;
			cell.triedDirection[1] = true; //Direction::RIGHT
			break;
		case Direction::RIGHT:
			if (!cell.triedDirection[0]) cell.numOfDirsTried++;
			cell.triedDirection[0] = true; //Direction::LEFT
			break;
		case Direction::UP:
			if (!cell.triedDirection[3]) cell.numOfDirsTried++;
			cell.triedDirection[3] = true; //Direction::DOWN
			break;
		case Direction::DOWN:
			if (!cell.triedDirection[2]) cell.numOfDirsTried++;
			cell.triedDirection[2] = true; //Direction::UP
			break;
		default:
			break;
		}
	}
}



// PUBLIC METHODS

/**
 * If this method is called, that means the player has walked a direction, came back from the opposite side
 * and then came back to the same spot, or he is backtracking (in this case -- do nothing)
 * That means that the player now knows the width or height of the maze.
 * This method identifies the width/height and updates it.
 */
void Player::hitBookmark() {
	Direction lastDirection = path.top();
	int &rows = knownDimensions[0];
	int &cols = knownDimensions[1];
	int &bookmarkRow = bookmarkLoc[0];
	int &bookmarkCol = bookmarkLoc[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];

	if (!directionChosen) return; //there's no need to do anything, as the player is only backtracking along the walked path

	//choose another direction
	directionChosen = false;


	//Fix the dimensions of the map, erase the needed rows or columns, update the player's and bookmark's location

	if (lastDirection == Direction::UP || lastDirection == Direction::DOWN) {
		if (heightKnown) return;
		heightKnown = true;
		int tempRows = rows;
		//update the number of rows the maze has
		rows = std::abs(curRow - bookmarkRow) - 1;
		int rowsToErase = tempRows - rows;
		if (rowsToErase == 0) rowsToErase++;
		if (lastDirection == Direction::UP) {
			bookmarkRow = bookmarkRow - rowsToErase;
			//move player to the updated bookmark's location
			curRow = bookmarkRow;
			//copy and remove the $rowsToErase highest rows
			for (int i = rowsToErase; i > 0; i--) {
				//copy the highest rows to the lowest rows, in order
				for (int j = 0; j <= cols; j++) {
					//do not copy the first row (it is the same as the bookmark's row), and copy only cells which are not unknown
					if (i < rowsToErase && mappedMaze.at(0).at(j).obstacle != MazeObstacle::UNKNOWN) {
						mappedMaze.at(mappedMaze.size() - i).at(j) = mappedMaze.at(0).at(j);
					}
				}
				mappedMaze.erase(mappedMaze.begin());
			}
		}
		else { //Direction::DOWN
			if (bookmarkRow > rows) bookmarkRow -= rowsToErase;
			//move player to the updated bookmark's location
			curRow = bookmarkRow;
			//copy remove the $rowsToErase lowest rows
			for (int i = rowsToErase; i > 0; i--) {
				//copy the lowest rows to the highest rows, in order
				for (int j = 0; j <= cols; j++) {
					//do not copy the last row (it is the same as the bookmark's row), and copy only cells which are not unknown
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
		//update the number of cols the maze has
		cols = std::abs(curCol - bookmarkCol) - 1;
		int colsToErase = tempCols - cols;
		if (colsToErase == 0) colsToErase++;
		if (lastDirection == Direction::LEFT) {
			bookmarkCol = bookmarkCol - colsToErase;
			//move player to the updated bookmark's location
			curCol = bookmarkCol;
			//copy and remove the $colsToErase leftmost columns
			for (int j = 0; j <= rows; j++) {
				std::vector<MazeCell> &iRow = mappedMaze.at(j);
				//copy the leftmost columns to the rightmost columns, in order
				for (int i = colsToErase; i > 0; i--) {
					//do not copy the first column (it is the same as the bookmark's column), and copy only cells which are not unknown
					if (i < colsToErase && iRow.at(0).obstacle != MazeObstacle::UNKNOWN) {
						iRow.at(iRow.size() - i) = iRow.at(0);
					}
					iRow.erase(iRow.begin());
				}
			}
		}
		else { //Direction::RIGHT
			if (bookmarkCol > cols) bookmarkCol -= colsToErase;
			//move player to the updated bookmark's location
			curCol = bookmarkCol;
			//copy and remove the $colsToErase rightmost columns
			for (int j = 0; j <= rows; j++) {
				std::vector<MazeCell> &iRow = mappedMaze.at(j);
				//copy the rightmost columns to the leftmost columns, in order
				for (int i = colsToErase; i > 0; i--) {
					//do not copy the last column (it is the same as the bookmark's column), and copy only cells which are not unknown
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
 * Cancels the last direction change.
 * Updates the cell's obstacle to MazeObstacle::WALL, removes the last direction change from path, and fixes the player's location.
 */
void Player::hitWall() {
	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	cell.obstacle = MazeObstacle::WALL;
	directionChosen = false;
	Direction lastDir = path.top();
	undoMove(lastDir);
	path.pop();
	if (curLocation[0] != bookmarkLoc[0] || curLocation[1] != bookmarkLoc[1]) { //check if the bookmark is at the same location as the player, and place it if it isn't
		placedBookmark = false;
	}
}



/**
 *
 */
Direction Player::move() {
	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	Direction nextDirection;

	// change current cell.obstacle to SPACE, as the player is on it
	if (cell.obstacle == MazeObstacle::UNKNOWN) {
		cell.obstacle = MazeObstacle::SPACE;
	}

	updateTriedFromOrigin(cell);

	//place a bookmark if the player is not at the same cell as the bookmark, he haven't placed one and he isn't backtracking
	if ((bookmarkLoc[0] != curLocation[0] || bookmarkLoc[1] != curLocation[1]) && !placedBookmark && cell.numOfDirsTried < 4) {
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
				cell.numOfDirsTried++;
			}
		}
		else if (cell.numOfDirsTried < 4) { //choose a random direction that wasn't tried yet
			int numAvailable = 4 - cell.numOfDirsTried;
			int randNum = rand() % numAvailable;

			for (; randNum >= 0; chosenDir++) {
				if (!cell.triedDirection[chosenDir]) randNum--;
			}
			chosenDir--;

			nextDirection = Direction(chosenDir);
			directionChosen = true;
			//update the cell's fields
			cell.numOfDirsTried++;
			cell.triedDirection[chosenDir] = true;
		}
		else { //backtrack along the walked path
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

	//update the player's location, add direction to path, and update map. do all of this only if the next direction is not a bookmark
	if (nextDirection != Direction::BOOKMARK) {
		updateLocation(nextDirection);
		if (directionChosen) {
			path.push(nextDirection);
			updateMap();
		}
	}

	return nextDirection;
}