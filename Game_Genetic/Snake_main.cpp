
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include <SDL.h> //Main SDL library
#include <SDL_ttf.h> //Optional SDL library used to display text using renderers
#include "SnakeGame.h"
#include "PlayerSnake.h"

int main(int argc, char* argv[]) {

	//Init everything so we have everything
	SDL_Init(SDL_INIT_EVERYTHING);

	//Init TTF and check for any errors
	if (TTF_Init() < 0) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}

	//These values change scale of blocks and scale of window (wScale)
	//They are later multiplied with eachother to allow easy change of window size according to block size
	int scale = 24;
	int wScale = 24;

	auto player_snake = new Snake::PlayerSnake();
	auto snake_game = new Snake::SnakeGame(scale, wScale);

	while (true) { //Main game loop, this constantly runs and keeps everything updated
		snake_game->update();
	}

	return 0;
}
