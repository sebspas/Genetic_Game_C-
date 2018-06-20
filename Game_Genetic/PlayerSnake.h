#pragma once
#include <SDL.h>
#include <vector>

namespace Snake
{
	class PlayerSnake
	{
		int x, y;
		int prevX, prevY;
		int tailLength;

		std::vector<int> tailX;
		std::vector<int> tailY;

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

		void resetPlayer();

		void update(bool up, bool left, bool right, bool down, float deltaTime, int scale);
		void updateTail();

		bool collisionWithPlayerTail(int objX, int objY);
	};

}

