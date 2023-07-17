#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../types.h"

typedef struct {
	u32 texture_id;
	vec2 scale;
	vec4 colour;
} Sprite;

void render_load_sprite(Sprite* sprite, const char* path, vec2 scale, vec4 colour);

typedef struct {
	u32 texture_id;
	f32 texture_width;
	f32 texture_height;
	f32 cell_width;
	f32 cell_height;
	vec2 scale;
	vec4 colour;
} SpriteSheet;

void render_load_sprite_sheet(SpriteSheet* sheet, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour);

SDL_Window* render_init(u32 width, u32 height, const char* window_name);
void render_begin(void);
void render_end(void);

void render_quad(vec2 position, vec2 size, vec4 colour, u32 texture);
void render_sprite(Sprite* sprite, vec2 position, vec2 size);
void render_sprite_sheet(SpriteSheet* sheet, u32 row, u32 column, vec2 position, vec2 size);
void render_line(vec2 start, vec2 end, vec4 colour, u32 line_width);

u32 render_load_texture(const char* path, bool flip, f32* width, f32* height);
void render_insert_sprite(usize entity, const char* path, vec2 scale, vec4 colour);
void render_insert_sprite_sheet(usize entity, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour);