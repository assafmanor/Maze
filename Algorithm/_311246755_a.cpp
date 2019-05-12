#include "_311246755_a.h"


REGISTER_ALGORITHM(_311246755_a)


/*******************************/
/******* CLASS: MazeCell *******/
/*******************************/

//Constructors
_311246755_a::MazeCell::MazeCell(MazeObstacle _obstacle) : hasBookmark(false), numOfDirsTried(0), obstacle(_obstacle) {
		triedDirection[Move::UP] = false;
		triedDirection[Move::DOWN] = false;
		triedDirection[Move::LEFT] = false;
		triedDirection[Move::RIGHT] = false;
}

_311246755_a::MazeCell::MazeCell() : hasBookmark(false), numOfDirsTried(0), obstacle(MazeObstacle::UNKNOWN) {
	for (int i = 0; i < 4; i++) {
		triedDirection[Move::UP] = false;
		triedDirection[Move::DOWN] = false;
		triedDirection[Move::LEFT] = false;
		triedDirection[Move::RIGHT] = false;
	}
}


_311246755_a::MazeCell::~MazeCell() {}


/**
 * Copy constructor
 * Copies each field by value
 */
_311246755_a::MazeCell::MazeCell(const MazeCell& copied) {
	numOfDirsTried = copied.numOfDirsTried;
	obstacle = copied.obstacle;
	auto copiedTriedDirections = copied.triedDirection;
	triedDirection[Move::UP] = copiedTriedDirections[Move::UP];
	triedDirection[Move::DOWN] = copiedTriedDirections[Move::DOWN];
	triedDirection[Move::LEFT] = copiedTriedDirections[Move::LEFT];
	triedDirection[Move::RIGHT] = copiedTriedDirections[Move::RIGHT];
}


/*******************************/
/******** CLASS: Player ********/
/*******************************/

//Constructor
_311246755_a::_311246755_a() :
	isWidthKnown(false), isHeightKnown(false), placedBookmark(false), isDirectionChosen(false),
	leftmostKnownCol(0), rightmostKnownCol(0), lowestKnownRow(0), highestKnownRow(0) {
	//set seed for random
	srand((int)time(NULL));
	//add the starting cell to the memorized map
	addNewCell(0, 0, MazeObstacle::SPACE);
}


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
 * Calculates the next location the player will be at, according to the given move, the known maze dimentions and the player's current location.
 */
std::pair <int, int> _311246755_a::calcNextLocation(const Move move) {
	int nextRow = curRow;
	int nextCol = curCol;
	switch (move) {
	case Move::UP:
		if (isHeightKnown && curRow == highestKnownRow) nextRow = lowestKnownRow;
		else nextRow--;
		break;
	case Move::DOWN:
		if (isHeightKnown && curRow == lowestKnownRow) nextRow = highestKnownRow;
		else nextRow++;
		break;
	case Move::LEFT:
		if (isWidthKnown && curCol == leftmostKnownCol) nextCol = rightmostKnownCol;
		else nextCol--;
		break;
	case Move::RIGHT:
		if (isWidthKnown && curCol == rightmostKnownCol) nextCol = leftmostKnownCol;
		else nextCol++;
		break;
	default: //BOOKMARK
		break;
	}
	return { nextRow, nextCol };
}


/**
 * Updates the player's location according to the given direction.
 * Takes into consideration if the width/height are known and updates the location accordingly.
 */
void _311246755_a::updateLocation(const Move move) {
	auto nextLocation = calcNextLocation(move);
	curRow = nextLocation.first;
	curCol = nextLocation.second;
}


/**
 * Adds a new cell at location to the mapped maze, given only an obstacle.
 */
void _311246755_a::addNewCell(const int row, const int col, const MazeObstacle obstacle) {
	MazeCell cell = MazeCell(obstacle);
	addNewCell(row, col, cell);
}


/**
 * Adds a new cell at location to the mapped maze, given an existing cell.
 */
void _311246755_a::addNewCell(const int row, const int col, MazeCell &cell) {
	mappedMaze[row].insert(std::make_pair(col, cell));
}


/**
 * Updates the obstacle of an existing cell - mappedMaze[row][col] to newObstacle.
 */
void _311246755_a::updateCellObstacle(const int row, const int col, MazeObstacle newObstacle) {
	MazeCell& cell = mappedMaze[row][col];
	cell.obstacle = newObstacle;
}


/**
 * Updates the current cell's triedDirection and numOfTriedDirs according to the direction the player came from
 */
