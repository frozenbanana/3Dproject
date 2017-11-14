#pragma once
#include <SDL2\SDL.h>
#include "Camera.h"

class InputHandler
{
public:
	InputHandler();
	void Update(Camera* camPtr);
	bool IsKeyPressed();
	bool IsMouseButtonPressed(int mouseButton);
	~InputHandler();

private:
	enum
	{
		MOUSE_LEFT,
		MOUSE_CENTER,
		MOUSE_RIGHT,

		NUM_BUTTONS
	};

	bool m_mouseButtons[NUM_BUTTONS];
	const Uint8* m_keyboardState;		// Array to check the states of the keys on keyboard

	float m_anglePitch, m_angleYaw;
	SDL_Event m_event;

	void GetButtonStates();
};

