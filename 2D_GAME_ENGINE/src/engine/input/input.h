#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../containers/map/map.h"
#include "../types.h"

typedef enum {
	INPUT_PRESSED,
	INPUT_HELD,
	INPUT_RELEASED,
	INPUT_UNPRESSED
} InputState;

typedef struct {
	u16 keys[SDL_NUM_SCANCODES];
	u16 mouse_buttons[3];
	u32 mouse_x, mouse_y;
	bool quit;
} Input;

void input_init(void);
void input_poll_events(void);
InputState input_get_key_state(SDL_Scancode code);
InputState input_get_mouse_state(u16 button);
void input_get_mouse_position(u32* x, u32* y);
bool input_get_quit_state(void);