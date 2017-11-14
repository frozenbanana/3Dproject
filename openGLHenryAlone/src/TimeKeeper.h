#pragma once
#include <SDL2\SDL.h>
#include <iostream>

class TimeKeeper
{
public:
	TimeKeeper()
	{
		this->m_pastTicks = SDL_GetTicks();		// Nr of milliseconds
	}

	void SetTimer() { this->m_pastTicks = SDL_GetTicks(); };
	unsigned int GetTimeElapsed() { return SDL_GetTicks() - this->m_pastTicks; };

private:
	unsigned int m_pastTicks;
};