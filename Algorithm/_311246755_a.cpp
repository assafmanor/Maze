#include "_311246755_a.h"


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
_311246755_a::_311246755_a() : mappedMaze(), isWidthKnown(false), isHeightKnown(false), placedBookmark(false), directionChosen(false) {
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
_311246755_a::~_311246755_a() {}


/**
 * Fixes the player's location, according to the value of dir.
 * For example: if dir==Direction::LEFT -- it moves the player one cell to the right.
 */
void _311246755_a::undoMove(const Move dir) {
	switch (dir) {
	case Move::LEFT:
		updateLocation(Move::RIGHT);
		break;
	case Move::RIGHT:
		updateLocation(Move::LEFT);
		break;
	case Move::UP:
		updateLocation(Move::DOWN);
		break;
	case Move::DOWN:
		updateLocation(Move::UP);
		break;
	default: //BOOKMARK. do nothing as it cannot get here.
		break;
	}
}

/**
 * Updates the player's location according to the given direction.
 * Takes into consideration if the width/height are known and updates the location accordingly.
 */
void _311246755_a::updateLocation(const Move dir) {
	const int &rows = knownDimensions[0];
	const int &cols = knownDimensions[1];
	int &curRow = curLocation[0];
	int &curCol = curLocation[1];
	switch (dir) {
	case Move::UP:
		if (isHeightKnown && curRow == 0) {
			curRow = rows;
		}
		else {
			curRow--;
		}
		break;
	case Move::DOWN:
		if (isHeightKnown && curRow == rows) {
			curRow = 0;
		}
		else {
			curRow++;
		}
		break;
	case Move::LEFT:
		if (isWidthKnown && curCol == 0) {
			curCol = cols;
		}
		else {
			curCol--;
		}
		break;
	case Move::RIGHT:
		if (isWidthKnown && curCol == cols) {
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
void _311246755_a::updateMap() {
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
void _311246755_a::updateTriedFromOrigin(MazeCell &cell) {
	if (!path.empty() && path.top() != Move::BOOKMARK) {
		switch (path.top()) {
		case Move::LEFT:
			if (!cell.triedDirection[1]) cell.numOfDirsTried++;
			cell.triedDirection[1] = true; //Direction::RIGHT
			break;
		case Move::RIGHT:
			if (!cell.triedDirection[0]) cell.numOfDirsTried++;
			cell.triedDirection[0] = true; //Direction::LEFT
			break;
		case Move::UP:
			if (!cell.triedDirection[3]) cell.numOfDirsTried++;
			cell.triedDirection[3] = true; //Direction::DOWN
			break;
		case Move::DOWN:
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
void _311246755_a::hitBookmark(int seq) {

	if (seq > 100000000) {
		std::cout << "just for debugging" << std::endl;
	}

	/*TODO: use seq*/


	const Move lastDirection = path.top();
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

	if (lastDirection == Move::UP || lastDirection == Move::DOWN) {
		if (isHeightKnown) return;
		isHeightKnown = true;
		int tempRows = rows;
		//update the number of rows the maze has
		rows = std::abs(curRow - bookmarkRow) - 1;
		int rowsToErase = tempRows - rows;
		if (rowsToErase == 0) rowsToErase++;
		if (lastDirection == Move::UP) {
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
		if (isWidthKnown) return;
		isWidthKnown = true;
		const int tempCols = cols;
		//update the number of cols the maze has
		cols = std::abs(curCol - bookmarkCol) - 1;
		int colsToErase = tempCols - cols;
		if (colsToErase == 0) colsToErase++;
		if (lastDirection == Move::LEFT) {
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
void _311246755_a::hitWall() {
	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	cell.obstacle = MazeObstacle::WALL;
	directionChosen = false;
	Move lastDir = path.top();
	undoMove(lastDir);
	path.pop();
	if (curLocation[0] != bookmarkLoc[0] || curLocation[1] != bookmarkLoc[1]) { //check if the bookmark is at the same location as the player, and place it if it isn't
		placedBookmark = false;
	}
}



/**
 *
 */
AbstractAlgorithm::Move _311246755_a::move() {

	/*TODO: use bookmarks more often*/
	


	MazeCell &cell = mappedMaze[curLocation[0]][curLocation[1]];
	Move nextDirection;

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
		nextDirection = Move::BOOKMARK;
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

			nextDirection = Move(chosenDir);
			directionChosen = true;
			//update the cell's fields
			cell.numOfDirsTried++;
			cell.triedDirection[chosenDir] = true;
		}
		else { //backtrack along the walked path
			directionChosen = false;
			placedBookmark = false;
			switch (path.top()) {
			case Move::LEFT:
				nextDirection = Move::RIGHT;
				break;
			case Move::RIGHT:
				nextDirection = Move::LEFT;
				break;
			case Move::UP:
				nextDirection = Move::DOWN;
				break;
			case Move::DOWN:
				nextDirection = Move::UP;
				break;
			default:
				break;
			}
			path.pop();
		}
	}

	//update the player's location, add direction to path, and update map. do all of this only if the next direction is not a bookmark
	if (nextDirection != Move::BOOKMARK) {
		updateLocation(nextDirection);
		if (directionChosen) {
			path.push(nextDirection);
			updateMap();
		}
	}

	return nextDirection;
}