#pragma once
#include <vector>
#include <SDL.h>

namespace Snake
{
	class Renderer
	{
		SDL_Renderer * renderer;
		SDL_Window* window;

	public:
		Renderer(int scale, int wscale);
		~Renderer();
		void renderPlayer(SDL_Rect player, int x, int y, int scale, std::vector<int> tailX, std::vector<int> tailY, int tailLength) const;
		void renderFood(SDL_Rect food) const;
		void renderScore(int tailLength, int scale, int wScale) const;
		void gameOver(SDL_Event event, int scale, int wScale, int tailLength) const;

		void renderGame() const;
	};
}