void _311246755_a::updateTriedFromOrigin(_311246755_a::MazeCell& cell) {
	if (!path.empty()) {
		switch (path.top()) {
		case Move::LEFT:
			if (!cell.triedDirection[Move::RIGHT]) cell.numOfDirsTried++;
			cell.triedDirection[Move::RIGHT] = true;
			break;
		case Move::RIGHT:
			if (!cell.triedDirection[Move::LEFT]) cell.numOfDirsTried++;
			cell.triedDirection[Move::LEFT] = true;
			break;
		case Move::UP:
			if (!cell.triedDirection[Move::DOWN]) cell.numOfDirsTried++;
			cell.triedDirection[Move::DOWN] = true;
			break;
		case Move::DOWN:
			if (!cell.triedDirection[Move::UP]) cell.numOfDirsTried++;
			cell.triedDirection[Move::UP] = true;
			break;
		default:
			break;
		}
	}
}


/**
 * Chooses a random direction from the directions not yet tried from the cell.
 */
AbstractAlgorithm::Move _311246755_a::chooseRandomDirection(MazeCell &cell) {
	int chosenMoveNum = 0;
	int numAvailable = 4 - cell.numOfDirsTried;
	int randNum = rand() % numAvailable;
	auto it = cell.triedDirection.begin();
	for (; randNum >= 0; chosenMoveNum++, ++it) {
		bool triedMov = it->second;
		if (!triedMov) randNum--;
	}
	--it;

	Move nextMove = it->first;
	//update the cell's fields
	cell.numOfDirsTried++;
	cell.triedDirection[nextMove] = true;
	return nextMove;
}


/**
 * Returns the opposite direction to the last direction the player moved.
 */
AbstractAlgorithm::Move _311246755_a::backtrackMove() {
	Move lastMove = path.top();
	path.pop(); //remove the last move from path
	switch (lastMove) {
	case Move::LEFT:
		return Move::RIGHT;
	case Move::RIGHT:
		return Move::LEFT;
	case Move::UP:
		return Move::DOWN;
	case Move::DOWN:
		return Move::UP;
	default: //shouldn't get here
		return lastMove;
	}
}


/**
 * Chooses the next move direction.
 */
AbstractAlgorithm::Move _311246755_a::chooseMove() {
	MazeCell &cell = mappedMaze[curRow][curCol];
	Move nextMove;
	//place a bookmark if the player is not at a bookmark, he hasn't placed one 
	if (!isDirectionChosen && !cell.hasBookmark && !placedBookmark) {
		cell.hasBookmark = true;
		placedBookmark = true;
		return Move::BOOKMARK;
	}

	if (isDirectionChosen) { //continue along the same direction
		nextMove = path.top();
		cell.triedDirection[nextMove] = true;
		isBacktracking = false;
		if (!cell.triedDirection[nextMove]) {
			cell.numOfDirsTried++;
		}
	}
	else if (cell.numOfDirsTried < 4) { //choose a random direction that wasn't tried yet
		isBacktracking = false;
		isDirectionChosen = true;
		nextMove = chooseRandomDirection(cell);
	}
	else { //backtrack along the walked path
		isBacktracking = true;
		placedBookmark = false;
		nextMove = backtrackMove();
	}

	return nextMove;
}


/**
 * Updates the highest/lowest/leftmost/rightmost known row/col according to the move and current location.
 */
void _311246755_a::updateKnownDimentions(Move move) {
	switch (move) {
	case Move::UP:
		if (!isHeightKnown && curRow < highestKnownRow) highestKnownRow = curRow;
		return;
	case Move::DOWN:
		if (!isHeightKnown && curRow > lowestKnownRow) lowestKnownRow = curRow;
		return;
	case Move::LEFT:
		if (!isWidthKnown && curCol < leftmostKnownCol) leftmostKnownCol = curCol;
		return;
	case Move::RIGHT:
		if (!isWidthKnown && curCol > rightmostKnownCol) rightmostKnownCol = curCol;
		return;
	default: //shouldn't get here
		return;
	}
}


/**
 * Ascertains the true height of the maze, and fixes the value of the highest known rows of the maze.
 * Returns the true height of the maze
 */
int _311246755_a::setHeightBounds(int seq) {
	const int firstRowVal = bookmarks.at(seq - 1).first; //the row of the bookmark
	const int secondRowVal = curRow; //the row of the current location
	//both coordinates are actually the same, and so they need to be fixed

	assert (firstRowVal != secondRowVal); //shouldn't get here if that's the situation
	isHeightKnown = true;

	const int mazeHeight = std::abs(firstRowVal - secondRowVal);
	
	//fix the lowest known rows according to the newly discovered real height of the maze
	highestKnownRow = lowestKnownRow - mazeHeight + 1;

	return mazeHeight;
}


/**
 * Ascertains the true width of the maze, and fixes the value of the rightmost known colums of the maze.
 * Returns the true width of the maze
 */
