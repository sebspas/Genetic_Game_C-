#include "stdafx.h"
#include "SnakeGame.h"
#include <ctime>
#include <SDL_ttf.h>
#include <iostream>

using namespace Snake;

SnakeGame::SnakeGame(int scale, int wScale, bool record, bool AIPlaying)
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
	time = SDL_GetTicks() / 150;

	this->record = record;
	if (record)
		snake_recorder = new SnakeRecorder("training_data_snake.txt");

	this->AIPlaying = AIPlaying;
	if (AIPlaying)
		enableNeuralNetwork();
}


SnakeGame::~SnakeGame()
{
	TTF_Quit();
	SDL_Quit();
}

void SnakeGame::getPlayerInputs()
{
	//Controls
	if (SDL_PollEvent(&event)) {
		//If user tries to exit window, exit program
		if (event.type == SDL_QUIT) {
			exit(0);
		}
		//If a key is pressed
		if (event.type == SDL_KEYDOWN) {
			started = true;
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
}

void SnakeGame::update()
{
	//Used to delay the movement of blocks
	float newTime = SDL_GetTicks() / 90; //This value (75) is the speed at which the blocks are updated
	float delta = newTime - time;
	time = newTime;

	if (AIPlaying && neural_network != nullptr)
	{
		getInputFromNeuralNetwork();
	} else
	{
		getPlayerInputs();
	}

	if (record && started && delta*renderer->getScale() == 24)
	{
		int left_obj = checkTypeOfObject(playerSnake->getX() - 1, playerSnake->getY(), food.x, food.y, renderer->getScale(), renderer->getWScale());
		int right_obj = checkTypeOfObject(playerSnake->getX() + 1, playerSnake->getY(), food.x, food.y, renderer->getScale(), renderer->getWScale());
		int front_obj = checkTypeOfObject(playerSnake->getX(), playerSnake->getY() - 1, food.x, food.y, renderer->getScale(), renderer->getWScale());

		int		   output = UP;
		if (right) output = RIGHT;
		if (left)  output = LEFT;
		if (down)  output = DOWN;

		//Game over if player out of bounds, also reset everything
		if (!playerSnake->collisionWithPlayerTail(playerSnake->getX(), playerSnake->getY())
			&& !outOfBounds(playerSnake->getX(), playerSnake->getY(), renderer->getScale(), renderer->getWScale()))
		{
			snake_recorder->writeData(
				left_obj,
				front_obj,
				right_obj,
//				(playerSnake->getX() - foodLoc.first) / renderer->getScale(),
				//(playerSnake->getY() - foodLoc.second) / renderer->getScale(),
				output);
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
		started = false;
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

int SnakeGame::checkTypeOfObject(int x, int y, int foodx, int foody, int scale, int wScale)
{
	if (foodx == x && foody == y)
		return 1; // food
	if (outOfBounds(x, y, scale, wScale))
		return -1; // wall

	return 0; // nothing
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

void SnakeGame::enableNeuralNetwork()
{
	NeuralNetwork::TrainingData train_data("training_data_snake.txt");

	std::vector<unsigned> topology;
	train_data.getTopology(topology);

	neural_network = new NeuralNetwork::Net(topology);

	neural_network->trainNetwork(&train_data);
}

void SnakeGame::getInputFromNeuralNetwork()
{
	if (SDL_PollEvent(&event)) {
		//If user tries to exit window, exit program
		if (event.type == SDL_QUIT) {
			exit(0);
		}
		if(event.key.keysym.scancode == SDL_SCANCODE_A)
		{
			renderer->gameOver(event, playerSnake->getTailLength());
			playerSnake->resetPlayer();
			up = false;
			left = false;
			right = false;
			down = false;
			spawnFood();
			started = false;
		}
	}

	int left_obj  = checkTypeOfObject(playerSnake->getX() - 1, playerSnake->getY(), food.x, food.y, renderer->getScale(), renderer->getWScale());
	int right_obj = checkTypeOfObject(playerSnake->getX() + 1, playerSnake->getY(), food.x, food.y, renderer->getScale(), renderer->getWScale());
	int front_obj = checkTypeOfObject(playerSnake->getX(), playerSnake->getY() - 1, food.x, food.y, renderer->getScale(), renderer->getWScale());
//	int deltaFoodPlayerX = (playerSnake->getX() - foodLoc.first) /renderer->getScale();
//	int deltaFoodPlayerY = (playerSnake->getY() - foodLoc.second) / renderer->getScale();

	std::vector<double> inputs;
	inputs.push_back(left_obj);
	inputs.push_back(front_obj);
	inputs.push_back(right_obj);
//	inputs.push_back(deltaFoodPlayerX);
//	inputs.push_back(deltaFoodPlayerY);

	std::vector<double> outputs;

	neural_network->feed_forward(inputs);
	neural_network->get_results(outputs);

	for (unsigned i = 0; i < inputs.size(); ++i) {
		std::cout << inputs[i] << " ";
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < outputs.size(); ++i) {
		std::cout << outputs[i] << " ";
	}
	while(true) {}

	/*std::cout << "Before round : " << outputs[0] << std::endl;
	int output = round(outputs[0]);
	
	std::cout << "After round : " << output << std::endl;

	if (output == RIGHT)
	{
		std::cout << "right" << std::endl;
		// right
		right = true;
		left = down = up = false;
	} else if (output == DOWN)
	{
		std::cout << "down" << std::endl;
		// down
		down = true;
		left = right = up = false;
	} else if (output == LEFT)
	{
		std::cout << "left" << std::endl;
		// left
		left = true;
		down = right = up = false;
	} else if (output == UP)
	{
		std::cout << "up" << std::endl;
		// up
		up = true;
		down = right = left = false;
	}*/
}

void SnakeGame::spawnFood()
{
	foodLoc = getFoodSpawn(playerSnake);
	food.x = foodLoc.first;
	food.y = foodLoc.second;
}
