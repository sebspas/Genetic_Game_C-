#pragma once
#include <vector>
#include "Renderer.h"
#include "PlayerSnake.h"

namespace Snake {
	class SnakeGame
	{
		PlayerSnake* playerSnake;
		Renderer* renderer;
		SDL_Event event;

		SDL_Rect food;
		std::pair <int, int> foodLoc;

		float time;

		//Booleans to control the movement
		bool up = false;
		bool down = false;
		bool right = false;
		bool left = false;

	public:
		SnakeGame(int scale, int wScale);
		~SnakeGame();

		void update();

		static bool checkCollision(int x, int y, int playerx, int playery);
		static bool outOfBounds(int x, int y, int scale, int wScale);

		std::pair <int, int> getFoodSpawn(const PlayerSnake* player_snake) const;

	private:
		void spawnFood();
	};
}
