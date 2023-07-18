#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../types.h"

#define MAX_ANIMATION_SPRITE_SHEETS 5

typedef struct {
	u32 id;
	u32 width;
	u32 height;
} Texture;

Texture render_load_texture(const char* path, bool flip);

typedef struct {
	Texture texture;
	vec2 scale;
	vec4 colour;
} Sprite;

void render_load_sprite(Sprite* sprite, const char* path, vec2 scale, vec4 colour);

typedef struct {
	Texture texture;
	f32 cell_width;
	f32 cell_height;
	vec2 scale;
	vec4 colour;
} SpriteSheet;

void render_load_sprite_sheet(SpriteSheet* sheet, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour);

typedef struct {
	SpriteSheet sheets[MAX_ANIMATION_SPRITE_SHEETS];
	f32 hold_times[MAX_ANIMATION_SPRITE_SHEETS];
	u32 current_sheet;
	u32 previous_sheet;
	u32 sheets_length;
	u32 row, column;
	u32 rows, columns;
	f32 current_time;
} AnimatedSprite;

AnimatedSprite render_create_animated_sprite(void);
void render_push_animated_sheet(AnimatedSprite* animation, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour, f32 hold_time);
void render_change_animated_sheet(AnimatedSprite* animation, u32 sheet);
void render_update_animation(AnimatedSprite* animation, f32 dt);

SDL_Window* render_init(u32 width, u32 height, const char* window_name);
void render_begin(void);
void render_end(void);
u32 render_get_window_width(void);
u32 render_get_window_height(void);

void render_quad(vec2 position, vec2 size, vec4 colour, u32 texture);
void render_sprite(Sprite* sprite, vec2 position, vec2 size);
void render_sprite_sheet(SpriteSheet* sheet, u32 row, u32 column, vec2 position, vec2 size);
void render_line(vec2 start, vec2 end, vec4 colour, u32 line_width);

Sprite* render_insert_sprite(usize entity, const char* path, vec2 scale, vec4 colour);
SpriteSheet* render_insert_sprite_sheet(usize entity, const char* path, f32 cell_width, f32 cell_height, vec2 scale, vec4 colour);
AnimatedSprite* render_insert_animated_sprite(usize entity);