int _311246755_a::setWidthBounds(int seq) {
	const int &firstColVal = bookmarks.at(seq - 1).second; //the column of the bookmark
	const int &secondColVal = curCol; //the column of the current location
	//both coordinates are actually the same, and so they need to be fixed

	assert (firstColVal == secondColVal); //shouldn't get here if that's the situation
	isWidthKnown = true;

	const int mazeWidth = std::abs(firstColVal - secondColVal);

	//fix the leftmost known columns according to the newly discovered real width of the maze
	leftmostKnownCol = rightmostKnownCol - mazeWidth + 1;

	return mazeWidth;
}


/**
 * Unifies the information gathered from both cells to the receiver cell.
 */
void _311246755_a::unifyCells(MazeCell &receiver, MazeCell &giver) {
	if (giver.hasBookmark) {
		receiver.hasBookmark = true;
	}
	if (receiver.obstacle == MazeObstacle::UNKNOWN) {
		receiver.obstacle = giver.obstacle;
	}
	for (int i = Move::UP; i < Move::LEFT; i++) {
		Move mov = static_cast<Move>(i);
		if (!receiver.triedDirection[mov] && giver.triedDirection[mov]) {
			receiver.triedDirection[mov] = true;
			receiver.numOfDirsTried++;
		}
	}
}


/**
 * Unifies all the information found in all the cells present in both rows, and stores the unified information inside the receiver row.
 */
void _311246755_a::unifyRows(std::map <int, MazeCell> receiver, std::map <int, MazeCell> giver, const int rowNumOfReceiver) {
	for (auto cellIt : giver) {
		const int curColNum = cellIt.first;
		MazeCell &giverCell = cellIt.second;
		auto receiverCellIt = receiver.find(curColNum);
		if (receiverCellIt == receiver.end()) { //cell not found
			addNewCell(rowNumOfReceiver, curColNum, giverCell);
		}
		else { //duplicate cell found
			unifyCells(receiverCellIt->second, giverCell);
		}
	}
}


/**
 * Moves the information gathered from cells with wrong row values to the corresponding cells in mappedMaze (the ones that have the correct row value).
 * The information is then unified with the information already gathered by the correct cell, or a new cell is created.
 */
void _311246755_a::fixRows(const int mazeHeight) {
	int curRowNum;
	for (auto tooHighRowIt : mappedMaze) {
		curRowNum = tooHighRowIt.first;
		int correctRowNum = findCorrectCoord(curRowNum, highestKnownRow,  mazeHeight);
		if (curRowNum >= highestKnownRow) break;
		//check if there already exists a row with the correct row number
		auto receiverRowIt = mappedMaze.find(correctRowNum);
		if (receiverRowIt != mappedMaze.end()) { //found duplicate row
			unifyRows(receiverRowIt->second, tooHighRowIt.second, curRowNum + mazeHeight);
		}
		else { //found a row that needs to get copied to the correct row
			mappedMaze[correctRowNum] = tooHighRowIt.second;
		}
	}
}



/**
 * Moves the information gathered from cells with wrong column values to the corresponding cells in mappedMaze (the ones that have the correct column value).
 * The information is then unified with the information already gathered by the correct cell, or a new cell is created.
 */
void _311246755_a::fixCols(const int mazeWidth) {
	int tooLeftColNum;
	int correctColNum;

	//do for all rows
	for (auto curRowIt : mappedMaze) {
		const int curRowNum = curRowIt.first;
		std::map <int, MazeCell> &curRow = curRowIt.second;
		for (auto tooLeftCellIt : curRow) {
			tooLeftColNum = tooLeftCellIt.first;
			correctColNum = findCorrectCoord(tooLeftColNum, leftmostKnownCol, mazeWidth);
			MazeCell& giverCell = tooLeftCellIt.second;
			if (tooLeftColNum >= leftmostKnownCol) break;
			//check if there already exists a cell in this row with the correct column number
			auto receiverCellIt = curRow.find(correctColNum);
			if (receiverCellIt != curRow.end()) { //found duplicate cell
				unifyCells(receiverCellIt->second, giverCell);
			}
			else { //found a cell that needs to get copied to the correct column
				addNewCell(curRowNum, correctColNum, giverCell);
			}
		}
	}
}


/**
 * Fixes the coordinates of all bookmarks according to the given maze width and height.
 * This method works only if exactly one of the values of mazeWidth and mazeHeight is -1 (meaning that the value is irrelevant).
 */
void _311246755_a::fixBookmarks(const int mazeWidth, const int mazeHeight) {
	int bRow, bCol;
	for (long unsigned int i = 0; i < bookmarks.size(); i++) {
		auto coords = bookmarks.at(i);
		bRow = coords.first;
		bCol = coords.second;
		if (mazeHeight != -1 && bRow < highestKnownRow) {
			int correctRow = findCorrectCoord(bRow, highestKnownRow, mazeHeight);
			bookmarks.at(i) = { correctRow, bCol };
		}
		else if (mazeWidth != -1 && bCol < leftmostKnownCol) {
			int correctCol = findCorrectCoord(bCol, leftmostKnownCol, mazeWidth);
			bookmarks.at(i) = { bRow, correctCol };
		}
	}
}


