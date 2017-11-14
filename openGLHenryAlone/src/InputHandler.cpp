#include "InputHandler.h"
#include <iostream>

InputHandler::InputHandler()
{
	SDL_PumpEvents();
	this->m_keyboardState = SDL_GetKeyboardState(NULL);
	for (size_t i = 0; i < NUM_BUTTONS; i++)
	{
		this->m_mouseButtons[i] = false;
	}

	this->m_angleYaw = -90.0f;
	this->m_anglePitch = 0.0f;

}

void InputHandler::Update(Camera* camPtr)
{
	SDL_PumpEvents();
	this->m_keyboardState = SDL_GetKeyboardState(NULL);
		if (this->m_keyboardState[SDL_SCANCODE_W])
			camPtr->MoveForward(SENSITIVITY);
		if (this->m_keyboardState[SDL_SCANCODE_S])
			camPtr->MoveForward(-SENSITIVITY);
		if (this->m_keyboardState[SDL_SCANCODE_A])
			camPtr->MoveRight(-SENSITIVITY);
		if (this->m_keyboardState[SDL_SCANCODE_D])
			camPtr->MoveRight(SENSITIVITY);
		if (this->m_keyboardState[SDL_SCANCODE_X])
			camPtr->MoveUp(-SENSITIVITY);
		if (this->m_keyboardState[SDL_SCANCODE_SPACE])
			camPtr->MoveUp(SENSITIVITY);

		while (SDL_PollEvent(&this->m_event)) {
			if (this->m_event.type == SDL_MOUSEMOTION) {
				camPtr->UpdateMouseView(glm::vec2(this->m_event.motion.x, this->m_event.motion.y));
			}
		}
	
}


bool InputHandler::IsKeyPressed()
{
	return this->m_event.type == SDL_KEYDOWN;
}

bool InputHandler::IsMouseButtonPressed(int mouseButton)
{
	if (mouseButton <= NUM_BUTTONS && mouseButton >= MOUSE_CENTER)
	{
		return this->m_mouseButtons[mouseButton - 1];
	}
	return false;
}

void InputHandler::GetButtonStates()
{
SDL_PollEvent(&this->m_event);
//	1 = left, 2 = center, 3 = right
if (this->m_event.type == SDL_MOUSEBUTTONDOWN)
{
		this->m_mouseButtons[this->m_event.button.button - 1] = true;
	}

	if (this->m_event.type == SDL_MOUSEBUTTONUP)
	{
		this->m_mouseButtons[this->m_event.button.button - 1] = false;
	}
}

InputHandler::~InputHandler()
{
}
