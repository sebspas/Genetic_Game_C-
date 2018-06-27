#pragma once
#include <vector>

namespace Snake
{
	enum Directions
	{
		LEFT,
		UP,
		RIGHT,
		DOWN,
		NONE
	};

	class PlayerSnake
	{
		int x, y;
		int prevX, prevY;
		int tailLength;

		std::vector<int> tailX;
		std::vector<int> tailY;

		Directions forward;
		Directions prevDir;

	public:
		PlayerSnake();
		~PlayerSnake();

		void setPrevX(int x) { this->prevX = x; }
		void setPrevY(int y) { this->prevY = y; }

		int getX() const { return x; }
		void setX(int x) { this->x = x; }

		int getY() const { return y; }
		void setY(int y) { this->y = y; }

		int getTailLength() const { return tailLength; }
		void incTailLenght() { this->tailLength++; }

		int getTailXAt(int i) const { return tailX[i]; }
		int getTailYAt(int i) const { return tailY[i]; }

		void setForward(const Directions dir)
		{
			this->prevDir = this->forward;
			this->forward = dir;
		}
		Directions getForward() const { return this->forward; }

		void setPrevDir(const Directions dir) { this->prevDir = dir; }
		Directions getPrevDir() const { return this->prevDir; }

		void resetPlayer();

		void update(Directions forward, float deltaTime, int scale);
		void updateTail();

		bool collisionWithPlayerTail(int objX, int objY);

	};

}

