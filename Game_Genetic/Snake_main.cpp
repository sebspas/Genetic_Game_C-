
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include <SDL.h> //Main SDL library
#include <SDL_ttf.h> //Optional SDL library used to display text using renderers
#include "Renderer.h"

using namespace std;

bool checkCollision(int foodx, int foody, int playerx, int playery) {
	//Very simple, if x and y of the player block is the same as a food block
	if (playerx == foodx && playery == foody) {
		//Then return true
		return true;
	}
	return false;
}

pair <int, int> getFoodSpawn(vector <int> tailX, vector <int> tailY, int playerX, int playerY, int scale, int wScale, int tailLength) {
	//Get a valid spawn for the food which is not on top of a tail or player block
	bool valid = false;
	int x = 0;
	int y = 0;
	while (!valid) {
		srand(time(0));
		x = scale * (rand() % wScale);
		y = scale * (rand() % wScale);
		valid = true;
		//Check all tail blocks and player block
		for (int i = 0; i < tailLength; i++) {
			if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY)) {
				valid = false;
			}
		}

	}
	//Create and return a pair of the two position values
	pair <int, int> foodLoc;
	foodLoc = make_pair(x, y);

	return foodLoc;
}

int main(int argc, char* argv[]) {

	//Init everything so we have everything
	SDL_Init(SDL_INIT_EVERYTHING);

	//Init TTF and check for any errors
	if (TTF_Init() < 0) {
		cout << "Error: " << TTF_GetError() << endl;
	}

	SDL_Event event;

	//This is the player rectangle, set all values to 0
	SDL_Rect player;
	player.x = 0;
	player.y = 0;
	player.h = 0;
	player.w = 0;

	//tailLength is incremented every time the snake eats food
	//Used in for loops to determine how many times they have to run
	int tailLength = 0;

	//Vectors for storage of tail block positions
	//Vectors are used because they don't need to have a fixed size, unlike normal arrays
	vector<int> tailX;
	vector<int> tailY;

	//These values change scale of blocks and scale of window (wScale)
	//They are later multiplied with eachother to allow easy change of window size according to block size
	int scale = 24;
	int wScale = 24;

	Snake::Renderer* snake_renderer = new Snake::Renderer(scale, wScale);

	//Default values of player position values
	int x = 0;
	int y = 0;
	int prevX = 0;
	int prevY = 0;

	//Booleans to control the movement
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	//This is the food rectangle, set all values to 0
	//Except for the x and y coordinates, which will be set to something random inside the window area
	SDL_Rect food;
	food.w = scale;
	food.h = scale;
	food.x = 0;
	food.y = 0;

	pair <int, int> foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
	food.x = foodLoc.first;
	food.y = foodLoc.second;

	//Used for delaying movement of blocks
	float time = SDL_GetTicks() / 100;

	while (true) { //Main game loop, this constantly runs and keeps everything updated

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

		//The previous position of the player block, these are used so tail blocks will never be on the player block's position
		prevX = x;
		prevY = y;

		//delta*xMovementPerTick
		//This makes the player move at the same speed regardless of framerate
		//Without this the player would move way too fast
		if (up) {
			y -= delta * scale;
		}
		else if (left) {
			x -= delta * scale;
		}
		else if (right) {
			x += delta * scale;
		}
		else if (down) {
			y += delta * scale;
		}

		//Collision detection, has played collided with food?
		if (checkCollision(food.x, food.y, x, y)) {

			//Spawn new food after it has been eaten
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
			food.x = foodLoc.first;
			food.y = foodLoc.second;

			tailLength++;
		}

		//Only runs in the frames where the player block has moved
		if (delta*scale == 24) {
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

		//Game over if player has collided with a tail block, also reset everything
		for (int i = 0; i < tailLength; i++) {
			if (x == tailX[i] && y == tailY[i]) {
				snake_renderer->gameOver(event, scale, wScale, tailLength);
				x = 0;
				y = 0;
				up = false;
				left = false;
				right = false;
				down = false;
				tailX.clear();
				tailY.clear();
				tailLength = 0;
				foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
				food.x = foodLoc.first;
				food.y = foodLoc.second;
			}
		}

		//Game over if player out of bounds, also reset everything
		if (x < 0 || y < 0 || x > scale*wScale - scale || y > scale*wScale - scale) {
			snake_renderer->gameOver(event, scale, wScale, tailLength);
			x = 0;
			y = 0;
			up = false;
			left = false;
			right = false;
			down = false;
			tailX.clear();
			tailY.clear();
			tailLength = 0;
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
			food.x = foodLoc.first;
			food.y = foodLoc.second;
		}

		//Render everything
		snake_renderer->renderFood(food);
		snake_renderer->renderPlayer(player, x, y, scale, tailX, tailY, tailLength);
		snake_renderer->renderScore(tailLength, scale, wScale);

		// render everything on the screen
		snake_renderer->renderGame();
	}

	TTF_Quit();

	SDL_Quit();

	return 0;

}