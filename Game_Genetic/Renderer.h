#pragma once
#include <vector>
#include <SDL.h>
#include "PlayerSnake.h"

namespace Snake
{
	class Renderer
	{
		SDL_Renderer * renderer;
		SDL_Window* window;

		SDL_Rect player_rect;
		int scale, wScale;

	public:
		Renderer(int scale, int wScale);
		~Renderer();
		void renderPlayer(const PlayerSnake* player_snake);
		void renderFood(SDL_Rect food) const;
		void renderScore(int tailLength) const;
		void gameOver(SDL_Event event, int tailLength) const;

		void renderGame() const;

		int getScale() const { return scale; }
		int getWScale() const { return wScale; }
	};
}
