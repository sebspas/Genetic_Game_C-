#pragma once
#include "Renderer.h"
#include "PlayerSnake.h"
#include "SnakeRecorder.h"
#include "Net.h"

namespace Snake {

	class SnakeGame
	{
		PlayerSnake* playerSnake;
		Renderer* renderer;
		SDL_Event event;

		SDL_Rect food;
		std::pair <int, int> foodLoc;

		float time;

		bool record = false;
		SnakeRecorder* snake_recorder;

		bool AIPlaying = false;
		NeuralNetwork::Net* neural_network;

	public:
		SnakeGame(int scale, int wScale, bool record, bool AIPlaying);
		~SnakeGame();

		void update();
		void getPlayerInputs();

		static bool checkCollision(int x, int y, int playerx, int playery);
		static bool outOfBounds(int x, int y, int scale, int wScale);
		static int checkTypeOfObject(int x, int y, int foodx, int foody, int scale, int wScale);

		std::pair <int, int> getFoodSpawn(const PlayerSnake* player_snake) const;

		std::vector<double> checkPosRelativeToSnake(const Directions forward) const;

		Directions snakeToWorld(const Directions outputNeural) const;
		Directions worldToSnake(const Directions oldSnake, const Directions NewSnake) const;
		
	private:
		void spawnFood();
		void enableNeuralNetwork();

		void getInputFromNeuralNetwork();
	};
}
