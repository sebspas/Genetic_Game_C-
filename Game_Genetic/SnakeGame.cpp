#include "stdafx.h"
#include "SnakeGame.h"
#include <ctime>
#include <SDL_ttf.h>

using namespace Snake;

SnakeGame::SnakeGame(int scale, int wScale)
	: renderer(new Renderer(scale, wScale)), playerSnake(new PlayerSnake())
{
	food.w = scale;
	food.h = scale;
	food.x = 0;
	food.y = 0;

	foodLoc = getFoodSpawn(playerSnake);
	food.x = foodLoc.first;
	food.y = foodLoc.second;

	//Used for delaying movement of blocks
	time = SDL_GetTicks() / 100;
}


SnakeGame::~SnakeGame()
{
	TTF_Quit();
	SDL_Quit();
}

void SnakeGame::update()
{
	//Used to delay the movement of blocks
	float newTime = SDL_GetTicks() / 75; //This value (75) is the speed at which the blocks are updated
	float delta = newTime - time;
	time = newTime;

	//Controls
	if (SDL_PollEvent(&event)) {
		//If user tries to exit window, exit program
		if (event.type == SDL_QUIT) {
			exit(0);
		}
		//If a key is pressed
		if (event.type == SDL_KEYDOWN) {
			//Then check for the key being pressed and change direction accordingly
			if (!down && event.key.keysym.scancode == SDL_SCANCODE_UP) {
				up = true;
				left = false;
				right = false;
				down = false;
			}
			else if (!right && event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
				up = false;
				left = true;
				right = false;
				down = false;
			}
			else if (!up && event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
				up = false;
				left = false;
				right = false;
				down = true;
			}
			else if (!left && event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
				up = false;
				left = false;
				right = true;
				down = false;
			}
		}
	}

	playerSnake->update(up,left,right,down, delta, renderer->getScale());

	//Collision detection, has played collided with food?
	if (checkCollision(food.x, food.y, playerSnake->getX(), playerSnake->getY())) {
		spawnFood();
		playerSnake->incTailLenght();
	}

	//Only runs in the frames where the player block has moved
	if (delta*renderer->getScale() == 24) {
		playerSnake->updateTail();
	}

	//Game over if player has collided with a tail block, also reset everything
	//Game over if player out of bounds, also reset everything
	if (playerSnake->collisionWithPlayerTail(playerSnake->getX(), playerSnake->getY())
		|| outOfBounds(playerSnake->getX(), playerSnake->getY(), renderer->getScale(), renderer->getWScale()))
	{
		renderer->gameOver(event, playerSnake->getTailLength());
		playerSnake->resetPlayer();
		up = false;
		left = false;
		right = false;
		down = false;
		spawnFood();
	}

	//Render everything
	renderer->renderFood(food);
	renderer->renderPlayer(playerSnake);
	renderer->renderScore(playerSnake->getTailLength());

	// render everything on the screen
	renderer->renderGame();
}

bool SnakeGame::checkCollision(int objx, int objy, int playerx, int playery)
{
	return playerx == objx && playery == objy;
}

bool SnakeGame::outOfBounds(int x, int y, int scale, int wScale)
{
	return  x < 0 || y < 0 || x > scale*wScale - scale || y > scale*wScale - scale;
}

std::pair<int, int> SnakeGame::getFoodSpawn(const PlayerSnake* player_snake) const
{
	//Get a valid spawn for the food which is not on top of a tail or player block
	bool valid = false;
	int x = 0;
	int y = 0;
	while (!valid) {
		srand(std::time(0));
		x = renderer->getScale() * (rand() % renderer->getWScale());
		y = renderer->getScale() * (rand() % renderer->getWScale());
		valid = true;
		//Check all tail blocks and player block
		for (int i = 0; i < player_snake->getTailLength(); i++) {
			if ((x == player_snake->getTailXAt(i) && y == player_snake->getTailYAt(i)) || (x == player_snake->getX() && y == player_snake->getY())) {
				valid = false;
			}
		}
	}

	return std::make_pair(x, y);
}

void SnakeGame::spawnFood()
{
	foodLoc = getFoodSpawn(playerSnake);
	food.x = foodLoc.first;
	food.y = foodLoc.second;
}
