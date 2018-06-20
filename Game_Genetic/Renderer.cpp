#include "stdafx.h"
#include "Renderer.h"
#include <SDL_ttf.h>
#include <iostream>
#include <string>

using namespace Snake;

Renderer::Renderer(int scale = 24, int wScale = 24)
{
	//Show the window with these settings and apply a renderer to it
	//The renderer is responsible for all graphics being displayed
	window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale*wScale, scale*wScale, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}


Renderer::~Renderer()
{
	SDL_DestroyWindow(window);
}

void Renderer::renderPlayer(SDL_Rect player, int x, int y, int scale, std::vector<int> tailX, std::vector<int> tailY, int tailLength) const
{
	//Setting color before rendering, needs to be set first each time or the block could get a color from another block
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	player.w = scale;
	player.h = scale;

	//Gets x and y of all tail blocks and renders them
	for (int i = 0; i < tailLength; i++) {
		player.x = tailX[i];
		player.y = tailY[i];
		SDL_RenderFillRect(renderer, &player);
	}

	player.x = x;
	player.y = y;

	SDL_RenderFillRect(renderer, &player);
}


void Renderer::renderFood(SDL_Rect food) const
{
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &food);
}

void Renderer::renderScore(int tailLength, int scale, int wScale) const
{
	SDL_Color Black = { 0, 0, 0 };
	//Get the font used for displaying text
	TTF_Font* font = TTF_OpenFont((char*)"arial.ttf", 10);
	if (font == NULL) {
		//font not loaded? print an error and return
		std::cout << "Font loading error" << std::endl;
		return;
	}
	//c_str() allows normal strings to be rendered here, otherwise the rendered text would have to be a const char* type
	//Const char* values cannot be changed after initialization, but we need dynamic text here
	SDL_Surface* score = TTF_RenderText_Solid(font, (std::string("Score: ") + std::to_string(tailLength * 10)).c_str(), Black);
	SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
	SDL_Rect scoreRect;
	scoreRect.w = 100;
	scoreRect.h = 30;
	scoreRect.x = ((scale*wScale) / 2) - (scoreRect.w / 2);
	scoreRect.y = 0;
	SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);
	//Close font before finishing so this font doesn't collide with any other function's font
	TTF_CloseFont(font);
}

void Renderer::gameOver(SDL_Event event, int scale, int wScale, int tailLength) const {
	SDL_Color Red = { 255, 0, 0 };
	SDL_Color White = { 255, 255, 255 };
	SDL_Color Black = { 0, 0, 0 };
	//Get the font used for displaying text
	TTF_Font* font = TTF_OpenFont((char*)"arial.ttf", 10);
	if (font == NULL) {
		//font not loaded? print an error and return
		std::cout << "Font loading error" << std::endl;
		return;
	}

	SDL_Surface* gameover = TTF_RenderText_Solid(font, "Game Over", Red);
	SDL_Surface* retry = TTF_RenderText_Solid(font, "Press Enter to retry", White);

	//c_str() allows normal strings to be rendered here, otherwise the rendered text would have to be a const char* type
	//Const char* values cannot be changed after initialization, but we need dynamic text here
	//We re-render the score in the game over screen so the player can still see their score
	SDL_Surface* score = TTF_RenderText_Solid(font, (std::string("Score: ") + std::to_string(tailLength * 10)).c_str(), Black);
	SDL_Texture* gameoverMessage = SDL_CreateTextureFromSurface(renderer, gameover);
	SDL_Texture* retryMessage = SDL_CreateTextureFromSurface(renderer, retry);
	SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
	SDL_Rect gameoverRect;
	SDL_Rect retryRect;
	SDL_Rect scoreRect;
	gameoverRect.w = 200;
	gameoverRect.h = 100;
	gameoverRect.x = ((scale*wScale) / 2) - (gameoverRect.w / 2);
	gameoverRect.y = ((scale*wScale) / 2) - (gameoverRect.h / 2) - 50;
	retryRect.w = 300;
	retryRect.h = 50;
	retryRect.x = ((scale*wScale) / 2) - ((retryRect.w / 2));
	retryRect.y = (((scale*wScale) / 2) - ((retryRect.h / 2)) + 150);
	scoreRect.w = 100;
	scoreRect.h = 30;
	scoreRect.x = ((scale*wScale) / 2) - (scoreRect.w / 2);
	scoreRect.y = 0;
	SDL_RenderCopy(renderer, gameoverMessage, NULL, &gameoverRect);
	SDL_RenderCopy(renderer, retryMessage, NULL, &retryRect);
	SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);
	//Close font before finishing so this font doesn't collide with any other function's font
	TTF_CloseFont(font);

	//Show game over screen while space has not been pressed
	while (true) {
		SDL_RenderPresent(renderer);
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				exit(0);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
				return;
			}
		}
	}
}

void Renderer::renderGame() const
{
	//Put everything on screen
	//Nothing is actually put on screen until this is called
	SDL_RenderPresent(renderer);

	//Choose a color and fill the entire window with it, this resets everything before the next frame
	//It also creates the background color
	SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
	SDL_RenderClear(renderer);
}
