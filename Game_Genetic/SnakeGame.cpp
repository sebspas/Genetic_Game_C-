#include "stdafx.h"
#include "SnakeGame.h"
#include <ctime>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>

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
			//Then check for the key being pressed and change direction accordingly
			if (playerSnake->getForward() != DOWN && event.key.keysym.scancode == SDL_SCANCODE_UP) {
				playerSnake->setForward(UP);
			}
			else if (playerSnake->getForward() != RIGHT && event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
				playerSnake->setForward(LEFT);
			}
			else if (playerSnake->getForward() != UP && event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
				playerSnake->setForward(DOWN);
			}
			else if (playerSnake->getForward() != LEFT && event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
				playerSnake->setForward(RIGHT);
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

	if (record && delta*renderer->getScale() == 24)
	{
		
		int deltaFoodX = 0, deltaFoodY = 0;
		if (playerSnake->getX() - foodLoc.first > 0) deltaFoodX = 1;
		if (playerSnake->getX() - foodLoc.first < 0) deltaFoodX = -1;
		if (playerSnake->getY() - foodLoc.second > 0) deltaFoodY = 1;
		if (playerSnake->getY() - foodLoc.second < 0) deltaFoodY = -1;


		std::vector<double> caseRelativeToSnake = checkPosRelativeToSnake(playerSnake->getForward());

		//Game over if player out of bounds, also reset everything
		if (!outOfBounds(playerSnake->getX(), playerSnake->getY(), renderer->getScale(), renderer->getWScale()))
		{
			snake_recorder->writeData(
				caseRelativeToSnake[0], // LEFT to the snake
				caseRelativeToSnake[1], // Forward to the snake
				caseRelativeToSnake[2], // Right to the snake
				deltaFoodX,
				deltaFoodY,
				worldToSnake(playerSnake->getPrevDir(), playerSnake->getForward())
			);
		}
	}
		
	playerSnake->update(playerSnake->getForward(), delta, renderer->getScale());

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

int SnakeGame::checkTypeOfObject(int x, int y, int foodx, int foody, int scale, int wScale)
{
	if (foodx == x && foody == y)
		return 2; // food
	if (outOfBounds(x, y, scale, wScale))
		return 1; // wall

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

std::vector<double> SnakeGame::checkPosRelativeToSnake(const Directions forward) const
{
	int relLeftPosX = 0, relLeftPosY = 0, relRightPosX = 0, relRightPosY = 0, relForwardPosX = 0, relForwardPosY = 0;

	switch (forward)
	{
		case RIGHT:
			relLeftPosX = playerSnake->getX();
			relLeftPosY = playerSnake->getY()-1;
			relRightPosX = playerSnake->getX();
			relRightPosY = playerSnake->getY()+1;
			relForwardPosX = playerSnake->getX()+1;
			relForwardPosY = playerSnake->getY();
			break;
		case LEFT:
			relLeftPosX = playerSnake->getX();
			relLeftPosY = playerSnake->getY()+1;
			relRightPosX = playerSnake->getX();
			relRightPosY = playerSnake->getY()-1;
			relForwardPosX = playerSnake->getX()-1;
			relForwardPosY = playerSnake->getY();
			break;
		case UP:
			relLeftPosX = playerSnake->getX()-1;
			relLeftPosY = playerSnake->getY();
			relRightPosX = playerSnake->getX()+1;
			relRightPosY = playerSnake->getY();
			relForwardPosX = playerSnake->getX();
			relForwardPosY = playerSnake->getY()-1;
			break;
		case DOWN:
			relLeftPosX = playerSnake->getX()+1;
			relLeftPosY = playerSnake->getY();
			relRightPosX = playerSnake->getX()-1;
			relRightPosY = playerSnake->getY();
			relForwardPosX = playerSnake->getX();
			relForwardPosY = playerSnake->getY()+1;
			break;
	}

	auto result = std::vector<double>();

	result.push_back(checkTypeOfObject(relLeftPosX, relLeftPosY, food.x, food.y, renderer->getScale(), renderer->getWScale()));
	result.push_back(checkTypeOfObject(relForwardPosX, relForwardPosY, food.x, food.y, renderer->getScale(), renderer->getWScale()));
	result.push_back(checkTypeOfObject(relRightPosX, relRightPosY, food.x, food.y, renderer->getScale(), renderer->getWScale()));

	return result;
}

Directions SnakeGame::snakeToWorld(const Directions outputNeural) const
{
	Directions ret = NONE;
	switch (playerSnake->getForward())
	{
	case RIGHT:
		if (outputNeural == 0)
		{
			ret = UP;
		} else if (outputNeural == 1)
		{
			ret = RIGHT;
		} else if (outputNeural == 2)
		{
			ret = DOWN;
		}
		break;
	case LEFT:
		if (outputNeural == 0)
		{
			ret = DOWN;
		}
		else if (outputNeural == 1)
		{
			ret = LEFT;
		}
		else if (outputNeural == 2)
		{
			ret = UP;
		}
		break;
	case UP:
		if (outputNeural == 0)
		{
			ret = LEFT;
		}
		else if (outputNeural == 1)
		{
			ret = UP;
		}
		else if (outputNeural == 2)
		{
			ret = RIGHT;
		}
		break;
	case DOWN:
		if (outputNeural == 0)
		{
			ret = RIGHT;
		}
		else if (outputNeural == 1)
		{
			ret = DOWN;
		}
		else if (outputNeural == 2)
		{
			ret = LEFT;
		}
		break;
	}

	return ret;
}

Directions SnakeGame::worldToSnake(const Directions oldSnake, const Directions NewSnake) const
{
	Directions ret = NONE;
	switch (oldSnake)
	{
	case RIGHT:
		if (NewSnake == UP)
		{
			ret = LEFT;
		}
		else if (NewSnake == RIGHT)
		{
			ret = UP;
		}
		else if (NewSnake == DOWN)
		{
			ret = RIGHT;
		}
		break;
	case LEFT:
		if (NewSnake == DOWN)
		{
			ret = LEFT;
		}
		else if (NewSnake == LEFT)
		{
			ret = UP;
		}
		else if (NewSnake == UP)
		{
			ret = RIGHT;
		}
		break;
	case UP:
		if (NewSnake == LEFT)
		{
			ret = LEFT;
		}
		else if (NewSnake == UP)
		{
			ret = UP;
		}
		else if (NewSnake == RIGHT)
		{
			ret = RIGHT;
		}
		break;
	case DOWN:
		if (NewSnake == RIGHT)
		{
			ret = LEFT;
		}
		else if (NewSnake == DOWN)
		{
			ret = UP;
		}
		else if (NewSnake == LEFT)
		{
			ret = RIGHT;
		}
		break;
	}

	return ret;
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
			spawnFood();
		}
	}

	std::vector<double> caseRelativeToSnake = checkPosRelativeToSnake(playerSnake->getForward());
	int deltaFoodX = 0, deltaFoodY = 0;
	if (playerSnake->getX() - foodLoc.first > 0) deltaFoodX = 1;
	if (playerSnake->getX() - foodLoc.first < 0) deltaFoodX = -1;
	if (playerSnake->getY() - foodLoc.second > 0) deltaFoodY = 1;
	if (playerSnake->getY() - foodLoc.second < 0) deltaFoodY = -1;
	caseRelativeToSnake.push_back(deltaFoodX);
	caseRelativeToSnake.push_back(deltaFoodY);
	std::vector<double> outputs;

	neural_network->feed_forward(caseRelativeToSnake);
	neural_network->get_results(outputs);

	for (unsigned i = 0; i < caseRelativeToSnake.size(); ++i) {
		std::cout << caseRelativeToSnake[i] << " ";
	}
	std::cout << std::endl;
	for (unsigned i = 0; i < outputs.size(); ++i) {
		std::cout << outputs[i] << " ";
	}
	std::cout << std::endl;

	double max_val = outputs[0];
	int output = 0;
	for (int i = 0; i < outputs.size(); ++i)
	{
		if (outputs[i] > max_val) {
			output = i;
			max_val = outputs[i];
		}
	}

	playerSnake->setForward(snakeToWorld((Directions)output));
}

void SnakeGame::spawnFood()
{
	foodLoc = getFoodSpawn(playerSnake);
	food.x = foodLoc.first;
	food.y = foodLoc.second;
}
