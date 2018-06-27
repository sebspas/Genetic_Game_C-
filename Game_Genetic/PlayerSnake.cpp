#include "stdafx.h"
#include "PlayerSnake.h"

using namespace Snake;

PlayerSnake::PlayerSnake()
{
	x = y = prevX = prevY = tailLength = 0;
	forward = DOWN;
}


PlayerSnake::~PlayerSnake()
{
}

void PlayerSnake::resetPlayer()
{
	x = 0;
	y = 0;
	tailX.clear();
	tailY.clear();
	tailLength = 0;
	forward = DOWN;
}

void PlayerSnake::update(Directions forward, float deltaTime, int scale)
{
	//The previous position of the player block, these are used so tail blocks will never be on the player block's position
	prevX = x;
	prevY = y;

	//delta*xMovementPerTick
	//This makes the player move at the same speed regardless of framerate
	//Without this the player would move way too fast
	if (forward == UP) {
		y -= deltaTime * scale;
	}
	else if (forward == LEFT) {
		x -= deltaTime * scale;		
	}
	else if (forward == RIGHT) {
		x += deltaTime * scale;
	}
	else if (forward == DOWN) {
		y += deltaTime * scale;
	}
}

void PlayerSnake::updateTail()
{
	//Update tail size and position
	//If the size of the tail isn't equal to the actual size, add another block to the tail (this needs to be first)
	//If this is not first the program will later attempt to check and move an amount of tail blocks equal to the tailLength -
	//(which is constantly updated), although the extra tail block hasn't been added, causing a crash
	if (tailX.size() != tailLength) {
		tailX.push_back(prevX);
		tailY.push_back(prevY);
	}

	//Loop through every tail block, move all blocks to the nearest block in front
	//This updates the blocks from end (farthest from player block) to the start (nearest to player block)
	for (int i = 0; i < tailLength; i++) {
		if (i > 0) {
			tailX[i - 1] = tailX[i];
			tailY[i - 1] = tailY[i];
		}
	}

	//If the tail has grown, change the nearest tail block position to the player block position (needs to be last, otherwise, the 2 first blocks stack)
	//The tail blocks will stack because the 2nd block will always have an updated position to the first block
	//While the third block and the rest have an outdated position to the block infront
	if (tailLength > 0) {
		tailX[tailLength - 1] = prevX;
		tailY[tailLength - 1] = prevY;
	}
}

bool PlayerSnake::collisionWithPlayerTail(int objX, int objY)
{
	for (int i = 0; i < tailLength; i++) {
		if (x == tailX[i] && y == tailY[i])
		{
			return true;
		}
	}

	return false;
}
