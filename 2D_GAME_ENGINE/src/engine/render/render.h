#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../types.h"

SDL_Window* render_init(u32 width, u32 height, const char* window_name);
void render_begin(void);
void render_end(void);

void render_quad(vec2 position, vec2 size, vec4 colour, u32 texture);
void render_line(vec2 start, vec2 end, vec4 colour, u32 line_width);

u32 render_load_texture(const char* path, bool flip);