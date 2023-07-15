#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "../types.h"

SDL_Window* render_init(u32 width, u32 height, const char* window_name);
void render_begin(void);
void render_end(void);

void render_quad(void);
void render_line(void);