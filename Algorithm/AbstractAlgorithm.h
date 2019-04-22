#pragma once

class AbstractAlgorithm {
public:
	virtual ~AbstractAlgorithm() {}
	virtual int move() = 0;
	virtual void hitWall() = 0;
	virtual void hitBookmark(int seq) = 0;
}; 