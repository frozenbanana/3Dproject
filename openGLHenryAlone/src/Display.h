#pragma once
#include <SDL2\SDL.h>
#include <GL\glew.h>
#include <string>
#include <iostream>

class Display
{
public:
	Display(int width, int height, const std::string& title);
	void Update();
	bool IsClosed();
	void Clear(float r, float g, float b, float a);
	~Display();
private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;		// Gives GPU control to window from operating system
	bool m_isClosed;
};

