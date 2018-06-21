#pragma once
#include "Renderer.h"
#include "PlayerSnake.h"
#include "SnakeRecorder.h"
#include "Net.h"

namespace Snake {

	enum Directions
	{
		LEFT,
		UP,
		RIGHT,
		DOWN
	};

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

		bool record = false;
		bool started = false;
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

		
	private:
		void spawnFood();
		void enableNeuralNetwork();

		void getInputFromNeuralNetwork();
	};
}
