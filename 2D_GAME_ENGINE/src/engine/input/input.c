#include "input.h"
#include <stdio.h>

static Input states;

void input_init(void) {
	states.quit = false;

	for (u16 i = 0; i < SDL_NUM_SCANCODES; i++)
		states.keys[i] = INPUT_UNPRESSED;
}

static void input_update_mouse_button(bool state, u16 button) {
	if (state && states.mouse_buttons[button] == INPUT_UNPRESSED)
		states.mouse_buttons[button] = INPUT_PRESSED;
	else if (state && states.mouse_buttons[button] == INPUT_PRESSED)
		states.mouse_buttons[button] = INPUT_HELD;
	else if (!state && states.mouse_buttons[button] == INPUT_HELD)
		states.mouse_buttons[button] = INPUT_RELEASED;
	else if (!state && states.mouse_buttons[button] == INPUT_RELEASED)
		states.mouse_buttons[button] = INPUT_UNPRESSED;
}

void input_poll_events(void) {
	u8* keyboard = SDL_GetKeyboardState(NULL);
	for (usize i = 0; i < SDL_NUM_SCANCODES; i++)
		if (keyboard[i] && states.keys[i] == INPUT_UNPRESSED)
			states.keys[i] = INPUT_PRESSED;
		else if (keyboard[i] && states.keys[i] == INPUT_PRESSED)
			states.keys[i] = INPUT_HELD;
		else if (!keyboard[i] && states.keys[i] == INPUT_HELD)
			states.keys[i] = INPUT_RELEASED;
		else if (!keyboard[i] && states.keys[i] == INPUT_RELEASED)
			states.keys[i] = INPUT_UNPRESSED;
	
	u32 button_mask = SDL_GetMouseState(&states.mouse_x, states.mouse_y);
	input_update_mouse_button(SDL_BUTTON(SDL_BUTTON_LEFT), SDL_BUTTON_LEFT);
	input_update_mouse_button(SDL_BUTTON(SDL_BUTTON_RIGHT), SDL_BUTTON_RIGHT);
	input_update_mouse_button(SDL_BUTTON(SDL_BUTTON_MIDDLE), SDL_BUTTON_MIDDLE);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			states.quit = true;
		}
	}
}

InputState input_get_key_state(SDL_Scancode code) {
	return states.keys[code];
}

InputState input_get_mouse_state(u16 button) {
	return states.mouse_buttons[button];
}

void input_get_mouse_position(u32* x, u32* y) {
	x = states.mouse_x;
	y = states.mouse_y;
}

bool input_get_quit_state(void) {
	return states.quit;
}