/**
 * Calculates the correct value for the given coordinate, according to the threshold and mazeDim.
 */
int _311246755_a::findCorrectCoord(const int oldCoord, const int thresh, const int mazeDim) {
	int res = oldCoord;
	while (res < thresh) {
		res += mazeDim;
	}
	return res;
}


/**
 * Returns true iff there exists a cell in mappedMaze with coordinates matching loc.
 * i.e. mappedMaze[loc.first][loc.second].
 */
bool _311246755_a::doesCellExist(std::pair <int, int> loc) {
	const int row = loc.first;
	const int col = loc.second;
	auto doesRowExist = mappedMaze.find(row);
	if (doesRowExist == mappedMaze.end()) return false;
	auto doesColExist = mappedMaze[row].find(col);
	if (doesColExist == mappedMaze[row].end()) return false;
	return true;
}



// PUBLIC METHODS

/**
 * Compares between the coordinates remembered by bookmarks.at(seq -1) and the current location.
 * If it turns out that they are different, then the player has passed through one of the borders.
 * The method uses this information in order to narrow down the size of the mappedMaze.
 * The information from cells with wrong coordinates is then transfered to the cells with the corresponding correct coordinates.
 */
void _311246755_a::hitBookmark(int seq) {
	if (!isDirectionChosen) return; //the player hit the bookmark because he was backtracking, so there's nothing to deduce
	
	//choose another direction next time
	isDirectionChosen = false;


	auto b = bookmarks.at(seq - 1);
	int bRow = b.first, bCol = b.second;
	if (curRow == bRow && curCol == bCol) return;
	if (curRow != bRow) {
		if (isHeightKnown) return; //there's nothing to deduce from this bookmark hit.
		else {
			const int mazeHeight = setHeightBounds(seq);
			if (curRow < highestKnownRow) {
				fixRows(mazeHeight);
				fixBookmarks(-1, mazeHeight);
			}
		}
	}
	if(curCol != bCol){
		if (isWidthKnown) return; //there's nothing to deduce from this bookmark hit.
		else {
			const int mazeWidth = setWidthBounds(seq);
			if (curCol < leftmostKnownCol) {
				fixCols(mazeWidth);
				fixBookmarks(mazeWidth, -1);
			}
		}
	}
}


/**
 * Cancels the last direction change.
 * Updates the cell's obstacle to MazeObstacle::WALL and removes the last direction change from path.
 */
void _311246755_a::hitWall() {
	updateCellObstacle(curRow, curCol, MazeObstacle::WALL);
	isDirectionChosen = false;
	movedLastTurn = false;
	Move lastDir = path.top();
	undoMove(lastDir);
	path.pop();
	if (!mappedMaze[curRow][curCol].hasBookmark) { //check if there is a bookmark at the same location as the player, and place one if there isn't
		placedBookmark = false;
	}
}



/**
 * Chooses the next move, updates the map, bookmarks, and path.
 */
AbstractAlgorithm::Move _311246755_a::move() {
	MazeCell &cell = mappedMaze[curRow][curCol];
	Move nextMove;

	// change current cell.obstacle to SPACE, as the player is on it
	if (cell.obstacle == MazeObstacle::UNKNOWN) {
		updateCellObstacle(curRow, curCol, MazeObstacle::SPACE);
	}

	//update the directions taken from current cell
	if (movedLastTurn) {
		updateTriedFromOrigin(cell);
	}

	//choose the next move. try to avoid walls and cells that already tried the chosen direction.
	for(int i = 0; i < 4; i++) {
		if(i > 0) isDirectionChosen = false;
		nextMove = chooseMove();
		auto nextLoc = calcNextLocation(nextMove);
		if (isBacktracking || nextMove == Move::BOOKMARK || !doesCellExist(nextLoc)) break;
		MazeCell& cell = mappedMaze[nextLoc.first][nextLoc.second];
		if (cell.obstacle != MazeObstacle::WALL && !cell.triedDirection[nextMove]) break;
	}
	
	//update the player's location, add direction to path, and update map. do all of this only if the next direction is not a bookmark
	if (nextMove != Move::BOOKMARK) {
		updateLocation(nextMove);
		if (!doesCellExist({ curRow, curCol })) {
			addNewCell(curRow, curCol, MazeObstacle::UNKNOWN);
		}
		updateKnownDimentions(nextMove);
		if (!isBacktracking) {
			path.push(nextMove);
		}
		movedLastTurn = true;
	}
	else { // nextMove == Move::BOOKMARK
		bookmarks.push_back({ curRow, curCol });
		movedLastTurn = false;
	}
	return nextMove;
}
